// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#define _USE_MATH_DEFINES

#include "cxToolManagerUsingIGSTK.h"

#include <QTimer>
#include <QDir>
#include <QList>
#include <QMetaType>
#include <QFileInfo>
#include <vtkDoubleArray.h>
#include <QCoreApplication>

#include "cxRegistrationTransform.h"
#include "cxReporter.h"
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
#include "cxLogger.h"
#include "cxPlaybackTime.h"
#include "cxTrackingPositionFilter.h"

namespace cx
{

ToolManagerUsingIGSTK::ToolManagerUsingIGSTKPtr ToolManagerUsingIGSTK::create()
{
	ToolManagerUsingIGSTKPtr retval;
	retval.reset(new ToolManagerUsingIGSTK());
	retval->mSelf = retval;

	retval->initializeManualTool(); // do this after setting self.
	retval->setDominantTool("ManualTool");

	return retval;
}

QStringList ToolManagerUsingIGSTK::getSupportedTrackingSystems()
{
	QStringList retval;
	retval = IgstkTracker::getSupportedTrackingSystems();
	return retval;
}

ToolManagerUsingIGSTK::ToolManagerUsingIGSTK() :
				mConfigurationFilePath(""),
				mLoggingFolder(""),
				mConfigured(false),
				mInitialized(false),
				mTracking(false),
				mPlayBackMode(false),
				mLastLoadPositionHistory(0),
				mToolTipOffset(0)
{
	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(globalConfigurationFileChangedSlot(QString)));
	// initialize config file
	this->setConfigurationFile(DataLocations::getToolConfigFilePath());
}

ToolManagerUsingIGSTK::~ToolManagerUsingIGSTK()
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
	}
}

/**Set playback mode.
 * Set endpoints into controller.
 *
 * The original tools are wrapped by playback tools. The original ones are
 * not changed, only their movement is ignored.
 */
void ToolManagerUsingIGSTK::setPlaybackMode(PlaybackTimePtr controller)
{
	if (!controller)
	{
		this->closePlayBackMode();
		return;
	}

	// attempt to configure tracker if not configured:
	if (!this->isConfigured())
		this->configure();

	if (!mTrackerThread)
		return;

	// wait for connection to complete
	for (unsigned i=0; i<100; ++i)
	{
		if (this->isConfigured())
			break;
		qApp->processEvents();
		mTrackerThread->wait(100);
	}

	if (!this->isConfigured())
	{
		reportWarning("ToolManager must be configured before setting playback");
		return;
	}

	ToolManager::ToolMap original = mTools; ///< all tools
	mTools.clear();

	std::pair<double,double> timeRange(getMilliSecondsSinceEpoch(), 0);

	for (ToolManager::ToolMap::iterator iter = original.begin(); iter!=original.end(); ++iter)
	{
		if (iter->second==mManualTool)
			continue; // dont wrap the manual tool
		cx::PlaybackToolPtr current(new PlaybackTool(mSelf.lock(), iter->second, controller));
		mTools[current->getUid()] = current;

		TimedTransformMapPtr history = iter->second->getPositionHistory();
		if (!history->empty())
		{
			timeRange.first = std::min(timeRange.first, history->begin()->first);
			timeRange.second = std::max(timeRange.second, history->rbegin()->first);
		}
	}
	mTools[mManualTool->getUid()] = mManualTool;

	controller->initialize(QDateTime::fromMSecsSinceEpoch(timeRange.first), timeRange.second - timeRange.first);

	report("Opened Playback Mode");
	mPlayBackMode = true;
	emit initialized();
}

/**Close playback mode by removing the playback tools and resetting to the original tools
 *
 */
void ToolManagerUsingIGSTK::closePlayBackMode()
{
	ToolManager::ToolMap original = mTools; ///< all tools

	for (ToolManager::ToolMap::iterator iter = original.begin(); iter!=original.end(); ++iter)
	{
		if (iter->second==mManualTool)
			continue; // dont unwrap the manual tool
		PlaybackToolPtr current = boost::dynamic_pointer_cast<PlaybackTool>(iter->second);
		if (current)
			mTools[current->getBase()->getUid()] = current->getBase();
	}
	mTools[mManualTool->getUid()] = mManualTool;

	report("Closed Playback Mode");
	mPlayBackMode = false;
	emit initialized();
}

void ToolManagerUsingIGSTK::runDummyTool(DummyToolPtr tool)
{
	report("Running dummy tool " + tool->getUid());

	mTools[tool->getUid()] = tool;
	tool->setVisible(true);
	connect(tool.get(), SIGNAL(toolVisible(bool)), this, SLOT(dominantCheckSlot()));
    tool->startTracking(30);
	this->setDominantTool(tool->getUid());

	report("Dummy: Config/Init/Track started in toolManager");
	mConfigured = true;
	emit configured();
	this->initializedSlot(true);
	this->trackerTrackingSlot(true);
}



void ToolManagerUsingIGSTK::initializeManualTool()
{
	if (!mManualTool)
	{
		//adding a manual tool as default
		mManualTool.reset(new ManualToolAdapter(mSelf.lock(), "ManualTool"));
		mTools["ManualTool"] = mManualTool;
		mManualTool->setVisible(true);
//    mManualTool->setVisible(settings()->value("showManualTool").toBool());
		connect(mManualTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(dominantCheckSlot()));
	}

	Transform3D rMpr = Transform3D::Identity(); // not known: not really important either
	Transform3D prMt = rMpr.inv() * createTransformRotateY(M_PI) * createTransformRotateZ(M_PI/2);
	mManualTool->set_prMt(prMt);
}

bool ToolManagerUsingIGSTK::isConfigured() const
{
	return mConfigured;
}
bool ToolManagerUsingIGSTK::isInitialized() const
{
	return mInitialized;
}
bool ToolManagerUsingIGSTK::isTracking() const
{
	return mTracking;
}
void ToolManagerUsingIGSTK::configure()
{
	if (mConfigurationFilePath.isEmpty() || !QFile::exists(mConfigurationFilePath))
	{
		reportWarning("Configuration file is not valid, could not configure the toolmanager.");
		return;
	}

	//parse
	ConfigurationFileParser configParser(mConfigurationFilePath, mLoggingFolder);

	std::vector<IgstkTracker::InternalStructure> trackers = configParser.getTrackers();

	if (trackers.empty())
	{
		reportWarning("Failed to configure tracking.");
		return;
	}

	IgstkTracker::InternalStructure trackerStructure = trackers[0]; //we only support one tracker atm

	IgstkTool::InternalStructure referenceToolStructure;
	std::vector<IgstkTool::InternalStructure> toolStructures;
	QString referenceToolFile = configParser.getAbsoluteReferenceFilePath();
	std::vector<QString> toolfiles = configParser.getAbsoluteToolFilePaths();
	for (std::vector<QString>::iterator it = toolfiles.begin(); it != toolfiles.end(); ++it)
	{
		ToolFileParser toolParser(*it, mLoggingFolder);
		IgstkTool::InternalStructure internalTool = toolParser.getTool();
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
// too strong: config is independent of tracker thread errors
//	connect(mTrackerThread.get(), SIGNAL(error()), this, SLOT(deconfigure()));
	connect(mTrackerThread.get(), SIGNAL(error()), this, SLOT(uninitialize()));

	//start threads
	if (mTrackerThread)
		mTrackerThread->start();
}

void ToolManagerUsingIGSTK::trackerConfiguredSlot(bool on)
{
	if (!on)
	{
		this->deconfigure();
		return;
	}

	if (!mTrackerThread)
	{
		reporter()->sendDebug(
						"Received a configured signal in ToolManager, but we don't have a mTrackerThread, this should never happen, contact programmer.");
		return;
	}

	//new all tools
	std::map<QString, IgstkToolPtr> igstkTools = mTrackerThread->getTools();
	IgstkToolPtr reference = mTrackerThread->getRefereceTool();
	std::map<QString, IgstkToolPtr>::iterator it = igstkTools.begin();
	for (; it != igstkTools.end(); ++it)
	{
		IgstkToolPtr igstkTool = it->second;
		cxToolPtr tool(new ToolUsingIGSTK(mSelf.lock(), igstkTool));
		if (tool->isValid())
		{
			if (igstkTool == reference)
				mReferenceTool = tool;

			mTools[it->first] = tool;
			connect(tool.get(), SIGNAL(toolVisible(bool)), this, SLOT(dominantCheckSlot()));
			if (tool->hasType(ToolUsingIGSTK::TOOL_US_PROBE))
				emit probeAvailable();
		}
		else
			reportWarning("Creation of the cxTool " + it->second->getUid() + " failed.");
	}

	// debug: give the manual tool properties from the first non-manual tool. Nice for testing tools
	if (settings()->value("giveManualToolPhysicalProperties").toBool())
	{
		for (ToolManager::ToolMap::iterator iter = mTools.begin(); iter != mTools.end(); ++iter)
		{
			if (iter->second == mManualTool)
				continue;
			if (iter->second->hasType(ToolUsingIGSTK::TOOL_REFERENCE))
				continue;
			mManualTool->setBase(iter->second);
			report("Manual tool imbued with properties from " + iter->first);
			break;
		}
	}
	//debug stop

	this->setDominantTool(this->getManualTool()->getUid());

	mConfigured = true;

	this->loadPositionHistory(); // the tools are always reconfigured after a setloggingfolder

	reportSuccess("ToolManager is configured.");
	emit configured();
}

void ToolManagerUsingIGSTK::deconfigure()
{
	if (this->isInitialized())
	{
		connect(this, SIGNAL(uninitialized()), this, SLOT(deconfigureAfterUninitializedSlot()));
		this->uninitialize();
		return;
	}

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

	this->setDominantTool(this->getManualTool()->getUid());

	mConfigured = false;
	emit
	deconfigured();
	report("ToolManager is deconfigured.");
}

void ToolManagerUsingIGSTK::initialize()
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
		reportError(
						"Cannot initialize the tracking system because the tracking thread does not exist.");
}

void ToolManagerUsingIGSTK::uninitialize()
{
	if (this->isTracking())
	{
		connect(this, SIGNAL(trackingStopped()), this, SLOT(uninitializeAfterTrackingStoppedSlot()));
		this->stopTracking();
		return;
	}

	if (!this->isInitialized())
	{
//		report("No need to uninitialize, toolmanager is not initialized.");
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
bool ToolManagerUsingIGSTK::createSymlink()
{
	bool retval = true;
	QFileInfo symlink = this->getSymlink();
	QDir linkDir(symlink.absolutePath());
	QString linkfile = symlink.absoluteFilePath();
	;

	if (!linkDir.exists())
	{
		reportError(
						QString("Folder %1 does not exist. System is not properly installed.").arg(linkDir.path()));
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

QFileInfo ToolManagerUsingIGSTK::getSymlink() const
{
	QString name("/Library/CustusX/igstk.links");
	QDir linkDir(name);
	QDir::root().mkdir(name); // only works if run with sudo
	QString linkFile = linkDir.path() + "/cu.CustusX.dev0";
	return QFileInfo(linkDir, linkFile);
}

/** removes symlinks to tracking system created during setup
 */
void ToolManagerUsingIGSTK::cleanupSymlink()
{
	report("Cleaning up symlinks.");
	QFile(this->getSymlink().absoluteFilePath()).remove();
}
#endif //WIN32
void ToolManagerUsingIGSTK::startTracking()
{
	if (!this->isInitialized())
	{
		connect(this, SIGNAL(initialized()), this, SLOT(startTrackingAfterInitSlot()));
		this->initialize();
		return;
	}

	if (!mInitialized)
	{
		reportWarning("Please initialize before trying to start tracking.");
		return;
	}
	if (mTrackerThread)
		mTrackerThread->track(true);
}

void ToolManagerUsingIGSTK::stopTracking()
{
	if (!mTracking)
	{
//		reportWarning("Please start tracking before trying to stop tracking.");
		return;
	}
	if (mTrackerThread)
		mTrackerThread->track(false);
}

void ToolManagerUsingIGSTK::saveToolsSlot()
{
	this->savePositionHistory();
//	report("Transforms and timestamps are saved for connected tools.");
}

SessionToolHistoryMap ToolManagerUsingIGSTK::getSessionHistory(double startTime, double stopTime)
{
	SessionToolHistoryMap retval;

	ToolManager::ToolMapPtr tools = this->getTools();
	ToolManager::ToolMap::iterator it = tools->begin();
	for (; it != tools->end(); ++it)
	{
		TimedTransformMap toolMap = it->second->getSessionHistory(startTime, stopTime);
		if (toolMap.empty())
			continue;
		retval[it->second] = toolMap;
	}
	return retval;
}

ToolManager::ToolMapPtr ToolManagerUsingIGSTK::getConfiguredTools()
{
	ToolManager::ToolMap retval;
	ToolManager::ToolMap::iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
	{
		if (!it->second->isInitialized())
			retval[it->first] = it->second;
	}

	return ToolManager::ToolMapPtr(new ToolManager::ToolMap(retval));
}

ToolManager::ToolMapPtr ToolManagerUsingIGSTK::getInitializedTools()
{
	ToolManager::ToolMap retval;
	ToolManager::ToolMap::iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
	{
		if (it->second->isInitialized())
			retval[it->first] = it->second;
	}
	return ToolManager::ToolMapPtr(new ToolManager::ToolMap(retval));
}

ToolManager::ToolMapPtr ToolManagerUsingIGSTK::getTools()
{
	return ToolManager::ToolMapPtr(new ToolManager::ToolMap(mTools));
}

ToolPtr ToolManagerUsingIGSTK::getTool(const QString& uid)
{
	if (uid == "active")
		return this->getDominantTool();

	ToolPtr retval;
	ToolManager::ToolMap::iterator it = mTools.find(uid);
	if (it != mTools.end())
		retval = it->second;

	return retval;
}

void ToolManagerUsingIGSTK::setTooltipOffset(double offset)
{
	if (similar(offset, mToolTipOffset))
		return;
	mToolTipOffset = offset;
	emit tooltipOffset(mToolTipOffset);
}
double ToolManagerUsingIGSTK::getTooltipOffset() const
{
	return mToolTipOffset;
}
;

ToolPtr ToolManagerUsingIGSTK::getDominantTool()
{
	return mDominantTool;
}

void ToolManagerUsingIGSTK::setDominantTool(const QString& uid)
{
	if (mDominantTool && mDominantTool->getUid() == uid)
		return;

	if (mDominantTool)
	{
		// make manual tool invisible when other tools are active.
		if (mDominantTool->hasType(ToolUsingIGSTK::TOOL_MANUAL))
		{
			mManualTool->setVisible(false);
		}
	}

	ToolPtr newTool;
	newTool = this->getTool(uid);

	// special case for manual tool
	if (newTool && newTool->hasType(ToolUsingIGSTK::TOOL_MANUAL) && mManualTool)
	{
		if (mDominantTool)
		{
			mManualTool->set_prMt(mDominantTool->get_prMt());
		}
		mManualTool->setVisible(true);
	}

	if (mDominantTool)
		disconnect(mDominantTool.get(), SIGNAL(tps(int)), this, SIGNAL(tps(int)));
	mDominantTool = newTool;
	connect(mDominantTool.get(), SIGNAL(tps(int)), this, SIGNAL(tps(int)));

	if (mDominantTool->hasType(ToolUsingIGSTK::TOOL_MANUAL))
		emit tps(0);

//	report("Change active tool to: " + mDominantTool->getName());

	emit dominantToolChanged(uid);
}

std::map<QString, QString> ToolManagerUsingIGSTK::getToolUidsAndNames() const
{
	std::map<QString, QString> uidsAndNames;

	ToolManager::ToolMap::const_iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
		uidsAndNames[it->second->getUid()] = it->second->getName();

	return uidsAndNames;
}

std::vector<QString> ToolManagerUsingIGSTK::getToolNames() const
{
	std::vector<QString> names;

	ToolManager::ToolMap::const_iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
		names.push_back(it->second->getName());

	return names;
}

std::vector<QString> ToolManagerUsingIGSTK::getToolUids() const
{
	std::vector<QString> uids;

	ToolManager::ToolMap::const_iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
		uids.push_back(it->second->getUid());

	return uids;
}

ToolPtr ToolManagerUsingIGSTK::getReferenceTool() const
{
	return mReferenceTool;
}

void ToolManagerUsingIGSTK::savePositionHistory()
{
	QString filename = mLoggingFolder + "/toolpositions.snwpos";

	PositionStorageWriter writer(filename);

	ToolManager::ToolMap::iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
	{
		ToolPtr current = it->second;
		TimedTransformMapPtr data = current->getPositionHistory();

		if (!data)
			continue;

		// save only data acquired after mLastLoadPositionHistory:
		TimedTransformMap::iterator iter = data->lower_bound(mLastLoadPositionHistory);
		for (; iter != data->end(); ++iter)
			writer.write(iter->second, (iter->first), current->getUid());
	}

	mLastLoadPositionHistory = getMilliSecondsSinceEpoch();
}

void ToolManagerUsingIGSTK::loadPositionHistory()
{
	QString filename = mLoggingFolder + "/toolpositions.snwpos";

	PositionStorageReader reader(filename);

	Transform3D matrix = Transform3D::Identity();
	double timestamp;
	QString toolUid;

	QStringList missingTools;

	while (!reader.atEnd())
	{
		if (!reader.read(&matrix, &timestamp, &toolUid))
			break;

		ToolPtr current = this->getTool(toolUid);
		if (current)
		{
			(*current->getPositionHistory())[timestamp] = matrix;
		}
		else
		{
			missingTools << toolUid;
		}
	}

	missingTools.removeDuplicates();
	missingTools.removeAll("");

	if (!missingTools.empty())
	{
		reportWarning(
						QString("Loaded position history.\n"
								"The following tools were found in the history\n"
								"but not in the configuration:\n%1").arg(missingTools.join(", ")));
	}

	mLastLoadPositionHistory = getMilliSecondsSinceEpoch();
}

void ToolManagerUsingIGSTK::setConfigurationFile(QString configurationFile)
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

void ToolManagerUsingIGSTK::setLoggingFolder(QString loggingFolder)
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

void ToolManagerUsingIGSTK::initializedSlot(bool value)
{
	mInitialized = value;
	if (mInitialized)
	{
		reportSuccess("ToolManager is initialized.");
		emit initialized();
	}
	else
	{
		report("ToolManager is uninitialized.");
		emit uninitialized();
	}
}

void ToolManagerUsingIGSTK::trackerTrackingSlot(bool value)
{
	mTracking = value;
	if (mTracking)
	{
		reportSuccess("ToolManager started tracking.");
		emit trackingStarted();
	}
	else
	{
		reportSuccess("ToolManager stopped tracking.");
		emit trackingStopped();
	}
}

void ToolManagerUsingIGSTK::startTrackingAfterInitSlot()
{
	disconnect(this, SIGNAL(initialized()), this, SLOT(startTrackingAfterInitSlot()));
	this->startTracking();
}

void ToolManagerUsingIGSTK::initializeAfterConfigSlot()
{
	disconnect(this, SIGNAL(configured()), this, SLOT(initializeAfterConfigSlot()));
	this->initialize();
}

void ToolManagerUsingIGSTK::uninitializeAfterTrackingStoppedSlot()
{
	disconnect(this, SIGNAL(trackingStopped()), this, SLOT(uninitializeAfterTrackingStoppedSlot()));
	this->uninitialize();
}

void ToolManagerUsingIGSTK::deconfigureAfterUninitializedSlot()
{
	disconnect(this, SIGNAL(uninitialized()), this, SLOT(deconfigureAfterUninitializedSlot()));
	this->deconfigure();
}

void ToolManagerUsingIGSTK::configureAfterDeconfigureSlot()
{
	disconnect(this, SIGNAL(deconfigured()), this, SLOT(configureAfterDeconfigureSlot()));
	this->configure();
}

void ToolManagerUsingIGSTK::globalConfigurationFileChangedSlot(QString key)
{
	if (key == "toolConfigFile")
	{
		this->setConfigurationFile(DataLocations::getToolConfigFilePath());
	}
	if (key.contains("TrackingPositionFilter"))
	{
		this->resetTrackingPositionFilters();
	}
}

void ToolManagerUsingIGSTK::resetTrackingPositionFilters()
{
	bool enabled = settings()->value("TrackingPositionFilter/enabled", false).toInt();

	for (ToolMap::iterator iter=mTools.begin(); iter!=mTools.end(); ++iter)
	{
		boost::shared_ptr<ToolImpl> tool;
		tool = boost::dynamic_pointer_cast<ToolImpl>(iter->second);
		if (!tool)
			continue;
		TrackingPositionFilterPtr filter;
		if (enabled)
			filter.reset(new TrackingPositionFilter());
		tool->resetTrackingPositionFilter(filter);
	}
}

void ToolManagerUsingIGSTK::dominantCheckSlot()
{
	if (this->isPlaybackMode())
	{
		// In static playback mode, tools does not turn invisible since
		// time dont move. Here we check whether manual tool has a newer
		// timestamp than the playback tools. If it has, make it dominant.
		// This enables automatic change to manual tool if the user
		// manipulates the manual tool in some way.
		double bestTime = 0;
		for (ToolMap::iterator it = mTools.begin(); it != mTools.end(); ++it)
		{
			if (it->second->hasType(ToolUsingIGSTK::TOOL_MANUAL))
				continue;
			bestTime = std::max(bestTime, it->second->getTimestamp());
		}
		if (bestTime < this->getManualTool()->getTimestamp())
		{
			this->setDominantTool(this->getManualTool()->getUid());
			return;
		}
	}

	bool use = settings()->value("Automation/autoSelectDominantTool").toBool();
	if (!use)
		return;

	//make a sorted vector of all visible tools
	std::vector<ToolPtr> visibleTools;
	ToolMap::iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
	{
		//TODO need to check if init???
		if (it->second->getVisible())
			visibleTools.push_back(it->second);
		else if (it->second->hasType(ToolUsingIGSTK::TOOL_MANUAL))
			visibleTools.push_back(it->second);
	}

	if (!visibleTools.empty())
	{
		//sort most important tool to the start of the vector:
		sort(visibleTools.begin(), visibleTools.end(), toolTypeSort);
		const QString uid = visibleTools.at(0)->getUid();
		this->setDominantTool(uid);
	}
}

namespace
{
/**
 * \return a priority of the tool. High means this tool is preferred more.
 */
int getPriority(ToolPtr tool)
{
	if (tool->hasType(ToolUsingIGSTK::TOOL_MANUAL)) // place this first, in case a tool has several attributes.
		return 2;

	if (tool->hasType(ToolUsingIGSTK::TOOL_US_PROBE))
		return 4;
	if (tool->hasType(ToolUsingIGSTK::TOOL_POINTER))
		return 3;
	if (tool->hasType(ToolUsingIGSTK::TOOL_REFERENCE))
		return 1;
	return 0;
}
}

/**
 * sorts tools in descending order of type
 * @param tool1 the first tool
 * @param tool2 the second tool
 * @return whether the second tool is of higher priority than the first or not
 */
bool toolTypeSort(const ToolPtr tool1, const ToolPtr tool2)
{
	return getPriority(tool2) < getPriority(tool1);
}

void ToolManagerUsingIGSTK::addXml(QDomNode& parentNode)
{
	QDomDocument doc = parentNode.ownerDocument();
	QDomElement base = doc.createElement("toolManager");
	parentNode.appendChild(base);

	QDomElement manualToolNode = doc.createElement("manualTool");
	manualToolNode.appendChild(doc.createTextNode("\n" + qstring_cast(mManualTool->get_prMt())));
	base.appendChild(manualToolNode);

	//Tools
	QDomElement toolsNode = doc.createElement("tools");
	ToolManager::ToolMapPtr tools = this->getTools();
	ToolManager::ToolMap::iterator toolIt = tools->begin();
	for (; toolIt != tools->end(); ++toolIt)
	{
		QDomElement toolNode = doc.createElement("tool");
		cxToolPtr tool = boost::dynamic_pointer_cast<ToolUsingIGSTK>(toolIt->second);
		if (tool)
		{
			tool->addXml(toolNode);
			toolsNode.appendChild(toolNode);
		}
	}
	base.appendChild(toolsNode);
}

void ToolManagerUsingIGSTK::clear()
{
	mManualTool->set_prMt(Transform3D::Identity());
}

void ToolManagerUsingIGSTK::parseXml(QDomNode& dataNode)
{
	if (dataNode.isNull())
		return;

	QString manualToolText = dataNode.namedItem("manualTool").toElement().text();
	mManualTool->set_prMt(Transform3D::fromString(manualToolText));

	//Tools
	ToolManager::ToolMapPtr tools = this->getTools();
	QDomNode toolssNode = dataNode.namedItem("tools");
	QDomElement toolNode = toolssNode.firstChildElement("tool");
	for (; !toolNode.isNull(); toolNode = toolNode.nextSiblingElement("tool"))
	{
		QDomElement base = toolNode.toElement();
		QString tool_uid = base.attribute("uid");
		if (tools->find(tool_uid) != tools->end())
		{
			cxToolPtr tool = boost::dynamic_pointer_cast<ToolUsingIGSTK>(tools->find(tool_uid)->second);
			tool->parseXml(toolNode);
		}
	}
}

ManualToolPtr ToolManagerUsingIGSTK::getManualTool()
{
	return mManualTool;
}

/**\brief Find a probe that can be connected to a rt source.
 *
 * Priority:
 *  - active probe
 *  - visible probe
 *  - any probe
 *
 */
ToolPtr ToolManagerUsingIGSTK::findFirstProbe()
{
	ToolPtr active = this->getDominantTool();
	if (active && active->getProbe() && active->getProbe()->isValid())
	{
		return active;
	}

	ToolManager::ToolMapPtr tools = this->getTools();

	// look for visible probes
	for (ToolManager::ToolMap::iterator iter = tools->begin(); iter != tools->end(); ++iter)
	{
		if (iter->second->getProbe() && iter->second->getProbe()->isValid() && iter->second->getVisible())
		{
			return iter->second;
		}
	}

	// pick the first probe, visible or not.
	for (ToolManager::ToolMap::iterator iter = tools->begin(); iter != tools->end(); ++iter)
	{
		if (iter->second->getProbe() && iter->second->getProbe()->isValid())
		{
			return iter->second;
		}
	}

	return ToolPtr();
}


} //namespace cx
