/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#define _USE_MATH_DEFINES

#include "cxTrackingSystemIGSTKService.h"

#include <QDir>
#include <QList>
#include <QMetaType>
#include <QFileInfo>
#include <vtkDoubleArray.h>
#include <QCoreApplication>

#include "cxRegistrationTransform.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxPositionStorageFile.h"
#include "cxTime.h"
#include "cxEnumConverter.h"
#include "cxDummyTool.h"
#include "cxToolUsingIGSTK.h"
#include "cxIgstkTracker.h"
#include "cxToolConfigurationParser.h"
#include "cxManualToolAdapter.h"
#include "cxSettings.h"
#include "cxDataLocations.h"
#include "cxIgstkTrackerThread.h"
#include "cxPlaybackTool.h"

#include "cxPlaybackTime.h"
#include "cxTrackingPositionFilter.h"
#include "cxXMLNodeWrapper.h"
#include "cxTrackerConfigurationImpl.h"
#include "cxProfile.h"

namespace cx
{

//QStringList TrackingSystemIGSTKService::getSupportedTrackingSystems()
//{
//	QStringList retval;
//	retval = IgstkTracker::getSupportedTrackingSystems();
//	return retval;
//}

TrackingSystemIGSTKService::TrackingSystemIGSTKService() :
				mConfigurationFilePath(""),
				mLoggingFolder(""),
				mState(Tool::tsNONE)
{
	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(globalConfigurationFileChangedSlot(QString)));
	// initialize config file
	this->setConfigurationFile(profile()->getToolConfigFilePath());
}

TrackingSystemIGSTKService::~TrackingSystemIGSTKService()
{
	this->destroyTrackerThread();
}

std::vector<ToolPtr> TrackingSystemIGSTKService::getTools()
{
	return mTools;
}

Tool::State TrackingSystemIGSTKService::getState() const
{
	return mState;
}

void TrackingSystemIGSTKService::setState(const Tool::State val)
{
	if (mState==val)
		return;

	if (val > mState) // up
	{
		if (val == Tool::tsTRACKING)
			this->startTracking();
		else if (val == Tool::tsINITIALIZED)
			this->initialize();
		else if (val == Tool::tsCONFIGURED)
			this->configure();
	}
	else // down
	{
		if (val == Tool::tsINITIALIZED)
			this->stopTracking();
		else if (val == Tool::tsCONFIGURED)
			this->uninitialize();
		else if (val == Tool::tsNONE)
		{
			this->deconfigure();
		}
	}
}

bool TrackingSystemIGSTKService::isConfigured() const
{
	return mState>=Tool::tsCONFIGURED;
}

bool TrackingSystemIGSTKService::isInitialized() const
{
	return mState>=Tool::tsINITIALIZED;
}

bool TrackingSystemIGSTKService::isTracking() const
{
	return mState>=Tool::tsTRACKING;
}

void TrackingSystemIGSTKService::configure()
{
	if (mConfigurationFilePath.isEmpty() || !QFile::exists(mConfigurationFilePath))
	{
		reportWarning(QString("Configuration file [%1] is not valid, could not configure the toolmanager.").arg(mConfigurationFilePath));
		return;
	}

	//parse
	ConfigurationFileParser configParser(mConfigurationFilePath, mLoggingFolder);

    std::vector<ToolFileParser::TrackerInternalStructure> trackers = configParser.getTrackers();

	if (trackers.empty())
	{
		reportWarning("Failed to configure tracking.");
		return;
	}

    ToolFileParser::TrackerInternalStructure trackerStructure = trackers[0]; //we only support one tracker atm

    ToolFileParser::ToolInternalStructure referenceToolStructure;
    std::vector<ToolFileParser::ToolInternalStructure> toolStructures;
	QString referenceToolFile = configParser.getAbsoluteReferenceFilePath();
	std::vector<QString> toolfiles = configParser.getAbsoluteToolFilePaths();
	for (std::vector<QString>::iterator it = toolfiles.begin(); it != toolfiles.end(); ++it)
	{
		ToolFileParser toolParser(*it, mLoggingFolder);
        ToolFileParser::ToolInternalStructure internalTool = toolParser.getTool();
		if ((*it) == referenceToolFile)
			referenceToolStructure = internalTool;
		else
			toolStructures.push_back(internalTool);
	}

	//new thread
	mTrackerThread.reset(new IgstkTrackerThread(trackerStructure, toolStructures, referenceToolStructure));

	connect(mTrackerThread.get(), SIGNAL(configured(bool)), this, SLOT(trackerConfiguredSlot(bool)));
	connect(mTrackerThread.get(), SIGNAL(initialized(bool)), this, SLOT(initializedSlot(bool)));
	connect(mTrackerThread.get(), SIGNAL(tracking(bool)), this, SLOT(trackerTrackingSlot(bool)));
	connect(mTrackerThread.get(), SIGNAL(error()), this, SLOT(uninitialize()));

	//start threads
	if (mTrackerThread)
		mTrackerThread->start();
}

void TrackingSystemIGSTKService::trackerConfiguredSlot(bool on)
{
	if (!on)
	{
		this->deconfigure();
		return;
	}

	if (!mTrackerThread)
	{
		reportDebug("Received a configured signal in ToolManager, but we don't have a mTrackerThread, this should never happen, contact programmer.");
		return;
	}

	//new all tools
	mTools.clear();
	std::map<QString, IgstkToolPtr> igstkTools = mTrackerThread->getTools();
	IgstkToolPtr reference = mTrackerThread->getRefereceTool();
	std::map<QString, IgstkToolPtr>::iterator it = igstkTools.begin();
	for (; it != igstkTools.end(); ++it)
	{
		IgstkToolPtr igstkTool = it->second;
        ToolUsingIGSTKPtr tool(new ToolUsingIGSTK(igstkTool));
		if (tool->isValid())
		{
			mTools.push_back(tool);
			if (tool->getProbe())
				emit newProbe(tool);
		}
		else
			reportWarning("Creation of the cxTool " + it->second->getUid() + " failed.");
	}

	mState = Tool::tsCONFIGURED;

	reportSuccess("IGSTK Tracking Service Configured.");
	emit configured();
	emit stateChanged();
}

//ToolPtr TrackingSystemIGSTKService::getReferenceTool()
//{
//	return mReference;
//}

void TrackingSystemIGSTKService::deconfigure()
{
	if (!this->isConfigured())
		return;

	if (this->isInitialized())
	{
		connect(this, SIGNAL(uninitialized()), this, SLOT(deconfigureAfterUninitializedSlot()));
		this->uninitialize();
		return;
	}
	mTools.clear();

	this->destroyTrackerThread();

//	this->setActiveTool(this->getManualTool()->getUid());

	mState = Tool::tsNONE;
	emit deconfigured();
	emit stateChanged();
	report("IGSTK Tracking Service is deconfigured.");
}

void TrackingSystemIGSTKService::initialize()
{
	if (!this->isConfigured())
	{
		connect(this, SIGNAL(configured()), this, SLOT(initializeAfterConfigSlot()));
		this->configure();
		return;
	}

	if (!this->isConfigured())
	{
		reportWarning("Please configure before trying to initialize.");
		return;
	}

#ifndef WIN32
	if (!this->createSymlink())
	{
		reportError("Initialization of tracking failed.");
		return;
	}
#endif

	if (mTrackerThread)
		mTrackerThread->initialize(true);
	else
		reportError("Cannot initialize the tracking system because the tracking thread does not exist.");
}

void TrackingSystemIGSTKService::uninitialize()
{
	if (this->isTracking())
	{
		connect(this, SIGNAL(trackingStopped()), this, SLOT(uninitializeAfterTrackingStoppedSlot()));
		this->stopTracking();
		return;
	}

	if (!this->isInitialized())
	{
		return;
	}
	if (mTrackerThread)
		mTrackerThread->initialize(false);
}

#ifndef WIN32
/** Assume that IGSTK requires the file /Library/CustusX/igstk.links/cu.CustusX.dev0
 *  as a rep for the HW connection. Also assume that directory is created with full
 *  read/write access (by installer or similar).
 *  Create that file as a symlink to the correct device.
 */
bool TrackingSystemIGSTKService::createSymlink()
{
	bool retval = true;
	QFileInfo symlink = this->getSymlink();
	QDir linkDir(symlink.absolutePath());
	QString linkfile = symlink.absoluteFilePath();
	;

	if (!linkDir.exists())
	{
		reportError(
                        QString("Folder %1 does not exist. It is required to exist and be writable in order to connecto to IGSTK.").arg(linkDir.path()));
		return false;
	}

	QDir devDir("/dev/");

	QStringList filters;
	// cu* applies to Mac, ttyUSB applies to Linux
	filters << "cu.usbserial*" << "cu.KeySerial*" << "serial" << "ttyUSB*"; //NOTE: only works with current hardware using aurora or polaris.
//  filters << "cu.usbserial*" << "cu.KeySerial*" << "serial" << "serial/by-id/usb-NDI*" ; //NOTE: only works with current hardware using aurora or polaris.
	QStringList files = devDir.entryList(filters, QDir::System);

	if (files.empty())
	{
		reportError(
						QString("No usb connections found in /dev using filters %1").arg(filters.join(";")));
		return false;
	}
	else
	{
		report(QString("Device files: %1").arg(files.join(",")));
		if (files.size() > 1)
			reportError(
					QString("More than one tracker connected? Will only try to connect to: %1").arg(files[0]));
	}

	QString device = devDir.filePath(files[0]);
//  QString device = "/dev/serial/by-id/usb-NDI_NDI_Host_USB_Converter-if00-port0";

	QFile(linkfile).remove();
	QFile devFile(device);
	QFileInfo devFileInfo(device);
	if (!devFileInfo.isWritable())
	{
		reportError(QString("Device %1 is not writable. Connection will fail.").arg(device));
		retval = false;
	}
	// this call only succeeds if Custus is run as root.
	bool val = devFile.link(linkfile);
	if (!val)
	{
		reportError(
						QString("Symlink %1 creation to device %2 failed with code %3").arg(linkfile).arg(device).arg(
										devFile.error()));
		retval = false;
	}
	else
	{
		report(QString("Created symlink %1 to device %2").arg(linkfile).arg(device));
	}

	devFile.setPermissions(
					QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadGroup | QFile::WriteGroup
									| QFile::ExeGroup | QFile::ReadOther | QFile::WriteOther | QFile::ExeOther);
	return retval;
}

QFileInfo TrackingSystemIGSTKService::getSymlink() const
{
	QString name("/Library/CustusX/igstk.links");
	QDir linkDir(name);
	QDir::root().mkdir(name); // only works if run with sudo
	QString linkFile = linkDir.path() + "/cu.CustusX.dev0";
	return QFileInfo(linkDir, linkFile);
}

/** removes symlinks to tracking system created during setup
 */
void TrackingSystemIGSTKService::cleanupSymlink()
{
	report("Cleaning up symlinks.");
	QFile(this->getSymlink().absoluteFilePath()).remove();
}
#endif //WIN32

void TrackingSystemIGSTKService::startTracking()
{
	if (!this->isInitialized())
	{
		connect(this, SIGNAL(initialized()), this, SLOT(startTrackingAfterInitSlot()));
		this->initialize();
		return;
	}

	if (mTrackerThread)
		mTrackerThread->track(true);
}

void TrackingSystemIGSTKService::stopTracking()
{
	if (!this->isTracking())
	{
		return;
	}
	if (mTrackerThread)
		mTrackerThread->track(false);
}

void TrackingSystemIGSTKService::setConfigurationFile(QString configurationFile)
{
	if (configurationFile == mConfigurationFilePath)
		return;

	if (this->isConfigured())
	{
		connect(this, SIGNAL(deconfigured()), this, SLOT(configureAfterDeconfigureSlot()));
		this->deconfigure();
	}

	mConfigurationFilePath = configurationFile;
}

void TrackingSystemIGSTKService::setLoggingFolder(QString loggingFolder)
{
	if (mLoggingFolder == loggingFolder)
		return;

	if (this->isConfigured())
	{
		connect(this, SIGNAL(deconfigured()), this, SLOT(configureAfterDeconfigureSlot()));
		this->deconfigure();
	}

	mLoggingFolder = loggingFolder;
}

void TrackingSystemIGSTKService::initializedSlot(bool value)
{
	if (value)
	{
		mState = Tool::tsINITIALIZED;
		reportSuccess("IGSTK Tracking Service is initialized.");
		emit stateChanged();
		emit initialized();
	}
	else
	{
		mState = Tool::tsCONFIGURED;
		report("IGSTK Tracking Service is uninitialized.");
		emit stateChanged();
		emit uninitialized();
	}
}

void TrackingSystemIGSTKService::trackerTrackingSlot(bool value)
{
	if (value)
	{
		mState = Tool::tsTRACKING;
		reportSuccess("IGSTK Tracking Service started tracking.");
		emit stateChanged();
		emit trackingStarted();
	}
	else
	{
		mState = Tool::tsINITIALIZED;
		reportSuccess("IGSTK Tracking Service stopped tracking.");
		emit stateChanged();
		emit trackingStopped();
	}
}

void TrackingSystemIGSTKService::startTrackingAfterInitSlot()
{
	disconnect(this, SIGNAL(initialized()), this, SLOT(startTrackingAfterInitSlot()));
	this->startTracking();
}

void TrackingSystemIGSTKService::initializeAfterConfigSlot()
{
	disconnect(this, SIGNAL(configured()), this, SLOT(initializeAfterConfigSlot()));
	this->initialize();
}

void TrackingSystemIGSTKService::uninitializeAfterTrackingStoppedSlot()
{
	disconnect(this, SIGNAL(trackingStopped()), this, SLOT(uninitializeAfterTrackingStoppedSlot()));
	this->uninitialize();
}

void TrackingSystemIGSTKService::deconfigureAfterUninitializedSlot()
{
	disconnect(this, SIGNAL(uninitialized()), this, SLOT(deconfigureAfterUninitializedSlot()));
	this->deconfigure();
}

void TrackingSystemIGSTKService::configureAfterDeconfigureSlot()
{
	disconnect(this, SIGNAL(deconfigured()), this, SLOT(configureAfterDeconfigureSlot()));
	this->configure();
}

void TrackingSystemIGSTKService::globalConfigurationFileChangedSlot(QString key)
{
	if (key == "toolConfigFile")
	{
		this->setConfigurationFile(profile()->getToolConfigFilePath());
	}
}

TrackerConfigurationPtr TrackingSystemIGSTKService::getConfiguration()
{
	TrackerConfigurationPtr retval;
	retval.reset(new TrackerConfigurationImpl());
	return retval;
}

void TrackingSystemIGSTKService::destroyTrackerThread()
{
	if (mTrackerThread)
	{
		mTrackerThread->quit();
		mTrackerThread->wait(2000);
		if (mTrackerThread->isRunning())
		{
			mTrackerThread->terminate();
			mTrackerThread->wait(); // forever or until dead thread
		}
		QObject::disconnect(mTrackerThread.get());
		mTrackerThread.reset();
	}
}

} //namespace cx
