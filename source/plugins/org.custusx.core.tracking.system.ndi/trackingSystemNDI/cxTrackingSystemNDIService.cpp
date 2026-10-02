/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTrackingSystemNDIService.h"

#include <QDir>
#include <QFile>
#include <QCoreApplication>

#include "cxLogger.h"
#include "cxToolUsingNDI.h"
#include "cxNdiTrackerThread.h"
#include "cxToolConfigurationParser.h"
#include "cxSettings.h"
#include "cxProfile.h"
#include "cxDefinitions.h"
#include "cxTrackerConfigurationImpl.h"

namespace cx
{

TrackingSystemNDIService::TrackingSystemNDIService()
{
	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(globalConfigurationFileChangedSlot(QString)));
	this->setConfigurationFile(profile()->getToolConfigFilePath());
}

TrackingSystemNDIService::~TrackingSystemNDIService()
{
	this->destroyTrackerThread();
}

std::vector<ToolPtr> TrackingSystemNDIService::getTools()
{
	return mTools;
}

void TrackingSystemNDIService::setState(const Tool::State val)
{
	this->internalSetState(val);
}

void TrackingSystemNDIService::configure()
{
	if (mConfigurationFilePath.isEmpty() || !QFile::exists(mConfigurationFilePath))
	{
		reportWarning(QString("NDI Tracking: Configuration file [%1] is not valid.")
						  .arg(mConfigurationFilePath));
		return;
	}

	ConfigurationFileParser configParser(mConfigurationFilePath, mLoggingFolder);

	QString implementation = configParser.getTrackingSystemImplementation();
	bool isNdi = implementation.contains(TRACKING_SYSTEM_IMPLEMENTATION_NDI, Qt::CaseInsensitive);
	bool isLegacyIgstk = implementation.contains(TRACKING_SYSTEM_IMPLEMENTATION_IGSTK, Qt::CaseInsensitive);
	if (!isNdi && !isLegacyIgstk)
	{
		CX_LOG_DEBUG() << "TrackingSystemNDIService::configure(): Not using NDI tracking.";
		return;
	}
	if (isLegacyIgstk)
		CX_LOG_INFO() << "TrackingSystemNDIService::configure(): Legacy IGSTK config detected, using NDI instead.";
	emit updateTrackingSystemImplementation(TRACKING_SYSTEM_IMPLEMENTATION_NDI);

	std::vector<ToolFileParser::TrackerInternalStructure> trackers = configParser.getTrackers();
	if (trackers.empty())
	{
		reportWarning("NDI Tracking: Failed to configure - no trackers found in configuration.");
		return;
	}

	ToolFileParser::TrackerInternalStructure trackerStructure = trackers[0]; // one tracker at a time

	ToolFileParser::ToolInternalStructurePtr referenceToolStructure;
	std::vector<ToolFileParser::ToolInternalStructurePtr> toolStructures;

	QString referenceToolFile = configParser.getAbsoluteReferenceFilePath();
	std::vector<QString> toolfiles = configParser.getAbsoluteToolFilePaths();

	for (const QString& toolFile : toolfiles)
	{
		ToolFileParser toolParser(toolFile, mLoggingFolder);
		ToolFileParser::ToolInternalStructurePtr internalTool = toolParser.getTool();
		if (toolFile == referenceToolFile)
			referenceToolStructure = internalTool;
		else
			toolStructures.push_back(internalTool);
	}

	mTrackerThread.reset(new NdiTrackerThread(trackerStructure, toolStructures, referenceToolStructure));

	connect(mTrackerThread.get(), &NdiTrackerThread::configured, this, &TrackingSystemNDIService::trackerConfiguredSlot);
	connect(mTrackerThread.get(), &NdiTrackerThread::initialized, this, &TrackingSystemNDIService::initializedSlot);
	connect(mTrackerThread.get(), &NdiTrackerThread::tracking, this, &TrackingSystemNDIService::trackerTrackingSlot);
	connect(mTrackerThread.get(), &NdiTrackerThread::error, this, &TrackingSystemNDIService::trackerErrorSlot);

	mTrackerThread->start();
}

void TrackingSystemNDIService::trackerConfiguredSlot(bool on)
{
	if (!on)
	{
		this->deconfigure();
		return;
	}

	if (!mTrackerThread)
	{
		reportDebug("NDI Tracking: Received configured signal but mTrackerThread is null.");
		return;
	}

	mTools.clear();
	std::map<QString, NdiToolPtr> ndiTools = mTrackerThread->getTools();
	for (std::pair<const QString, NdiToolPtr>& kv : ndiTools)
	{
		NdiToolPtr ndiTool = kv.second;
		ToolUsingNDIPtr tool(new ToolUsingNDI(ndiTool));
		if (tool->isValid())
		{
			mTools.push_back(tool);
			if (ndiTool->isReference())
				mReference = tool;
		}
		else
		{
			reportWarning("NDI Tracking: Creation of tool '" + kv.first + "' failed.");
		}
	}
	if (mExternalTool)
		mTools.push_back(mExternalTool);

	mState = Tool::tsCONFIGURED;
	reportSuccess("NDI Tracking Service configured.");
	emit configured();
	emit stateChanged();
}

void TrackingSystemNDIService::deconfigure()
{
	if (!this->isConfigured())
		return;

	// Destroy the tracker thread synchronously regardless of tracking state.
	// This works even when the Qt event loop is not running (e.g. at application
	// shutdown). The thread's own cleanup deletes the NdiTrackerManager, which
	// stops tracking, frees the port handles and closes the device, so no explicit
	// stopTracking/uninitialize sequence is needed.
	this->destroyTrackerThread();
	mTools.clear();
	mReference.reset();

	mState = Tool::tsNONE;
	emit deconfigured();
	emit stateChanged();
	report("NDI Tracking Service deconfigured.");
}

void TrackingSystemNDIService::initialize()
{
	if (!this->isConfigured())
	{
		connect(this, SIGNAL(configured()), this, SLOT(initializeAfterConfigSlot()), Qt::UniqueConnection);
		this->configure();
		return;
	}

	if (mTrackerThread)
	{
		mTrackerThread->setConnectionSettings(this->readConnectionSettings());
		mTrackerThread->initialize(true);
	}
	else
		reportError("NDI Tracking: Cannot initialize - tracking thread does not exist.");
}

void TrackingSystemNDIService::uninitialize()
{
	if (this->isTracking())
	{
		connect(this, SIGNAL(trackingStopped()), this, SLOT(uninitializeAfterTrackingStoppedSlot()), Qt::UniqueConnection);
		this->stopTracking();
		return;
	}

	if (!this->isInitialized())
		return;

	if (mTrackerThread)
		mTrackerThread->initialize(false);
}

void TrackingSystemNDIService::startTracking()
{
	if (!this->isInitialized())
	{
		connect(this, SIGNAL(initialized()), this, SLOT(startTrackingAfterInitSlot()), Qt::UniqueConnection);
		this->initialize();
		return;
	}

	if (mTrackerThread)
		mTrackerThread->track(true);
}

void TrackingSystemNDIService::stopTracking()
{
	if (!this->isTracking())
		return;

	if (mTrackerThread)
		mTrackerThread->track(false);
}

void TrackingSystemNDIService::initializedSlot(bool value)
{
	if (value)
	{
		mState = Tool::tsINITIALIZED;
		reportSuccess("NDI Tracking Service initialized.");
		emit stateChanged();
		emit initialized();
	}
	else
	{
		mState = Tool::tsCONFIGURED;
		report("NDI Tracking Service uninitialized.");
		emit stateChanged();
		emit uninitialized();
	}
}

void TrackingSystemNDIService::trackerTrackingSlot(bool value)
{
	if (value)
	{
		mState = Tool::tsTRACKING;
		reportSuccess("NDI Tracking Service started tracking.");
		emit stateChanged();
		emit trackingStarted();
	}
	else
	{
		mState = Tool::tsINITIALIZED;
		reportSuccess("NDI Tracking Service stopped tracking.");
		emit stateChanged();
		emit trackingStopped();
	}
}

void TrackingSystemNDIService::setConfigurationFile(QString configurationFile)
{
	if (configurationFile == mConfigurationFilePath)
		return;

	if (this->isConfigured())
	{
		connect(this, SIGNAL(deconfigured()), this, SLOT(configureAfterDeconfigureSlot()), Qt::UniqueConnection);
		this->deconfigure();
	}

	mConfigurationFilePath = configurationFile;
}

void TrackingSystemNDIService::setLoggingFolder(QString loggingFolder)
{
	if (mLoggingFolder == loggingFolder)
		return;

	if (this->isConfigured())
	{
		connect(this, SIGNAL(deconfigured()), this, SLOT(configureAfterDeconfigureSlot()), Qt::UniqueConnection);
		this->deconfigure();
	}
	mLoggingFolder = loggingFolder;
}

TrackerConfigurationPtr TrackingSystemNDIService::getConfiguration()
{
	TrackerConfigurationPtr retval;
	retval.reset(new TrackerConfigurationImpl());
	retval->setTrackingSystemImplementation(TRACKING_SYSTEM_IMPLEMENTATION_NDI);
	return retval;
}

NdiConnectionSettings TrackingSystemNDIService::readConnectionSettings() const
{
	// Hidden settings: set in the settings file if needed.
	NdiConnectionSettings retval;
	retval.hostname = settings()->value("ndiTrackingHostname", "").toString();
	retval.baudRate = settings()->value("ndiTrackingBaudRate", 115200).toInt();
	retval.maxErrorRMS_mm = settings()->value("ndiTrackingMaxErrorRMS", 0.0).toDouble();
	if (settings()->value("ndiTrackingDebugLogging", false).toBool() && !mLoggingFolder.isEmpty())
	{
		QDir().mkpath(mLoggingFolder);
		retval.communicationLogFile = mLoggingFolder + "/NdiCommunication.txt";
	}
	return retval;
}

void TrackingSystemNDIService::trackerErrorSlot()
{
	// Cancel any pending "start tracking/initialize when ready", so that a later
	// successful attempt doesn't start something the user didn't ask for.
	disconnect(this, SIGNAL(initialized()), this, SLOT(startTrackingAfterInitSlot()));
	disconnect(this, SIGNAL(configured()), this, SLOT(initializeAfterConfigSlot()));
	disconnect(this, SIGNAL(trackingStopped()), this, SLOT(uninitializeAfterTrackingStoppedSlot()));
	this->uninitialize();
}

void TrackingSystemNDIService::startTrackingAfterInitSlot()
{
	disconnect(this, SIGNAL(initialized()), this, SLOT(startTrackingAfterInitSlot()));
	this->startTracking();
}

void TrackingSystemNDIService::initializeAfterConfigSlot()
{
	disconnect(this, SIGNAL(configured()), this, SLOT(initializeAfterConfigSlot()));
	this->initialize();
}

void TrackingSystemNDIService::uninitializeAfterTrackingStoppedSlot()
{
	disconnect(this, SIGNAL(trackingStopped()), this, SLOT(uninitializeAfterTrackingStoppedSlot()));
	this->uninitialize();
}

void TrackingSystemNDIService::configureAfterDeconfigureSlot()
{
	disconnect(this, SIGNAL(deconfigured()), this, SLOT(configureAfterDeconfigureSlot()));
	this->configure();
}

void TrackingSystemNDIService::globalConfigurationFileChangedSlot(QString key)
{
	if (key == "toolConfigFile")
		this->setConfigurationFile(profile()->getToolConfigFilePath());
}

void TrackingSystemNDIService::destroyTrackerThread()
{
	if (mTrackerThread)
	{
		mTrackerThread->quit();
		mTrackerThread->wait(2000);
		if (mTrackerThread->isRunning())
		{
			mTrackerThread->terminate();
			mTrackerThread->wait();
		}
		QObject::disconnect(mTrackerThread.get());
		mTrackerThread.reset();
	}
}

} // namespace cx
