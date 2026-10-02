/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxNdiTrackerManager.h"

#include <QTimer>

#include "cxNdiCapiDevice.h"
#include "cxLogger.h"
#include "cxTime.h"

namespace cx
{

NdiTrackerManager::NdiTrackerManager(ToolFileParser::TrackerInternalStructure trackerStructure,
									 std::vector<ToolFileParser::ToolInternalStructurePtr> toolStructures,
									 ToolFileParser::ToolInternalStructurePtr referenceToolStructure,
									 NdiDevicePtr device) :
	mDevice(device),
	mTimer(new QTimer(this)),
	mLastFrameNumber(0),
	mInFastPollMode(true),
	mConsecutiveReadFailures(0),
	mTrackerStructure(trackerStructure),
	mInitialized(false),
	mTracking(false)
{
	if (!mDevice)
	{
		mDevice.reset(new NdiCapiDevice());
	}

	for (ToolFileParser::ToolInternalStructurePtr& structure : toolStructures)
	{
		NdiToolPtr tool(new NdiTool(structure));
		mTools[tool->getUid()] = tool;
	}
	if (referenceToolStructure)
	{
		mReferenceTool.reset(new NdiTool(referenceToolStructure));
		mTools[mReferenceTool->getUid()] = mReferenceTool;
	}

	connect(mTimer, &QTimer::timeout, this, &NdiTrackerManager::poll);
}

NdiTrackerManager::~NdiTrackerManager()
{
	mTimer->stop();
	if (mInitialized)
	{
		this->disconnectFromDevice();
	}
}

std::map<QString, NdiToolPtr> NdiTrackerManager::getTools()
{
	QMutexLocker locker(&mToolMutex);
	return mTools;
}

NdiToolPtr NdiTrackerManager::getReferenceTool()
{
	QMutexLocker locker(&mToolMutex);
	return mReferenceTool;
}

bool NdiTrackerManager::isInitialized() const
{
	return mInitialized;
}

bool NdiTrackerManager::isTracking() const
{
	return mTracking;
}

NdiToolConfig NdiTrackerManager::createToolConfig(ToolFileParser::ToolInternalStructurePtr structure, TRACKING_SYSTEM trackerType)
{
	// Same rule as the IGSTK plugin: Polaris tools are wireless (passive, .rom file),
	// Aurora tools are wired and identified by port (and channel for 5DOF sensors).
	bool polaris = isPolaris(trackerType);
	NdiToolConfig retval;
	retval.uid = structure->mUid;
	retval.reference = structure->mIsReference;
	retval.wireless = polaris;
	retval.sromFile = structure->mSROMFilename;
	retval.port = static_cast<int>(structure->mPortNumber);
	retval.channel = static_cast<int>(structure->mChannelNumber);
	retval.fiveDOF = structure->m5DOF;
	return retval;
}

void NdiTrackerManager::initializeSlot(bool on)
{
	if (on && !mInitialized)
	{
		bool success = this->connectToDevice() && this->setupTools();
		if (success)
		{
			mInitialized = true;
			emit initialized(true);
		}
		else
		{
			this->disconnectFromDevice();
			emit error();
		}
	}
	else if (!on && mInitialized)
	{
		this->disconnectFromDevice();
		mInitialized = false;
		emit initialized(false);
	}
}

void NdiTrackerManager::trackSlot(bool on)
{
	if (!mInitialized)
	{
		reportWarning("NDI Tracker: Cannot start or stop tracking, not initialized.");
	}
	else if (on && !mTracking)
	{
		if (mDevice->startTracking())
		{
			mTracking = true;
			mLastFrameNumber = 0;
			mConsecutiveReadFailures = 0;
			mFrameRate.reset();
			this->startFastPoll(); // switches to waiting for the next frame once the frame rate is known
			emit tracking(true);
		}
		else
		{
			reportError("NDI Tracker: Failed to start tracking: " + mDevice->getLastError());
			emit error();
		}
	}
	else if (!on && mTracking)
	{
		mTimer->stop();
		mTracking = false;
		if (!mDevice->stopTracking())
		{
			reportWarning("NDI Tracker: Failed to stop tracking cleanly: " + mDevice->getLastError());
		}
		this->setAllToolsInvisible();
		emit tracking(false);
	}
}

void NdiTrackerManager::poll()
{
	if (!mTracking)
	{
		return;
	}
	NdiFrame frame;
	if (!mDevice->readFrame(mPortHandles, frame))
	{
		mConsecutiveReadFailures++;
		if (mConsecutiveReadFailures >= kMaxConsecutiveReadFailures)
		{
			reportError("NDI Tracker: Lost contact with the device: " + mDevice->getLastError());
			mTimer->stop();
			mTracking = false;
			this->setAllToolsInvisible();
			emit tracking(false);
			emit error();
		}
		return;
	}
	mConsecutiveReadFailures = 0;

	bool newFrame = (frame.frameNumber == 0) || (frame.frameNumber != mLastFrameNumber);
	if (newFrame)
	{
		double now = getMilliSecondsSinceEpoch();
		mLastFrameNumber = frame.frameNumber;
		mFrameRate.addFrame(now);
		this->scheduleNextPoll(now);
		this->distributeFrame(frame, now);
	}
	else if (!mInFastPollMode)
	{
		this->startFastPoll(); // waited for the next frame, but it isn't there yet
	}
}

void NdiTrackerManager::startFastPoll()
{
	mInFastPollMode = true;
	mTimer->setSingleShot(false);
	mTimer->start(NdiFrameRateEstimator::kFastPollInterval_ms);
}

void NdiTrackerManager::scheduleNextPoll(double now_ms)
{
	int wait = mFrameRate.getWaitBeforeNextPoll_ms(now_ms);
	if (wait > 0)
	{
		mInFastPollMode = false;
		mTimer->setSingleShot(true);
		mTimer->start(wait);
	}
	else
	{
		this->startFastPoll();
	}
}

void NdiTrackerManager::distributeFrame(const NdiFrame& frame, double timestamp)
{
	QMutexLocker locker(&mToolMutex);

	// Tool positions are reported relative to the reference tool, if there is one.
	// While the reference is not visible, none of the tools are.
	bool referenceVisible = !mReferenceTool;
	Transform3D cMref = Transform3D::Identity();
	QString referenceWarning;
	for (const NdiToolSample& sample : frame.samples)
	{
		std::map<int, NdiToolPtr>::iterator it = mPortHandleToTool.find(sample.portHandle);
		if (it != mPortHandleToTool.end() && it->second == mReferenceTool && sample.visible)
		{
			referenceVisible = true;
			cMref = sample.transform;
			referenceWarning = mReferenceTool->updateQuality(sample);
		}
	}

	for (const NdiToolSample& sample : frame.samples)
	{
		std::map<int, NdiToolPtr>::iterator it = mPortHandleToTool.find(sample.portHandle);
		if (it == mPortHandleToTool.end())
		{
			continue;
		}
		NdiToolPtr tool = it->second;
		bool visible = sample.visible && referenceVisible;
		Transform3D prMt = Transform3D::Identity();
		if (visible && tool != mReferenceTool)
		{
			prMt = cMref.inverse() * sample.transform;
		}
		if (visible && tool == mReferenceTool)
		{
			tool->setPositionWarning(referenceWarning);
		}
		else if (visible)
		{
			tool->setPositionWarning(combinePositionWarnings(tool->updateQuality(sample), referenceWarning));
		}
		tool->updateTransform(prMt, timestamp, visible, sample);
	}
}

QString NdiTrackerManager::combinePositionWarnings(QString toolWarning, QString referenceWarning)
{
	QString retval = toolWarning;
	if (!referenceWarning.isEmpty())
	{
		retval = toolWarning.isEmpty() ? "Reference: " + referenceWarning : toolWarning + "; reference: " + referenceWarning;
	}
	return retval;
}

double NdiTrackerManager::getDefaultMaxErrorRMS(TRACKING_SYSTEM trackerType)
{
	bool aurora = (trackerType == tsAURORA);
	return aurora ? NdiPositionQuality::kDefaultMaxErrorRMSAurora_mm : NdiPositionQuality::kDefaultMaxErrorRMSPolaris_mm;
}

bool NdiTrackerManager::isPolaris(TRACKING_SYSTEM trackerType)
{
	return (trackerType == tsPOLARIS || trackerType == tsPOLARIS_SPECTRA || trackerType == tsPOLARIS_VICRA);
}

void NdiTrackerManager::setMaxErrorRMS(double maxErrorRMS_mm)
{
	QMutexLocker locker(&mToolMutex);
	for (std::pair<const QString, NdiToolPtr>& entry : mTools)
	{
		entry.second->setMaxErrorRMS(maxErrorRMS_mm);
	}
}

void NdiTrackerManager::setAllToolsInvisible()
{
	QMutexLocker locker(&mToolMutex);
	for (std::pair<const QString, NdiToolPtr>& entry : mTools)
	{
		entry.second->setInvisible();
	}
}

void NdiTrackerManager::setConnectionSettings(const NdiConnectionSettings& connection)
{
	QMutexLocker locker(&mConnectionMutex);
	mConnection = connection;
}

bool NdiTrackerManager::connectToDevice()
{
	NdiConnectionSettings connection;
	{
		QMutexLocker locker(&mConnectionMutex);
		connection = mConnection;
	}
	connection.requestMarkerInfo = isPolaris(mTrackerStructure.mType);
	this->setMaxErrorRMS(connection.maxErrorRMS_mm > 0.0 ? connection.maxErrorRMS_mm : getDefaultMaxErrorRMS(mTrackerStructure.mType));
	QString where = connection.hostname.isEmpty() ? QString("(searching serial ports)") : connection.hostname;
	report("NDI Tracker: Connecting to device " + where + "...");
	bool success = mDevice->open(connection);
	if (success)
	{
		reportSuccess(QString("NDI Tracker: Connected to %1 %2").arg(where).arg(mDevice->getDescription()));
	}
	else
	{
		reportError("NDI Tracker: Could not connect to the device: " + mDevice->getLastError()
					+ ". Check the cable, or set 'ndiTrackingHostname' in the CustusX settings.");
	}
	return success;
}

bool NdiTrackerManager::setupTools()
{
	std::vector<NdiToolConfig> configs;
	{
		QMutexLocker locker(&mToolMutex);
		for (std::pair<const QString, NdiToolPtr>& entry : mTools)
		{
			configs.push_back(createToolConfig(entry.second->getInternalStructure(), mTrackerStructure.mType));
		}
	}

	NdiToolSetupResult result = setupNdiTools(*mDevice, configs);
	for (const QString& warning : result.warnings)
	{
		reportWarning("NDI Tracker: " + warning);
	}
	for (const QString& message : result.messages)
	{
		report("NDI Tracker: " + message);
	}

	QMutexLocker locker(&mToolMutex);
	mPortHandleToTool.clear();
	mPortHandles.clear();
	for (const std::pair<const int, QString>& entry : result.portHandleToUid)
	{
		NdiToolPtr tool = mTools[entry.second];
		tool->setPortHandle(entry.first);
		tool->setInitialized(true);
		mPortHandleToTool[entry.first] = tool;
		mPortHandles.push_back(entry.first);
	}

	if (result.success())
	{
		reportSuccess(QString("NDI Tracker: %1 of %2 tool(s) ready.").arg(result.portHandleToUid.size()).arg(configs.size()));
	}
	else
	{
		reportError("NDI Tracker: None of the configured tools could be set up.");
	}
	return result.success();
}

void NdiTrackerManager::disconnectFromDevice()
{
	mTimer->stop();
	if (mTracking)
	{
		mDevice->stopTracking();
		mTracking = false;
	}
	for (int portHandle : mPortHandles)
	{
		mDevice->freePortHandle(portHandle);
	}
	mDevice->close();

	QMutexLocker locker(&mToolMutex);
	mPortHandleToTool.clear();
	mPortHandles.clear();
	for (std::pair<const QString, NdiToolPtr>& entry : mTools)
	{
		entry.second->setPortHandle(-1);
		entry.second->setInitialized(false);
		entry.second->setInvisible();
	}
}

} // namespace cx
