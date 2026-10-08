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
#include <QFileInfo>
#include <QProcess>
#include <QTextStream>
#include "cxLogger.h"
#include "cxProfile.h"
#include "cxTime.h"
#include "cxTrackingService.h"
#include "cxXmlOptionItem.h"
#include "cxDefinitions.h"

namespace cx
{

namespace
{
const int kTimerInterval_ms = 100;
const int kCommandTerminateAfter_ms = 3000;
const int kCommandKillAfter_ms = 6000;
const int kStopTimeout_ms = 15000;
const char* kOpenIGTLinkHostOption = "ip_scanner_openigtlink";
const char* kOpenIGTLinkPortOption = "ip_port";
}

TrackingSystemComparison::TrackingSystemComparison(TrackingServicePtr trackingService) :
	mTrackingService(trackingService),
	mRecorder(trackingService),
	mProcess(nullptr),
	mStep(sIDLE),
	mStepDeadline_ms(0.0),
	mStepStart_ms(0.0),
	mRunIndex(0),
	mSuccess(false)
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
		bool runActive = (mStep != sWAIT_FOR_STOP && mStep != sPAUSE);
		bool runsLeft = (mRunIndex + 1 < static_cast<int>(mSettings.runs.size()));
		mRecorder.stop();
		if (runActive)
		{
			this->stopRun("cancelled");
		}
		mSuccess = mSuccess && !runActive && !runsLeft;
		this->killCommand();
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
		std::map<QString, double> differences = getMeanDifferences(results[0].statistics, results[i].statistics);
		for (const std::pair<const QString, ToolPositionStatistics>& entry : results[0].statistics)
		{
			bool comparable = !entry.second.reference && entry.second.positions > 0;
			if (comparable && differences.count(entry.first))
			{
				retval << QString("%1 %2: mean position %3 mm from %4").arg(results[i].label).arg(entry.first)
						  .arg(differences[entry.first], 0, 'f', 3).arg(results[0].label);
			}
			else if (comparable)
			{
				retval << QString("%1 %2: no positions, not compared with %3").arg(results[i].label).arg(entry.first).arg(results[0].label);
			}
		}
	}
	return retval;
}

QStringList TrackingSystemComparison::getResultTableHeader()
{
	return {"System", "Tool", "Rate (Hz)", "Positions", "With warning", "Jitter (mm)", "Drift (mm)", "From first (mm)"};
}

std::vector<QStringList> TrackingSystemComparison::createResultTable(const std::vector<TrackingComparisonResult>& results)
{
	std::vector<QStringList> retval;
	for (unsigned i = 0; i < results.size(); ++i)
	{
		std::map<QString, double> differences = getMeanDifferences(results[0].statistics, results[i].statistics);
		for (const std::pair<const QString, ToolPositionStatistics>& entry : results[i].statistics)
		{
			const ToolPositionStatistics& statistics = entry.second;
			if (!statistics.reference)
			{
				QString difference = (i > 0 && differences.count(entry.first)) ? QString::number(differences[entry.first], 'f', 3) : "-";
				retval.push_back({results[i].label, entry.first, QString::number(statistics.getRate_Hz(), 'f', 1),
								  QString::number(statistics.positions), QString::number(statistics.positionsWithWarning),
								  QString::number(statistics.getStandardDeviation().norm(), 'f', 3),
								  QString::number(statistics.getDrift_mm(), 'f', 3), difference});
			}
		}
	}
	return retval;
}

std::vector<TrackingComparisonResult> TrackingSystemComparison::getResults() const
{
	return mResults;
}

int TrackingSystemComparison::getComparedToolCount() const
{
	int retval = 0;
	for (unsigned i = 1; i < mResults.size(); ++i)
	{
		retval += static_cast<int>(getMeanDifferences(mResults[0].statistics, mResults[i].statistics).size());
	}
	return retval;
}

bool TrackingSystemComparison::hasTrackedTool(const ToolPositionStatisticsMap& statistics)
{
	bool retval = false;
	for (const std::pair<const QString, ToolPositionStatistics>& entry : statistics)
	{
		retval = retval || (!entry.second.reference && entry.second.positions > 0);
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

QString TrackingSystemComparison::expandHomeFolder(QString path)
{
	QString retval = path;
	if (path == "~" || path.startsWith("~/"))
	{
		retval = QDir::homePath() + path.mid(1);
	}
	return retval;
}

QString TrackingSystemComparison::getWorkingFolder(QString program)
{
	QString retval;
	if (program.contains("/"))
	{
		retval = QFileInfo(program).absolutePath();
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
		this->stopRun(hasTrackedTool(mRecorder.getStatistics()) ? "OK" : "FAILED: no positions recorded from any tool other than the reference");
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
	mRecorder.clear();
	this->log(QString("Run %1: %2").arg(label).arg(run.command.isEmpty() ? run.implementation : run.implementation + " via " + run.command));
	QString configFile = profile()->getApplicationToolConfigPaths().front() + label + "_tracking_comparison.xml";
	mWrittenToolConfigFiles << configFile;
	if (!this->writeConfiguration(configFile, run.implementation))
	{
		this->stopRun("FAILED: could not write the tool configuration " + configFile);
		return;
	}
	QFile::copy(configFile, mSettings.outputFolder + "/" + label + "_toolconfig.xml");
	profile()->setToolConfigFilePath(configFile);
	this->startCommand(run);
	this->wait(sWAIT_FOR_COMMAND, run.command.isEmpty() ? 0 : mSettings.commandStartupDelay_ms);
}

void TrackingSystemComparison::startTracking()
{
	if (this->currentRun().implementation.contains(TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK, Qt::CaseInsensitive))
	{
		this->setOpenIGTLinkServer();
	}
	mTrackingService->setState(Tool::tsTRACKING);
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
	mTrackingService->setState(Tool::tsNONE);
	this->restoreOpenIGTLinkServer();
	this->stopCommand();
	this->wait(sWAIT_FOR_STOP, kStopTimeout_ms);
}

void TrackingSystemComparison::checkTrackingStopped()
{
	double now = getMilliSecondsSinceEpoch();
	bool commandStopped = this->checkCommandStopped(now - mStepStart_ms);
	bool stopped = commandStopped && (mTrackingService->getState() == Tool::tsNONE);
	if (stopped || now >= mStepDeadline_ms)
	{
		if (!stopped)
		{
			this->log(QString("Tracking did not stop within %1 s, continuing anyway").arg(kStopTimeout_ms / 1000));
			mSuccess = false;
			this->killCommand();
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
	for (const QString& configFile : mWrittenToolConfigFiles)
	{
		QFile::remove(configFile);
	}
	mWrittenToolConfigFiles.clear();
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
		QProcess* process = mProcess;
		connect(process, &QProcess::errorOccurred, this, [this, process](QProcess::ProcessError error)
		{
			this->log(QString("Error %1 from %2: %3").arg(error).arg(process->program()).arg(process->errorString()));
		});
		QString program = expandHomeFolder(run.command);
		mProcess->setWorkingDirectory(getWorkingFolder(program));
		mProcess->start(program, run.arguments);
		this->log(QString("Started %1 %2").arg(run.command).arg(run.arguments.join(" ")));
	}
}

void TrackingSystemComparison::stopCommand()
{
	if (mProcess)
	{
		mProcess->write("\n");
		mProcess->closeWriteChannel();
	}
}

bool TrackingSystemComparison::checkCommandStopped(double elapsed_ms)
{
	if (mProcess && mProcess->state() != QProcess::NotRunning)
	{
		if (elapsed_ms >= kCommandKillAfter_ms)
		{
			mProcess->kill();
		}
		else if (elapsed_ms >= kCommandTerminateAfter_ms)
		{
			mProcess->terminate();
		}
	}
	else if (mProcess)
	{
		this->log(QString("Stopped %1, exit code %2").arg(mProcess->program()).arg(mProcess->exitCode()));
		mProcess->deleteLater();
		mProcess = nullptr;
	}
	return !mProcess;
}

void TrackingSystemComparison::killCommand()
{
	if (mProcess)
	{
		mProcess->kill();
		mProcess->waitForFinished(1000);
		this->log(QString("Killed %1").arg(mProcess->program()));
		mProcess->deleteLater();
		mProcess = nullptr;
	}
}

void TrackingSystemComparison::setOpenIGTLinkServer()
{
	this->setOpenIGTLinkOption(kOpenIGTLinkHostOption, mSettings.openIGTLinkHost);
	this->setOpenIGTLinkOption(kOpenIGTLinkPortOption, QString::number(mSettings.openIGTLinkPort));
}

void TrackingSystemComparison::restoreOpenIGTLinkServer()
{
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
	mStepStart_ms = getMilliSecondsSinceEpoch();
	mStepDeadline_ms = mStepStart_ms + duration_ms;
}

void TrackingSystemComparison::log(QString message)
{
	report("Tracking comparison: " + message);
	emit progress(message);
}

} // namespace cx
