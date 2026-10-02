/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTrackingSystemComparison.h"

#include <algorithm>
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QProcess>
#include <QTextStream>
#include "cxLogger.h"
#include "cxProfile.h"
#include "cxTime.h"
#include "cxTrackingService.h"
#include "cxVideoService.h"
#include "cxXmlOptionItem.h"
#include "cxDefinitions.h"

namespace cx
{

namespace
{
const int kTimerInterval_ms = 100;
const int kCommandStopTimeout_ms = 3000;
const int kStopTimeout_ms = 15000;
const char* kOpenIGTLinkStreamer = "openigtlink3_streamer";
const char* kOpenIGTLinkStartTrackingOption = "start_tracking";
const char* kOpenIGTLinkHostOption = "ip_scanner_openigtlink";
const char* kOpenIGTLinkPortOption = "ip_port";
}

TrackingSystemComparison::TrackingSystemComparison(TrackingServicePtr trackingService, VideoServicePtr videoService) :
	mTrackingService(trackingService),
	mVideoService(videoService),
	mRecorder(trackingService),
	mProcess(nullptr),
	mStep(sIDLE),
	mStepDeadline_ms(0.0),
	mRunIndex(0),
	mSuccess(false),
	mOpenIGTLinkOpened(false)
{
	mTimer.setInterval(kTimerInterval_ms);
	connect(&mTimer, &QTimer::timeout, this, &TrackingSystemComparison::onTimer);
}

TrackingSystemComparison::~TrackingSystemComparison()
{
	this->cancel();
}

void TrackingSystemComparison::start(TrackingComparisonSettings settings)
{
	this->cancel();
	mSettings = settings;
	mRunIndex = 0;
	mSuccess = true;
	mSummary.clear();
	mResults.clear();
	mOriginalToolConfigFile = profile()->getToolConfigFilePath();
	mOriginalConnectionMethod = mVideoService ? mVideoService->getConnectionMethod() : QString();
	QDir().mkpath(mSettings.outputFolder);
	this->log(QString("Comparing %1 tracking system(s), recording %2 s each, output in %3")
			  .arg(mSettings.runs.size()).arg(mSettings.recordDuration_ms / 1000.0).arg(mSettings.outputFolder));
	mTimer.start();
	this->startRun();
}

void TrackingSystemComparison::cancel()
{
	if (this->isRunning())
	{
		mRecorder.stop();
		this->stopRun("cancelled");
		mSuccess = false;
		this->finish();
	}
}

bool TrackingSystemComparison::isRunning() const
{
	return mStep != sIDLE;
}

bool TrackingSystemComparison::wasSuccessful() const
{
	return mSuccess;
}

QString TrackingSystemComparison::getSummary() const
{
	return mSummary.join("\n");
}

QString TrackingSystemComparison::setTrackingSystemImplementation(QString configXml, QString implementation, bool applyReference)
{
	QString retval;
	QDomDocument doc;
	if (doc.setContent(configXml))
	{
		QDomNodeList nodes = doc.elementsByTagName("trackingsystemimplementation");
		for (int i = 0; i < nodes.count(); ++i)
		{
			nodes.at(i).toElement().setAttribute("type", implementation);
			if (implementation == TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK)
			{
				nodes.at(i).toElement().setAttribute("openigtlinkApplyRefToTools", applyReference ? "yes" : "no");
			}
		}
		retval = (nodes.count() > 0) ? doc.toString(4) : QString();
	}
	return retval;
}

QString TrackingSystemComparison::getRunLabel(int index, QString implementation)
{
	return QString("%1_%2").arg(index + 1).arg(implementation);
}

std::map<QString, double> TrackingSystemComparison::getMeanDifferences(const ToolPositionStatisticsMap& first, const ToolPositionStatisticsMap& other)
{
	std::map<QString, double> retval;
	for (const std::pair<const QString, ToolPositionStatistics>& entry : first)
	{
		ToolPositionStatisticsMap::const_iterator match = other.find(entry.first);
		bool comparable = !entry.second.reference && entry.second.positions > 0 && match != other.end() && match->second.positions > 0;
		if (comparable)
		{
			retval[entry.first] = (match->second.mean - entry.second.mean).norm();
		}
	}
	return retval;
}

QStringList TrackingSystemComparison::compareRuns(const std::vector<TrackingComparisonResult>& results)
{
	QStringList retval;
	for (unsigned i = 1; i < results.size(); ++i)
	{
		for (const std::pair<const QString, double>& entry : getMeanDifferences(results[0].statistics, results[i].statistics))
		{
			retval << QString("%1 %2: mean position %3 mm from %4").arg(results[i].label).arg(entry.first)
					  .arg(entry.second, 0, 'f', 3).arg(results[0].label);
		}
	}
	return retval;
}

double TrackingSystemComparison::getMaxMeanDifference_mm() const
{
	double retval = 0.0;
	for (unsigned i = 1; i < mResults.size(); ++i)
	{
		for (const std::pair<const QString, double>& entry : getMeanDifferences(mResults[0].statistics, mResults[i].statistics))
		{
			retval = std::max(retval, entry.second);
		}
	}
	return retval;
}

QStringList TrackingSystemComparison::splitArguments(QString arguments)
{
	QStringList retval;
	QString current;
	bool quoted = false;
	for (const QChar& c : arguments)
	{
		if (c == '"')
		{
			quoted = !quoted;
		}
		else if (c.isSpace() && !quoted)
		{
			if (!current.isEmpty())
			{
				retval << current;
			}
			current.clear();
		}
		else
		{
			current += c;
		}
	}
	if (!current.isEmpty())
	{
		retval << current;
	}
	return retval;
}

TrackingComparisonRun TrackingSystemComparison::currentRun() const
{
	return mSettings.runs[mRunIndex];
}

void TrackingSystemComparison::onTimer()
{
	bool deadlinePassed = getMilliSecondsSinceEpoch() >= mStepDeadline_ms;
	if (mStep == sWAIT_FOR_COMMAND && deadlinePassed)
	{
		this->startTracking();
	}
	else if (mStep == sWAIT_FOR_TRACKING)
	{
		this->checkTrackingStarted();
	}
	else if (mStep == sSETTLE && deadlinePassed)
	{
		this->startRecording();
	}
	else if (mStep == sRECORD && deadlinePassed)
	{
		mRecorder.stop();
		bool gotPositions = false;
		for (const std::pair<const QString, ToolPositionStatistics>& entry : mRecorder.getStatistics())
		{
			gotPositions = gotPositions || (entry.second.positions > 0);
		}
		this->stopRun(gotPositions ? "OK" : "FAILED: no positions recorded");
	}
	else if (mStep == sWAIT_FOR_STOP)
	{
		this->checkTrackingStopped();
	}
	else if (mStep == sPAUSE && deadlinePassed)
	{
		mRunIndex++;
		this->startRun();
	}
}

void TrackingSystemComparison::startRun()
{
	if (mRunIndex >= static_cast<int>(mSettings.runs.size()))
	{
		this->finish();
		return;
	}
	TrackingComparisonRun run = this->currentRun();
	QString label = getRunLabel(mRunIndex, run.implementation);
	this->log(QString("Run %1: %2").arg(label).arg(run.command.isEmpty() ? run.implementation : run.implementation + " via " + run.command));
	QString configFile = mSettings.outputFolder + "/" + label + "_toolconfig.xml";
	if (!this->writeConfiguration(configFile, run.implementation))
	{
		this->stopRun("FAILED: could not write the tool configuration " + configFile);
		return;
	}
	profile()->setToolConfigFilePath(configFile);
	this->startCommand(run);
	this->wait(sWAIT_FOR_COMMAND, run.command.isEmpty() ? 0 : mSettings.commandStartupDelay_ms);
}

void TrackingSystemComparison::startTracking()
{
	if (this->currentRun().implementation.contains(TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK, Qt::CaseInsensitive))
	{
		this->openOpenIGTLinkConnection();
	}
	else
	{
		mTrackingService->setState(Tool::tsTRACKING);
	}
	this->wait(sWAIT_FOR_TRACKING, mSettings.startTimeout_ms);
}

void TrackingSystemComparison::checkTrackingStarted()
{
	if (mTrackingService->getState() >= Tool::tsTRACKING)
	{
		this->wait(sSETTLE, mSettings.settleTime_ms);
	}
	else if (getMilliSecondsSinceEpoch() >= mStepDeadline_ms)
	{
		this->stopRun(QString("FAILED: tracking did not start within %1 s").arg(mSettings.startTimeout_ms / 1000.0));
	}
}

void TrackingSystemComparison::startRecording()
{
	QString label = getRunLabel(mRunIndex, this->currentRun().implementation);
	QString filename = mSettings.outputFolder + "/" + label + "_positions.csv";
	if (mRecorder.start(filename, label))
	{
		this->log("Recording to " + filename);
		this->wait(sRECORD, mSettings.recordDuration_ms);
	}
	else
	{
		this->stopRun("FAILED: could not write " + filename);
	}
}

void TrackingSystemComparison::stopRun(QString result)
{
	QString label = getRunLabel(mRunIndex, this->currentRun().implementation);
	mSuccess = mSuccess && (result == "OK");
	mSummary << QString("%1: %2").arg(label).arg(result);
	if (result == "OK")
	{
		TrackingComparisonResult runResult;
		runResult.label = label;
		runResult.statistics = mRecorder.getStatistics();
		mResults.push_back(runResult);
	}
	QString statistics = mRecorder.getSummary();
	if (!statistics.isEmpty())
	{
		mSummary << statistics;
	}
	this->log(QString("Run %1: %2").arg(label).arg(result));
	this->closeOpenIGTLinkConnection();
	mTrackingService->setState(Tool::tsNONE);
	this->stopCommand();
	this->wait(sWAIT_FOR_STOP, kStopTimeout_ms);
}

void TrackingSystemComparison::checkTrackingStopped()
{
	bool stopped = (mTrackingService->getState() == Tool::tsNONE);
	if (stopped || getMilliSecondsSinceEpoch() >= mStepDeadline_ms)
	{
		if (!stopped)
		{
			this->log(QString("Tracking did not stop within %1 s, continuing anyway").arg(kStopTimeout_ms / 1000));
			mSuccess = false;
		}
		this->wait(sPAUSE, mSettings.pauseBetweenRuns_ms);
	}
}

void TrackingSystemComparison::finish()
{
	mTimer.stop();
	mStep = sIDLE;
	QStringList comparison = compareRuns(mResults);
	if (!comparison.isEmpty())
	{
		mSummary << "" << comparison;
	}
	profile()->setToolConfigFilePath(mOriginalToolConfigFile);
	if (mVideoService && !mOriginalConnectionMethod.isEmpty())
	{
		mVideoService->setConnectionMethod(mOriginalConnectionMethod);
	}
	QFile file(mSettings.outputFolder + "/summary.txt");
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
	{
		file.write((this->getSummary() + "\n").toUtf8());
	}
	this->log(QString("Comparison %1. Summary in %2").arg(mSuccess ? "done" : "FAILED").arg(file.fileName()));
	emit finished(mSuccess);
}

bool TrackingSystemComparison::writeConfiguration(QString filename, QString implementation)
{
	QFile input(mSettings.toolConfigFile);
	QString config;
	if (input.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		config = setTrackingSystemImplementation(QString::fromUtf8(input.readAll()), implementation, mSettings.openIGTLinkApplyReference);
	}
	QFile output(filename);
	bool success = !config.isEmpty() && output.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
	if (success)
	{
		output.write(config.toUtf8());
	}
	return success;
}

void TrackingSystemComparison::startCommand(const TrackingComparisonRun& run)
{
	if (!run.command.isEmpty())
	{
		mProcess = new QProcess(this);
		mProcess->setProcessChannelMode(QProcess::MergedChannels);
		mProcess->setStandardOutputFile(mSettings.outputFolder + "/" + getRunLabel(mRunIndex, run.implementation) + "_command.log");
		mProcess->start(run.command, run.arguments);
		this->log(QString("Started %1 %2").arg(run.command).arg(run.arguments.join(" ")));
	}
}

void TrackingSystemComparison::stopCommand()
{
	if (mProcess)
	{
		mProcess->write("\n");
		mProcess->closeWriteChannel();
		if (!mProcess->waitForFinished(kCommandStopTimeout_ms))
		{
			mProcess->terminate();
			if (!mProcess->waitForFinished(kCommandStopTimeout_ms))
			{
				mProcess->kill();
				mProcess->waitForFinished(kCommandStopTimeout_ms);
			}
		}
		this->log(QString("Stopped %1, exit code %2").arg(mProcess->program()).arg(mProcess->exitCode()));
		mProcess->deleteLater();
		mProcess = nullptr;
	}
}

void TrackingSystemComparison::openOpenIGTLinkConnection()
{
	if (mVideoService)
	{
		this->setOpenIGTLinkOption(kOpenIGTLinkStartTrackingOption, "1");
		this->setOpenIGTLinkOption(kOpenIGTLinkHostOption, mSettings.openIGTLinkHost);
		this->setOpenIGTLinkOption(kOpenIGTLinkPortOption, QString::number(mSettings.openIGTLinkPort));
		mVideoService->setConnectionMethod(kOpenIGTLinkStreamer);
		mVideoService->openConnection();
		mOpenIGTLinkOpened = true;
	}
	else
	{
		this->log("No video service, cannot start OpenIGTLink tracking");
	}
}

void TrackingSystemComparison::closeOpenIGTLinkConnection()
{
	if (mOpenIGTLinkOpened && mVideoService)
	{
		mVideoService->closeConnection();
	}
	mOpenIGTLinkOpened = false;
	QDomElement options = profile()->getXmlSettings().descend("video").getElement("video");
	for (const std::pair<const QString, QString>& entry : mOriginalOpenIGTLinkOptions)
	{
		XmlOptionItem(entry.first, options).writeValue(entry.second);
	}
	mOriginalOpenIGTLinkOptions.clear();
}

void TrackingSystemComparison::setOpenIGTLinkOption(QString uid, QString value)
{
	XmlOptionItem item(uid, profile()->getXmlSettings().descend("video").getElement("video"));
	if (!mOriginalOpenIGTLinkOptions.count(uid))
	{
		mOriginalOpenIGTLinkOptions[uid] = item.readValue("");
	}
	item.writeValue(value);
}

void TrackingSystemComparison::wait(Step step, int duration_ms)
{
	mStep = step;
	mStepDeadline_ms = getMilliSecondsSinceEpoch() + duration_ms;
}

void TrackingSystemComparison::log(QString message)
{
	report("Tracking comparison: " + message);
	emit progress(message);
}

} // namespace cx
