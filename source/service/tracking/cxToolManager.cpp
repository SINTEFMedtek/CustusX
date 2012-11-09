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

#include "cxToolManager.h"

#include <QTimer>
#include <QDir>
#include <QList>
#include <QMetaType>
#include <QFileInfo>
#include <vtkDoubleArray.h>

#include "sscRegistrationTransform.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscPositionStorageFile.h"
#include "sscTime.h"
#include "sscEnumConverter.h"
#include "sscDummyTool.h"
#include "cxTool.h"
#include "cxIgstkTracker.h"
#include "cxToolConfigurationParser.h"
//#include "cxRecordSession.h"
#include "cxManualToolAdapter.h"
#include "cxSettings.h"
#include "cxDataLocations.h"
#include "cxIgstkTrackerThread.h"
#include "cxPlaybackTool.h"
#include "sscLogger.h"

namespace cx
{

void ToolManager::initializeObject()
{
	ssc::ToolManager::setInstance(new ToolManager());
}

ToolManager* ToolManager::getInstance()
{
	return dynamic_cast<ToolManager*>(ssc::ToolManager::getInstance());
}

QStringList ToolManager::getSupportedTrackingSystems()
{
	QStringList retval;
	retval = IgstkTracker::getSupportedTrackingSystems();
	return retval;
}

ToolManager::ToolManager() :
				mConfigurationFilePath(""),
				mLoggingFolder(""),
				mConfigured(false),
				mInitialized(false),
				mTracking(false),
				mPlayBackMode(false),
				mLastLoadPositionHistory(0),
				mToolTipOffset(0)
{
	m_rMpr_History.reset(new ssc::RegistrationHistory());
	connect(m_rMpr_History.get(), SIGNAL(currentChanged()), this, SIGNAL(rMprChanged()));

	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(globalConfigurationFileChangedSlot(QString)));

	this->initializeManualTool();
	this->setDominantTool("ManualTool");

	// initialize config file
	this->setConfigurationFile(DataLocations::getToolConfigFilePath());
}

ToolManager::~ToolManager()
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
void ToolManager::setPlaybackMode(PlaybackTimePtr controller)
{
	if (!controller)
	{
		this->closePlayBackMode();
		return;
	}

	if (!this->isConfigured())
	{
		ssc::messageManager()->sendWarning("ToolManager must be configured before setting playback");
		return;
	}

//	QDateTime now = QDateTime::currentDateTime();
//	ssc::ToolPtr test = mTools["Ultrasonix_L14-5"];
//	if (test)
//	{
//		ssc::TimedTransformMapPtr history = test->getPositionHistory();
//		QDateTime start = now.addSecs(-60*60); // 1h back
//		std::cout << "adding test data to probe " << start.toString(ssc::timestampMilliSecondsFormatNice()) << std::endl;
//
//		for (int i=0; i<3000; ++i)
//		{
//			start = start.addMSecs(20);
//			ssc::Transform3D pos = ssc::createTransformTranslate(ssc::Vector3D(i*0.1,0,0));
//			history->insert(std::make_pair(start.toMSecsSinceEpoch(), pos));
//		}
//
//		start = start.addSecs(1*60); // 10 minutes back
//
//		for (int i=0; i<2000; ++i)
//		{
//			start = start.addMSecs(20);
//			ssc::Transform3D pos = ssc::createTransformTranslate(ssc::Vector3D(100, i*0.1,0));
//			history->insert(std::make_pair(start.toMSecsSinceEpoch(), pos));
//		}
//
//	}
//
//	ssc::ToolPtr test2 = mTools["01-117-0329_Planning-Navigator"];
//	if (test2)
//	{
//		ssc::TimedTransformMapPtr history = test2->getPositionHistory();
//		QDateTime start = now.addSecs(-60*60); // 1h back
//		std::cout << "adding test data to pointer " << start.toString(ssc::timestampMilliSecondsFormatNice()) << std::endl;
//		start = start.addSecs(5);
//
//		for (int i=0; i<2000; ++i)
//		{
//			start = start.addMSecs(20);
//			ssc::Transform3D pos = ssc::createTransformTranslate(ssc::Vector3D(i*0.1,0,0));
//			history->insert(std::make_pair(start.toMSecsSinceEpoch(), pos));
//		}
//	}

	ssc::ToolManager::ToolMap original = mTools; ///< all tools
	mTools.clear();

	std::pair<double,double> timeRange(ssc::getMilliSecondsSinceEpoch(), 0);

	for (ssc::ToolManager::ToolMap::iterator iter = original.begin(); iter!=original.end(); ++iter)
	{
		if (iter->second==mManualTool)
			continue; // dont wrap the manual tool
		cx::PlaybackToolPtr current(new PlaybackTool(iter->second, controller));
		mTools[current->getUid()] = current;

//		std::cout << "tool: " << iter->first  << std::endl;
		ssc::TimedTransformMapPtr history = iter->second->getPositionHistory();
		if (!history->empty())
		{
			timeRange.first = std::min(timeRange.first, history->begin()->first);
			timeRange.second = std::max(timeRange.second, history->rbegin()->first);
//			std::cout << "===t start " << QDateTime::fromMSecsSinceEpoch(history->begin()->first).toString(ssc::timestampMilliSecondsFormatNice()) << std::endl;
//			std::cout << "===t  end " << QDateTime::fromMSecsSinceEpoch(history->rbegin()->first).toString(ssc::timestampMilliSecondsFormatNice()) << std::endl;
		}
	}
	mTools[mManualTool->getUid()] = mManualTool;

//	std::cout << "toolmanager" << std::endl;
//	std::cout << "===start " << QDateTime::fromMSecsSinceEpoch(timeRange.first).toString(ssc::timestampMilliSecondsFormatNice()) << std::endl;
//	std::cout << "===  end " << QDateTime::fromMSecsSinceEpoch(timeRange.second).toString(ssc::timestampMilliSecondsFormatNice()) << std::endl;
	controller->initialize(QDateTime::fromMSecsSinceEpoch(timeRange.first), timeRange.second - timeRange.first);

	ssc::messageManager()->sendInfo("Opened Playback Mode");
	mPlayBackMode = true;
	emit initialized();
}

/**Close playback mode by removing the playback tools and resetting to the original tools
 *
 */
void ToolManager::closePlayBackMode()
{
	ssc::ToolManager::ToolMap original = mTools; ///< all tools

	for (ssc::ToolManager::ToolMap::iterator iter = original.begin(); iter!=original.end(); ++iter)
	{
		if (iter->second==mManualTool)
			continue; // dont unwrap the manual tool
		PlaybackToolPtr current = boost::shared_dynamic_cast<PlaybackTool>(iter->second);
		if (current)
			mTools[current->getBase()->getUid()] = current->getBase();
	}
	mTools[mManualTool->getUid()] = mManualTool;

	ssc::messageManager()->sendInfo("Closed Playback Mode");
	mPlayBackMode = false;
	emit initialized();
}

void ToolManager::runDummyTool(ssc::DummyToolPtr tool)
{
	ssc::messageManager()->sendInfo("Running dummy tool " + tool->getUid());

	mTools[tool->getUid()] = tool;
	tool->setVisible(true);
	connect(tool.get(), SIGNAL(toolVisible(bool)), this, SLOT(dominantCheckSlot()));
	tool->startTracking(20);
	this->setDominantTool(tool->getUid());

	emit initialized();
}

void ToolManager::initializeManualTool()
{
	if (!mManualTool)
	{
		//adding a manual tool as default
		mManualTool.reset(new ManualToolAdapter(this, "ManualTool"));
		mTools["ManualTool"] = mManualTool;
		mManualTool->setVisible(true);
//    mManualTool->setVisible(settings()->value("showManualTool").toBool());
		connect(mManualTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(dominantCheckSlot()));
	}

	ssc::Transform3D prMt = this->get_rMpr()->inv() * ssc::createTransformRotateY(M_PI)
					* ssc::createTransformRotateZ(M_PI/2);
	mManualTool->set_prMt(prMt);
}

bool ToolManager::isConfigured() const
{
	return mConfigured;
}
bool ToolManager::isInitialized() const
{
	return mInitialized;
}
bool ToolManager::isTracking() const
{
	return mTracking;
}
void ToolManager::configure()
{
	if (mConfigurationFilePath.isEmpty() || !QFile::exists(mConfigurationFilePath))
	{
		ssc::messageManager()->sendWarning("Configuration file is not valid, could not configure the toolmanager.");
		return;
	}

	//parse
	ConfigurationFileParser configParser(mConfigurationFilePath, mLoggingFolder);

	std::vector<IgstkTracker::InternalStructure> trackers = configParser.getTrackers();

	if (trackers.empty())
	{
		ssc::messageManager()->sendWarning("Failed to configure tracking.");
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

void ToolManager::trackerConfiguredSlot(bool on)
{
	if (!on)
	{
		this->deconfigure();
		return;
	}

	if (!mTrackerThread)
	{
		ssc::messageManager()->sendDebug(
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
		ToolPtr tool(new Tool(igstkTool));
		if (tool->isValid())
		{
			if (igstkTool == reference)
				mReferenceTool = tool;

			mTools[it->first] = tool;
			connect(tool.get(), SIGNAL(toolVisible(bool)), this, SLOT(dominantCheckSlot()));
		}
		else
			ssc::messageManager()->sendWarning("Creation of the cxTool " + it->second->getUid() + " failed.");
	}

	// debug: give the manual tool properties from the first non-manual tool. Nice for testing tools
	if (settings()->value("giveManualToolPhysicalProperties").toBool())
	{
		for (ssc::ToolManager::ToolMap::iterator iter = mTools.begin(); iter != mTools.end(); ++iter)
		{
			if (iter->second == mManualTool)
				continue;
			if (iter->second->hasType(Tool::TOOL_REFERENCE))
				continue;
			mManualTool->setBase(iter->second);
			ssc::messageManager()->sendInfo("Manual tool imbued with properties from " + iter->first);
			break;
		}
	}
	//debug stop

	this->setDominantTool(this->getManualTool()->getUid());

	mConfigured = true;

	this->loadPositionHistory(); // the tools are always reconfigured after a setloggingfolder

	ssc::messageManager()->sendSuccess("ToolManager is configured.", true);
	emit configured();
}

void ToolManager::deconfigure()
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
	ssc::messageManager()->sendInfo("ToolManager is deconfigured.");
}

void ToolManager::initialize()
{
	if (!this->isConfigured())
	{
		connect(this, SIGNAL(configured()), this, SLOT(initializeAfterConfigSlot()));
		this->configure();
		return;
	}

	if (!this->isConfigured())
	{
		ssc::messageManager()->sendWarning("Please configure before trying to initialize.");
		return;
	}

#ifndef WIN32
	if (!this->createSymlink())
	{
		ssc::messageManager()->sendError("Initialization of tracking failed.");
		return;
	}
#endif

	if (mTrackerThread)
		mTrackerThread->initialize(true);
	else
		ssc::messageManager()->sendError(
						"Cannot initialize the tracking system because the tracking thread does not exist.");
}

void ToolManager::uninitialize()
{
	if (this->isTracking())
	{
		connect(this, SIGNAL(trackingStopped()), this, SLOT(uninitializeAfterTrackingStoppedSlot()));
		this->stopTracking();
		return;
	}

	if (!this->isInitialized())
	{
//		ssc::messageManager()->sendInfo("No need to uninitialize, toolmanager is not initialized.");
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
bool ToolManager::createSymlink()
{
	bool retval = true;
	QFileInfo symlink = this->getSymlink();
	QDir linkDir(symlink.absolutePath());
	QString linkfile = symlink.absoluteFilePath();
	;

	if (!linkDir.exists())
	{
		ssc::messageManager()->sendError(
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
		ssc::messageManager()->sendError(
						QString("No usb connections found in /dev using filters %1").arg(filters.join(";")));
		return false;
	}
	else
	{
		ssc::messageManager()->sendInfo(QString("Device files: %1").arg(files.join(",")));
	}

	QString device = devDir.filePath(files[0]);
//  QString device = "/dev/serial/by-id/usb-NDI_NDI_Host_USB_Converter-if00-port0";

	QFile(linkfile).remove();
	QFile devFile(device);
	QFileInfo devFileInfo(device);
	if (!devFileInfo.isWritable())
	{
		ssc::messageManager()->sendError(QString("Device %1 is not writable. Connection will fail.").arg(device));
		retval = false;
	}
	// this call only succeeds if Custus is run as root.
	bool val = devFile.link(linkfile);
	if (!val)
	{
		ssc::messageManager()->sendError(
						QString("Symlink %1 creation to device %2 failed with code %3").arg(linkfile).arg(device).arg(
										devFile.error()));
		retval = false;
	}
	else
	{
		ssc::messageManager()->sendInfo(QString("Created symlink %1 to device %2").arg(linkfile).arg(device));
	}

	devFile.setPermissions(
					QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadGroup | QFile::WriteGroup
									| QFile::ExeGroup | QFile::ReadOther | QFile::WriteOther | QFile::ExeOther);
	return retval;
}

QFileInfo ToolManager::getSymlink() const
{
	QString name("/Library/CustusX/igstk.links");
	QDir linkDir(name);
	QDir::root().mkdir(name); // only works if run with sudo
	QString linkFile = linkDir.path() + "/cu.CustusX.dev0";
	return QFileInfo(linkDir, linkFile);
}

/** removes symlinks to tracking system created during setup
 */
void ToolManager::cleanupSymlink()
{
	ssc::messageManager()->sendInfo("Cleaning up symlinks.");
	QFile(this->getSymlink().absoluteFilePath()).remove();
}
#endif //WIN32
void ToolManager::startTracking()
{
	if (!this->isInitialized())
	{
		connect(this, SIGNAL(initialized()), this, SLOT(startTrackingAfterInitSlot()));
		this->initialize();
		return;
	}

	if (!mInitialized)
	{
		ssc::messageManager()->sendWarning("Please initialize before trying to start tracking.");
		return;
	}
	if (mTrackerThread)
		mTrackerThread->track(true);
}

void ToolManager::stopTracking()
{
	if (!mTracking)
	{
//		ssc::messageManager()->sendWarning("Please start tracking before trying to stop tracking.");
		return;
	}
	if (mTrackerThread)
		mTrackerThread->track(false);
}

void ToolManager::saveToolsSlot()
{
	this->savePositionHistory();
	ssc::messageManager()->sendInfo("Transforms and timestamps are saved for connected tools.");
}

ssc::LandmarkMap ToolManager::getLandmarks()
{
	return mLandmarks;
}

void ToolManager::setLandmark(ssc::Landmark landmark)
{
	mLandmarks[landmark.getUid()] = landmark;
	emit landmarkAdded(landmark.getUid());
}

void ToolManager::removeLandmark(QString uid)
{
	mLandmarks.erase(uid);
	emit landmarkRemoved(uid);
}

void ToolManager::removeLandmarks()
{
	ssc::LandmarkMap landmarks = ssc::toolManager()->getLandmarks();
	ssc::LandmarkMap::iterator it = landmarks.begin();
	for (; it != landmarks.end(); ++it)
	{
		ssc::toolManager()->removeLandmark(it->first);
	}
}

ssc::SessionToolHistoryMap ToolManager::getSessionHistory(double startTime, double stopTime)
{
	ssc::SessionToolHistoryMap retval;

	ssc::ToolManager::ToolMapPtr tools = this->getTools();
	ssc::ToolManager::ToolMap::iterator it = tools->begin();
	for (; it != tools->end(); ++it)
	{
		ssc::TimedTransformMap toolMap = it->second->getSessionHistory(startTime, stopTime);
		if (toolMap.empty())
			continue;
		retval[it->second] = toolMap;
	}
	return retval;
}

ssc::ToolManager::ToolMapPtr ToolManager::getConfiguredTools()
{
	ssc::ToolManager::ToolMap retval;
	ssc::ToolManager::ToolMap::iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
	{
		if (!it->second->isInitialized())
			retval[it->first] = it->second;
	}

	return ssc::ToolManager::ToolMapPtr(new ssc::ToolManager::ToolMap(retval));
}

ssc::ToolManager::ToolMapPtr ToolManager::getInitializedTools()
{
	ssc::ToolManager::ToolMap retval;
	ssc::ToolManager::ToolMap::iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
	{
		if (it->second->isInitialized())
			retval[it->first] = it->second;
	}
	return ssc::ToolManager::ToolMapPtr(new ssc::ToolManager::ToolMap(retval));
}

ssc::ToolManager::ToolMapPtr ToolManager::getTools()
{
	return ssc::ToolManager::ToolMapPtr(new ssc::ToolManager::ToolMap(mTools));
}

ssc::ToolPtr ToolManager::getTool(const QString& uid)
{
	if (uid == "active")
		return this->getDominantTool();

	ssc::ToolPtr retval;
	ssc::ToolManager::ToolMap::iterator it = mTools.find(uid);
	if (it != mTools.end())
		retval = it->second;

	return retval;
}

void ToolManager::setTooltipOffset(double offset)
{
	if (ssc::similar(offset, mToolTipOffset))
		return;
	mToolTipOffset = offset;
	emit tooltipOffset(mToolTipOffset);
}
double ToolManager::getTooltipOffset() const
{
	return mToolTipOffset;
}
;

ssc::ToolPtr ToolManager::getDominantTool()
{
	return mDominantTool;
}

void ToolManager::setDominantTool(const QString& uid)
{
	if (mDominantTool && mDominantTool->getUid() == uid)
		return;

	if (mDominantTool)
	{
		// make manual tool invisible when other tools are active.
		if (mDominantTool->hasType(Tool::TOOL_MANUAL))
		{
			mManualTool->setVisible(false);
		}
	}

	ssc::ToolPtr newTool;
	newTool = this->getTool(uid);

	// special case for manual tool
	if (newTool && newTool->hasType(Tool::TOOL_MANUAL) && mManualTool)
	{
		if (mDominantTool)
		{
			mManualTool->set_prMt(mDominantTool->get_prMt());
			mManualTool->setTooltipOffset(mDominantTool->getTooltipOffset());

		}
		mManualTool->setVisible(true);
//    mManualTool->setVisible(settings()->value("showManualTool").toBool());
	}

	if (mDominantTool)
		disconnect(mDominantTool.get(), SIGNAL(tps(int)), this, SIGNAL(tps(int)));
	mDominantTool = newTool;
	connect(mDominantTool.get(), SIGNAL(tps(int)), this, SIGNAL(tps(int)));

	if (mDominantTool->hasType(Tool::TOOL_MANUAL))
		emit tps(0);

//	ssc::messageManager()->sendInfo("Change active tool to: " + mDominantTool->getName());

	emit dominantToolChanged(uid);
}

void ToolManager::setClinicalApplication(ssc::CLINICAL_APPLICATION application)
{
	mApplication = application;
}

std::map<QString, QString> ToolManager::getToolUidsAndNames() const
{
	std::map<QString, QString> uidsAndNames;

	ssc::ToolManager::ToolMap::const_iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
		uidsAndNames[it->second->getUid()] = it->second->getName();

	return uidsAndNames;
}

std::vector<QString> ToolManager::getToolNames() const
{
	std::vector<QString> names;

	ssc::ToolManager::ToolMap::const_iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
		names.push_back(it->second->getName());

	return names;
}

std::vector<QString> ToolManager::getToolUids() const
{
	std::vector<QString> uids;

	ssc::ToolManager::ToolMap::const_iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
		uids.push_back(it->second->getUid());

	return uids;
}

ssc::Transform3DPtr ToolManager::get_rMpr() const
{
	return ssc::Transform3DPtr(new ssc::Transform3D(m_rMpr_History->getCurrentRegistration().mValue));
}

void ToolManager::set_rMpr(const ssc::Transform3DPtr& val)
{
	m_rMpr_History->setRegistration(*val);
}

ssc::ToolPtr ToolManager::getReferenceTool() const
{
	return mReferenceTool;
}

void ToolManager::savePositionHistory()
{
	QString filename = mLoggingFolder + "/toolpositions.snwpos";

	ssc::PositionStorageWriter writer(filename);

	ssc::ToolManager::ToolMap::iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
	{
		ssc::ToolPtr current = it->second;
		ssc::TimedTransformMapPtr data = current->getPositionHistory();

		if (!data)
			continue;

		// save only data acquired after mLastLoadPositionHistory:
		ssc::TimedTransformMap::iterator iter = data->lower_bound(mLastLoadPositionHistory);
		for (; iter != data->end(); ++iter)
			writer.write(iter->second, (iter->first), current->getUid());
	}

	mLastLoadPositionHistory = ssc::getMilliSecondsSinceEpoch();
}

void ToolManager::loadPositionHistory()
{
	QString filename = mLoggingFolder + "/toolpositions.snwpos";

	ssc::PositionStorageReader reader(filename);

	ssc::Transform3D matrix = ssc::Transform3D::Identity();
	double timestamp;
	QString toolUid;

	QStringList missingTools;

	while (!reader.atEnd())
	{
		if (!reader.read(&matrix, &timestamp, &toolUid))
			break;

		ssc::ToolPtr current = this->getTool(toolUid);
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
		ssc::messageManager()->sendWarning(
						QString("Loaded position history.\n"
								"The following tools were found in the history\n"
								"but not in the configuration:\n%1").arg(missingTools.join(", ")));
	}

	mLastLoadPositionHistory = ssc::getMilliSecondsSinceEpoch();
}

void ToolManager::setConfigurationFile(QString configurationFile)
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

void ToolManager::setLoggingFolder(QString loggingFolder)
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

void ToolManager::initializedSlot(bool value)
{
	mInitialized = value;
	if (mInitialized)
	{
		ssc::messageManager()->sendSuccess("ToolManager is initialized.", true);
		emit initialized();
	}
	else
	{
		ssc::messageManager()->sendInfo("ToolManager is uninitialized.");
		emit uninitialized();
	}
}

void ToolManager::trackerTrackingSlot(bool value)
{
	mTracking = value;
	if (mTracking)
	{
		ssc::messageManager()->sendSuccess("ToolManager started tracking.");
		emit trackingStarted();
	}
	else
	{
		ssc::messageManager()->sendSuccess("ToolManager stopped tracking.");
		emit trackingStopped();
	}
}

void ToolManager::startTrackingAfterInitSlot()
{
	disconnect(this, SIGNAL(initialized()), this, SLOT(startTrackingAfterInitSlot()));
	this->startTracking();
}

void ToolManager::initializeAfterConfigSlot()
{
	disconnect(this, SIGNAL(configured()), this, SLOT(initializeAfterConfigSlot()));
	this->initialize();
}

void ToolManager::uninitializeAfterTrackingStoppedSlot()
{
	disconnect(this, SIGNAL(trackingStopped()), this, SLOT(uninitializeAfterTrackingStoppedSlot()));
	this->uninitialize();
}

void ToolManager::deconfigureAfterUninitializedSlot()
{
	disconnect(this, SIGNAL(uninitialized()), this, SLOT(deconfigureAfterUninitializedSlot()));
	this->deconfigure();
}

void ToolManager::configureAfterDeconfigureSlot()
{
	disconnect(this, SIGNAL(deconfigured()), this, SLOT(configureAfterDeconfigureSlot()));
	this->configure();
}

void ToolManager::globalConfigurationFileChangedSlot(QString key)
{
	if (key != "toolConfigFile")
		return;

	this->setConfigurationFile(DataLocations::getToolConfigFilePath());
}

void ToolManager::dominantCheckSlot()
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
			if (it->second->hasType(Tool::TOOL_MANUAL))
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
	std::vector<ssc::ToolPtr> visibleTools;
	ToolMap::iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
	{
		//TODO need to check if init???
		if (it->second->getVisible())
			visibleTools.push_back(it->second);
		else if (it->second->hasType(Tool::TOOL_MANUAL))
			visibleTools.push_back(it->second);
	}

	if (!visibleTools.empty())
	{
		//sort most important tool to the start of the vector:
		sort(visibleTools.begin(), visibleTools.end(), toolTypeSort);
		const QString uid = visibleTools.at(0)->getUid();
//	std::cout << "sorted:" << std::endl;
//    for (int i=0; i<visibleTools.size(); ++i)
//    {
//    	std::cout << "    " << visibleTools[i]->getUid() << std::endl;
//    }
		this->setDominantTool(uid);
	}
}

namespace
{
/**
 * \return a priority of the tool. High means this tool is preferred more.
 */
int getPriority(ssc::ToolPtr tool)
{
	if (tool->hasType(Tool::TOOL_MANUAL)) // place this first, in case a tool has several attributes.
		return 2;

	if (tool->hasType(Tool::TOOL_US_PROBE))
		return 4;
	if (tool->hasType(Tool::TOOL_POINTER))
		return 3;
	if (tool->hasType(Tool::TOOL_REFERENCE))
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
bool toolTypeSort(const ssc::ToolPtr tool1, const ssc::ToolPtr tool2)
{
	return getPriority(tool2) < getPriority(tool1);
}

void ToolManager::addXml(QDomNode& parentNode)
{
	QDomDocument doc = parentNode.ownerDocument();
	QDomElement base = doc.createElement("toolManager");
	parentNode.appendChild(base);
	m_rMpr_History->addXml(base);

	QDomElement manualToolNode = doc.createElement("manualTool");
	manualToolNode.appendChild(doc.createTextNode("\n" + qstring_cast(mManualTool->get_prMt())));
	base.appendChild(manualToolNode);

	QDomElement landmarksNode = doc.createElement("landmarks");
	ssc::LandmarkMap::iterator it = mLandmarks.begin();
	for (; it != mLandmarks.end(); ++it)
	{
		QDomElement landmarkNode = doc.createElement("landmark");
		it->second.addXml(landmarkNode);
		landmarksNode.appendChild(landmarkNode);
	}
	base.appendChild(landmarksNode);

	//Tools
	QDomElement toolsNode = doc.createElement("tools");
	ssc::ToolManager::ToolMapPtr tools = this->getTools();
	ssc::ToolManager::ToolMap::iterator toolIt = tools->begin();
	for (; toolIt != tools->end(); toolIt++)
	{
		QDomElement toolNode = doc.createElement("tool");
		ToolPtr tool = boost::shared_dynamic_cast<Tool>(toolIt->second);
		if (tool)
		{
			tool->addXml(toolNode);
			toolsNode.appendChild(toolNode);
		}
	}
	base.appendChild(toolsNode);
}

void ToolManager::clear()
{
	m_rMpr_History->clear();
	mManualTool->set_prMt(ssc::Transform3D::Identity());
	mLandmarks.clear();
}

void ToolManager::parseXml(QDomNode& dataNode)
{
	if (dataNode.isNull())
		return;

	QDomNode registrationHistory = dataNode.namedItem("registrationHistory");
	m_rMpr_History->parseXml(registrationHistory);

	QString manualToolText = dataNode.namedItem("manualTool").toElement().text();
	mManualTool->set_prMt(ssc::Transform3D::fromString(manualToolText));

	QDomNode landmarksNode = dataNode.namedItem("landmarks");
	QDomElement landmarkNode = landmarksNode.firstChildElement("landmark");
	for (; !landmarkNode.isNull(); landmarkNode = landmarkNode.nextSiblingElement("landmark"))
	{
		ssc::Landmark landmark;
		landmark.parseXml(landmarkNode);
		this->setLandmark(landmark);
	}

	//Tools
	ssc::ToolManager::ToolMapPtr tools = this->getTools();
	QDomNode toolssNode = dataNode.namedItem("tools");
	QDomElement toolNode = toolssNode.firstChildElement("tool");
	for (; !toolNode.isNull(); toolNode = toolNode.nextSiblingElement("tool"))
	{
		QDomElement base = toolNode.toElement();
		QString tool_uid = base.attribute("uid");
		if (tools->find(tool_uid) != tools->end())
		{
			ToolPtr tool = boost::shared_dynamic_cast<Tool>(tools->find(tool_uid)->second);
			tool->parseXml(toolNode);
		}
	}
}

ssc::RegistrationHistoryPtr ToolManager::get_rMpr_History()
{
	return m_rMpr_History;
}

ssc::ManualToolPtr ToolManager::getManualTool()
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
ssc::ToolPtr ToolManager::findFirstProbe()
{
	ssc::ToolPtr active = this->getDominantTool();
	if (active && active->getProbe() && active->getProbe()->isValid())
	{
		return active;
	}

	ssc::ToolManager::ToolMapPtr tools = this->getTools();

	// look for visible probes
	for (ssc::ToolManager::ToolMap::iterator iter = tools->begin(); iter != tools->end(); ++iter)
	{
		if (iter->second->getProbe() && iter->second->getProbe()->isValid() && iter->second->getVisible())
		{
			return iter->second;
		}
	}

	// pick the first probe, visible or not.
	for (ssc::ToolManager::ToolMap::iterator iter = tools->begin(); iter != tools->end(); ++iter)
	{
		if (iter->second->getProbe() && iter->second->getProbe()->isValid())
		{
			return iter->second;
		}
	}

	return ssc::ToolPtr();
}


} //namespace cx
