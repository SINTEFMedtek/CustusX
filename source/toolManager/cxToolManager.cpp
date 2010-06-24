#include "cxToolManager.h"

#include <QTimer>
#include <QDir>
#include <QList>
#include <QMetaType>
#include <QFileInfo>
#include <vtkDoubleArray.h>
#include "sscTypeConversions.h"
#include "sscRegistrationTransform.h"
#include "sscMessageManager.h"
#include "cxDataManager.h"
#include "cxTool.h"
#include "cxTracker.h"
#include "cxToolConfigurationParser.h"

namespace cx
{
// Doxygen ignores code between \cond and \endcond
/// \cond
ToolManager* ToolManager::mCxInstance = NULL;
/// \endcond
ToolManager* toolManager() { return ToolManager::getInstance(); }
ToolManager* ToolManager::getInstance()
{
  if (mCxInstance == NULL)
  {
    mCxInstance = new ToolManager();
    ssc::ToolManager::setInstance(mCxInstance);
  }
  return mCxInstance;
}

ToolManager::ToolManager() :
      mConfigurationFilePath(""),
      mLoggingFolder(""),
      mTracker(TrackerPtr()),
      mConfiguredTools(new ssc::ToolManager::ToolMap), //TODO Why was this commented out???
      mConnectedTools(new ssc::ToolManager::ToolMap), mDominantTool(
          ssc::ToolPtr()), mReferenceTool(ssc::ToolPtr()), mConfigured(false),
      mInitialized(false), mTracking(false), mPulseGenerator(
          igstk::PulseGenerator::New())
{
  //this->createSymlink(); // test

  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(checkTimeoutsAndRequestTransform()));

  m_rMpr_History.reset(new ssc::RegistrationHistory());
  connect(m_rMpr_History.get(), SIGNAL(currentChanged()), this, SIGNAL(rMprChanged()));

  igstk::RealTimeClock::Initialize();

  mPulseGenerator->RequestSetFrequency(30.0);
  mPulseGenerator->RequestStart();

  this->initializeManualTool();
  this->setDominantTool("Manual Tool");
}

ToolManager::~ToolManager()
{
  this->cleanupSymlink();
}

void ToolManager::initializeManualTool()
{
  if (!mManualTool)
  {
    //adding a manual tool as default
    mManualTool.reset(new ssc::ManualTool("Manual Tool"));
    (*mConfiguredTools)["Manual Tool"] = mManualTool;
    mManualTool->setVisible(true);
    this->addConnectedTool("Manual Tool");
  }

  ssc::Transform3D prMt =
      this->get_rMpr()->inv() *
      ssc::createTransformRotateY(M_PI) *
      ssc::createTransformRotateZ(M_PI_2);
  mManualTool->set_prMt(prMt);
}

void ToolManager::configureReferences()
{
  ToolMapConstIter iter = mConfiguredTools->begin();
  while (iter != mConfiguredTools->end())
  {
    ssc::ToolPtr tool = (*iter).second;
    if (tool->getType() == ssc::Tool::TOOL_REFERENCE)
    {
      mReferenceTool = tool;
    }
    iter++;
  }
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
  if(mConfigurationFilePath.empty() || !QFile::exists(QString(mConfigurationFilePath.c_str())))
  {
    ssc::messageManager()->sendWarning("Configuration file is not valid, could not configure the toolmanager.");
    return;
  }

  ToolConfigurationParser toolConfigurationParser(mConfigurationFilePath, mLoggingFolder);
  mTracker = toolConfigurationParser.getTracker();
  if(mTracker->getType() == Tracker::TRACKER_NONE)
  {
    ssc::messageManager()->sendError("Could not configure the toolmanager, tracker is invalid.");
    return;
  }
  mConfiguredTools = toolConfigurationParser.getConfiguredTools();

  this->configureReferences();

  this->setDominantTool(this->getManualTool()->getUid());

  this->connectSignalsAndSlots();

  mConfigured = true;
  ssc::messageManager()->sendInfo("ToolManager is configured.");
  emit configured();
}
void ToolManager::initialize()
{
  if (!this->isConfigured())
    this->configure();

  if (!mConfigured)
  {
    ssc::messageManager()->sendWarning("Please configure before trying to initialize.");
    return;
  }
  this->createSymlink();
  mTracker->open();
  mTracker->attachTools(mConfiguredTools);
}

/** Assume that IGSTK requires the file /Library/CustusX/igstk.links/cu.CustusX.dev0
 *  as a rep for the HW connection. Also assume that directory is created with full
 *  read/write access (by installer or similar).
 *  Create that file as a symlink to the correct device.
 */
void ToolManager::createSymlink()
{
  QFileInfo symlink = this->getSymlink();
  QDir linkDir(symlink.absolutePath());
  QString linkfile = symlink.absoluteFilePath();;

  if (!linkDir.exists())
  {
    ssc::messageManager()->sendError(string_cast(QString("Folder %1 does not exist. System is not properly installed.").arg(linkDir.path())));
    return;
  }

  QDir devDir("/dev/");

  QStringList filters;
  //filters << "*cu.*"; // test
  filters << "cu.usbserial*" << "cu.KeySerial*"; //NOTE: only works with current hardware using aurora or polaris.
  QStringList files = devDir.entryList(filters, QDir::System);

  if (files.empty())
  {
    ssc::messageManager()->sendError(string_cast(QString("Warning: No usb connections found in /dev using filters %1").arg(filters.join(";"))));
    return;
  }
  else
  {
    ssc::messageManager()->sendInfo(string_cast(QString("device files: %1").arg(files.join(","))));
  }

  QString device = devDir.filePath(files[0]);

  QFile(linkfile).remove();
  QFile devFile(device);
  // this call only succeeds if Custus is run as root.
  bool val = devFile.link(linkfile);
  if (!val)
  {
    ssc::messageManager()->sendError(string_cast(QString("symlink %1 creation to device %2 failed with code %3").arg(linkfile).arg(device).arg(devFile.error())));
  }
  else
  {
    ssc::messageManager()->sendInfo(string_cast(QString("created symlink %1 to device %2").arg(linkfile).arg(device)));
  }
}

QFileInfo ToolManager::getSymlink() const
{
  QDir linkDir("/Library/CustusX/igstk.links");
  QString linkFile = linkDir.path() + "/cu.CustusX.dev0";
  return QFileInfo(linkDir, linkFile);
}

/** removes symlinks to tracking system created during setup
 *
 */
void ToolManager::cleanupSymlink()
{
  std::cout << "ToolManager::cleanupSymlink()" << std::endl;
  std::cout << QFile(this->getSymlink().absoluteFilePath()).remove() << std::endl;
}

void ToolManager::startTracking()
{
	if (!this->isInitialized())
		this->initialize();

  if (!mInitialized)
  {
    ssc::messageManager()->sendWarning("Please initialize before trying to start tracking.");
    return;
  }
  mTracker->startTracking();
}
void ToolManager::stopTracking()
{
  if (!mTracking)
  {
    ssc::messageManager()->sendWarning("Please start tracking before trying to stop tracking.");
    return;
  }
  mTracker->stopTracking();
}
void ToolManager::saveToolsSlot()
{
  this->saveTransformsAndTimestamps();
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

void ToolManager::removeLandmark(std::string uid)
{
  mLandmarks.erase(uid);
  emit landmarkRemoved(uid);
}

TrackerPtr ToolManager::getTracker()
{
  return mTracker;
}

ssc::ToolManager::ToolMapPtr ToolManager::getConfiguredTools()
{
  return mConfiguredTools;
}
ssc::ToolManager::ToolMapPtr ToolManager::getTools()
{
  return mConnectedTools;
}
ssc::ToolPtr ToolManager::getTool(const std::string& uid)
{
  ToolMapConstIter it = mConnectedTools->find(uid);
  if (it != mConnectedTools->end())
  {
    return ((*it).second);
  } else
  {
    return boost::shared_ptr<ssc::Tool>();
  }
}
ssc::ToolPtr ToolManager::getDominantTool()
{
  return mDominantTool;
}

void ToolManager::setDominantTool(const std::string& uid)
{
  //std::cout << "1: void ToolManager::setDominantTool( "+uid+" )" << std::endl;
  if(mDominantTool && mDominantTool->getUid() == uid)
    return;

  if (mDominantTool)
  {
    // make manual tool invisible when other tools are active.
    if (mDominantTool->getType()==ssc::Tool::TOOL_MANUAL)
    {
      mManualTool->setVisible(false);
    }
  }

  //std::cout << "void ToolManager::setDominantTool( "+uid+" )" << std::endl;

  ssc::ToolPtr newTool;

  ToolMapConstIter iter = mConfiguredTools->find(uid);
  if (iter != mConfiguredTools->end())
  {
    newTool = iter->second;
  }
  ToolMapConstIter it = mConnectedTools->find(uid);
  if (it != mConnectedTools->end())
  {
    newTool = it->second;
  }

  // special case for manual tool
  if(newTool && newTool->getType() == ssc::Tool::TOOL_MANUAL && mManualTool)
  {
    if (mDominantTool)
    {
      mManualTool->set_prMt(mDominantTool->get_prMt());
      mManualTool->setTooltipOffset(mDominantTool->getTooltipOffset());
    }
    mManualTool->setVisible(true);
  }

  mDominantTool = newTool;
  emit dominantToolChanged(uid);
}

std::map<std::string, std::string> ToolManager::getToolUidsAndNames() const
{
  std::map<std::string, std::string> uidsAndNames;

  ToolMapConstIter it = mConnectedTools->begin();
  while (it != mConnectedTools->end())
  {
    uidsAndNames[((*it).second)->getUid()] = ((*it).second)->getName();
    it++;
  }
  ToolMapConstIter iter = mConfiguredTools->begin();
  while (iter != mConfiguredTools->end())
  {
    uidsAndNames[((*iter).second)->getUid()] = ((*iter).second)->getName();
    iter++;
  }
  return uidsAndNames;
}
std::vector<std::string> ToolManager::getToolNames() const
{
  std::vector<std::string> names;
  ToolMapConstIter it = mConnectedTools->begin();
  while (it != mConnectedTools->end())
  {
    names.push_back(((*it).second)->getName());
    it++;
  }
  ToolMapConstIter iter = mConfiguredTools->begin();
  while (iter != mConfiguredTools->end())
  {
    names.push_back(((*iter).second)->getName());
    iter++;
  }
  return names;
}
std::vector<std::string> ToolManager::getToolUids() const
{
  std::vector<std::string> uids;
  ToolMapConstIter it = mConnectedTools->begin();
  while (it != mConnectedTools->end())
  {
    uids.push_back(((*it).second)->getUid());
    it++;
  }
  ToolMapConstIter iter = mConfiguredTools->begin();
  while (iter != mConfiguredTools->end())
  {
    uids.push_back(((*iter).second)->getUid());
    iter++;
  }
  return uids;
}
ssc::Transform3DPtr ToolManager::get_rMpr() const
{
  return ssc::Transform3DPtr(new ssc::Transform3D(
      m_rMpr_History->getCurrentRegistration()));
}
void ToolManager::set_rMpr(const ssc::Transform3DPtr& val)
{
  m_rMpr_History->setRegistration(*val);
}
ssc::ToolPtr ToolManager::getReferenceTool() const
{
  return mReferenceTool;
}
void ToolManager::saveTransformsAndTimestamps(std::string filePathAndName)
{
  ToolMapConstIter it = mConnectedTools->begin();
  while (it != mConnectedTools->end())
  {
    //((*it).second)->setTransformSaveFile(filePathAndName); is set during tools constructor
    ((*it).second)->saveTransformsAndTimestamps();
    it++;
  }
}
void ToolManager::setConfigurationFile(std::string configurationFile)
{
  mConfigurationFilePath = configurationFile;
}
void ToolManager::setLoggingFolder(std::string loggingFolder)
{
  mLoggingFolder = loggingFolder;
}

/**
 * Slot that receives reports from tools
 * \param message What happended to the tool
 * \param state   Whether the tool was trying to enter or leave a state
 * \param success Whether or not the request was a success
 * \param uid     The tools unique id
 */
void ToolManager::receiveToolReport(ToolMessage message, bool state, bool success, stdString uid)
{
  std::string toolUid = "" + uid;
  std::string report = "";

  switch (message)
  {
  case Tool::TOOL_INVALID_REQUEST:
    report.append(toolUid + " performed an invalid request.");
    break;
  case Tool::TOOL_HW_CONFIGURED:
    report.append(toolUid + " is ");
    if (!success)
      report.append("not ");
    report.append(" hardware configured successfully.");
    break;
  case Tool::TOOL_ATTACHED_TO_TRACKER:
    report.append(toolUid + " was ");
    if (!success)
      report.append("not ");
    if (!state)
      report.append("detached from ");
    else
      report.append("attached to ");
    report.append("the tracker.");
    if (success && state)
      this->addConnectedTool(uid);
    break;
  case Tool::TOOL_VISIBLE:
    report.append(toolUid + " is ");
    if (!state)
      report.append("not ");
    report.append("visible.");
    break;
  case Tool::TOOL_TRACKED:
    report.append(toolUid + " ");
    if (state)
      report.append("started tracking ");
    else
      report.append("stopped tracking ");
    if (!success)
      report.append("not ");
    report.append("successfully.");
    break;
  case Tool::TOOL_COORDINATESYSTEM_TRANSFORM:
    report.append(toolUid + " got a new transform and timestamp.");
    return; //this is spamming the screen....
    break;
  case Tool::TOOL_NDI_PORT_NUMBER:
    report.append(toolUid + " could not used the given port number.");
    break;
  case Tool::TOOL_NDI_SROM_FILENAME:
    report.append(toolUid + " could not used the given SROM file.");
    break;
  case Tool::TOOL_NDI_PART_NUMBER:
    report.append(toolUid + " did not have a valid part number.");
    break;
  case Tool::TOOL_AURORA_CHANNEL_NUMBER:
    report.append(toolUid + " could not use the given Aurora channel number.");
    break;
  default:
    report.append(toolUid + " reported an unknown message.");
    break;
  }
  ssc::messageManager()->sendInfo(report);
}

/**
 * Slot that receives reports from trackers
 * \param message What happended to the tool
 * \param state   Whether the tool was trying to enter or leave a state
 * \param success Whether or not the request was a success
 * \param uid     The trackers unique id
 */
void ToolManager::receiveTrackerReport(Tracker::Message message, bool state, bool success, std::string uid)
{
  std::string trackerUid = uid;
  std::string report = "";

  switch (message)
  {
  case Tracker::TRACKER_INVALID_REQUEST:
    report.append(trackerUid + " performed an invalid request.");
    break;
  case Tracker::TRACKER_OPEN:
    report.append(trackerUid + " is ");
    if (!success)
      report.append("not ");
    if (state)
    {
      if(success){ //Should this really be done here?
        mInitialized = success;
        emit initialized();
        ssc::messageManager()->sendInfo("ToolManager is initialized. (TRACKER_OPEN)");
      }
      report.append("open.");
    } else
    {
      mInitialized = !success;
      report.append("closed.");
    }
    break;
  case Tracker::TRACKER_INITIALIZED:
    report.append(trackerUid + " is ");
    if (!success)
    {
      report.append("not ");
      mInitialized = false;
    } else
    {
      mInitialized = true;
      emit initialized();
      ssc::messageManager()->sendInfo("ToolManager is initialized.(TRACKER_INITIALIZED");
    }
    report.append("initialized.");
    break;
  case Tracker::TRACKER_TRACKING:
    report.append(trackerUid + " could ");
    if (!success)
      report.append("not ");
    if (state)
      report.append("start tracking.");
    else
      report.append("stop tracking.");
    if (success && state)
    {
      mTracking = true;
      emit trackingStarted();
      mTimer->start(33);
    }
    if (success && !state)
    {
      mTracking = false;
      emit trackingStopped();
      mTimer->stop();
    }
    break;
  case Tracker::TRACKER_UPDATE_STATUS:
    report.append(trackerUid + " has a tool that is ");
    if (!success)
      report.append("not ");
    else
      return;//this is spamming the screen....
    report.append("updated successfully.");
    break;
  case Tracker::TRACKER_TOOL_TRANSFORM_UPDATED:
    report.append(trackerUid + " has a tool that is ");
    if (!success)
      report.append("not ");
    else
      return; //this is spamming the screen....
    report.append("updated successfully.");
    break;
  case Tracker::TRACKER_COMMUNICATION_COMPLETE:
    return; //this is spamming the screen....
    report.append("Communication did something ");
    if (!success)
      report.append("not ");
    report.append("successfully.");
    break;
  case Tracker::TRACKER_COMMUNICATION_INPUT_OUTPUT_ERROR:
    report.append(trackerUid + ": TRACKER_COMMUNICATION_INPUT_OUTPUT_ERROR");
    break;
  case Tracker::TRACKER_COMMUNICATION_INPUT_OUTPUT_TIMEOUT:
    report.append(trackerUid + ": TRACKER_COMMUNICATION_INPUT_OUTPUT_TIMEOUT");
    break;
  case Tracker::TRACKER_COMMUNICATION_OPEN_PORT_ERROR:
    report.append(trackerUid + ": TRACKER_COMMUNICATION_OPEN_PORT_ERROR");
    break;
  default:
    report.append(trackerUid + " reported an unknown message.");
    break;
  }
  ssc::messageManager()->sendInfo(report);
}

void ToolManager::addConnectedTool(std::string uid)
{  
  ssc::ToolManager::ToolMap::iterator it = mConfiguredTools->find(uid);  
  if (it == mConfiguredTools->end() || !it->second)
  {
    ssc::messageManager()->sendInfo("Tool with id " + uid
        + " was not found to be configured "
          ", thus could not add is as a connected tool.");
    return;
  }
  (*mConnectedTools)[it->first] = it->second;
  ssc::ToolPtr tool = it->second;

  //connect visible/hidden signal to domiantCheck
  connect(tool.get(), SIGNAL(toolVisible(bool)), this, SLOT(dominantCheckSlot()));
  
  mConfiguredTools->erase(it);
  ssc::messageManager()->sendInfo("Tool with id " + uid
      + " was moved from the configured to the connected map.");
}
void ToolManager::connectSignalsAndSlots()
{
  typedef Tracker::Message TrackerMessage;
  qRegisterMetaType<TrackerMessage> ("TrackerMessage");
  typedef Tool::Message ToolMessage;
  qRegisterMetaType<ToolMessage> ("ToolMessage");
  typedef std::string stdString;
  qRegisterMetaType<stdString> ("stdString");

  connect(mTracker.get(), SIGNAL(trackerReport(TrackerMessage, bool, bool, stdString)),
      this, SLOT(receiveTrackerReport(TrackerMessage, bool, bool, stdString)));

  ssc::ToolManager::ToolMap::iterator it = mConfiguredTools->begin();
  while (it != mConfiguredTools->end())
  {
    connect(((*it).second).get(), SIGNAL(toolReport(ToolMessage, bool, bool, stdString)),
        this, SLOT(receiveToolReport(ToolMessage, bool, bool, stdString)));
    it++;
  }
  //ssc::messageManager()->sendInfo("Signals and slots have been connected.");
}
void ToolManager::checkTimeoutsAndRequestTransform()
{
  mPulseGenerator->CheckTimeouts();

  if (!mReferenceTool) // no need to request extra transforms from tools to the tracker, its already done
    return;

  ToolPtr refTool = boost::shared_dynamic_cast<Tool>(mReferenceTool);
  ToolMap::iterator it = mConnectedTools->begin();
  for(;it != mConnectedTools->end();++it)
  {
    ToolPtr connectedTool = boost::shared_dynamic_cast<Tool>(it->second);
    if(!refTool || !connectedTool)
      continue;
    connectedTool->getPointer()->RequestComputeTransformTo(refTool->getPointer());
  }
}
void ToolManager::dominantCheckSlot()
{
  //std::cout << "void ToolManager::dominantCheckSlot()" << std::endl;

  //make a sorted vector of all visible tools
  std::vector<ssc::ToolPtr> visibleTools;
  for(ToolMap::iterator it = mConnectedTools->begin();
      it != mConnectedTools->end(); ++it)
  {
    if(it->second->getVisible())
      visibleTools.push_back(it->second);
    else if(it->second->getType() == ssc::Tool::TOOL_MANUAL)
      visibleTools.push_back(it->second);
  }

  if(!visibleTools.empty())
  {
    //sort most important tool to the start of the vector:
    sort(visibleTools.begin(), visibleTools.end(), toolTypeSort);
    const std::string uid = visibleTools.at(0)->getUid();
    this->setDominantTool(uid);
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
  return tool1->getType() > tool2->getType();
}

void ToolManager::addXml(QDomNode& parentNode)
{
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement base = doc.createElement("toolManager");
  parentNode.appendChild(base);
  m_rMpr_History->addXml(base);

  QDomElement manualToolNode = doc.createElement("manualTool");
  manualToolNode.appendChild(doc.createTextNode("\n"+qstring_cast(mManualTool->get_prMt())));
  base.appendChild(manualToolNode);

//  if (mDominantTool)
//  {
//    QDomElement toolOffsetNode = doc.createElement("toolOffset");
//    toolOffsetNode.appendChild(doc.createTextNode("\n"+qstring_cast(mDominantTool->getTooltipOffset())));
//    base.appendChild(toolOffsetNode);
//  }

  QDomElement landmarksNode = doc.createElement("landmarks");
  ssc::LandmarkMap::iterator it = mLandmarks.begin();
  for(; it != mLandmarks.end(); ++it)
  {
    QDomElement landmarkNode = doc.createElement("landmark");
    it->second.addXml(landmarkNode);
    landmarksNode.appendChild(landmarkNode);
  }
  base.appendChild(landmarksNode);
}

void ToolManager::parseXml(QDomNode& dataNode)
{
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
}

ssc::RegistrationHistoryPtr ToolManager::get_rMpr_History()
{
  return m_rMpr_History;
}

ssc::ManualToolPtr ToolManager::getManualTool()
{
  return mManualTool;
}

}//namespace cx
