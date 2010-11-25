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
#include "sscDataManager.h"
#include "cxTool.h"
#include "cxTracker.h"
#include "cxToolConfigurationParser.h"
#include "sscTypeConversions.h"

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

ToolManager::ToolManager() :
      mConfigurationFilePath(""),
      mLoggingFolder(""),
      mTracker(TrackerPtr()),
      mConfiguredTools(new ssc::ToolManager::ToolMap),
      mInitializedTools(new ssc::ToolManager::ToolMap), mDominantTool(
          ssc::ToolPtr()), mReferenceTool(ssc::ToolPtr()), mConfigured(false),
      mInitialized(false), mTracking(false), mPulseGenerator(
          igstk::PulseGenerator::New())
{
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
{}

void ToolManager::runDummyTool(ssc::DummyToolPtr tool)
{
  ssc::messageManager()->sendInfo("Running dummy tool "+tool->getUid());

  (*mConfiguredTools)[tool->getUid()] = tool;
  tool->setVisible(true);
  this->addInitializedTool(tool->getUid());
  tool->startTracking();
  this->setDominantTool(tool->getUid());

  emit initialized();
}

void ToolManager::initializeManualTool()
{
  if (!mManualTool)
  {
    //adding a manual tool as default
    mManualTool.reset(new ssc::ManualTool("Manual Tool"));
    (*mConfiguredTools)["Manual Tool"] = mManualTool;
    mManualTool->setVisible(true);
    this->addInitializedTool("Manual Tool");
  }

  ssc::Transform3D prMt =
      this->get_rMpr()->inv() *
      ssc::createTransformRotateY(M_PI) *
      ssc::createTransformRotateZ(M_PI_2);
  mManualTool->set_prMt(prMt);
}

void ToolManager::configureReferences()
{
  ToolMapIter iter = mConfiguredTools->begin();
  while (iter != mConfiguredTools->end())
  {
    ssc::ToolPtr tool = (*iter).second;
    if (tool->getType() == ssc::Tool::TOOL_REFERENCE)
    {
      mReferenceTool = tool;
      ssc::messageManager()->sendInfo("Reference set to be "+mReferenceTool->getName());
      return;
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
  // << "Inside configure" << std::endl;
  if(mConfigurationFilePath.isEmpty() || !QFile::exists(mConfigurationFilePath))
  {
    ssc::messageManager()->sendWarning("Configuration file is not valid, could not configure the toolmanager.");
    return;
  }

  ToolConfigurationParser toolConfigurationParser(mConfigurationFilePath, mLoggingFolder);
  mTracker = toolConfigurationParser.getTracker();
  if(!mTracker->isValid())
  {
    ssc::messageManager()->sendWarning("Could not configure the toolmanager, tracker is invalid.");
    return;
  }else
  {
    connect(mTracker.get(), SIGNAL(open(bool)), this, SLOT(trackerOpenSlot(bool)));
    connect(mTracker.get(), SIGNAL(initialized(bool)), this, SLOT(trackerInitializedSlot(bool)));
    connect(mTracker.get(), SIGNAL(tracking(bool)), this, SLOT(trackerTrackingSlot(bool)));
  }
  mConfiguredTools = toolConfigurationParser.getConfiguredTools();
  //std::cout << "Found " << mConfiguredTools->size() << " configured tools." << std::endl;

  ssc::ToolManager::ToolMap::iterator it = mConfiguredTools->begin();
  while(it != mConfiguredTools->end())
  {
    //std::cout << "Tool: " << it->first << std::endl;
    connect(((*it).second).get(), SIGNAL(attachedToTracker(bool)), this, SLOT(toolInitialized(bool)));
    it++;
  }

  this->configureReferences();

  this->setDominantTool(this->getManualTool()->getUid());

  mConfigured = true;
  ssc::messageManager()->sendSuccess("ToolManager is configured.");
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

void ToolManager::uninitialize()
{
  if(!this->isInitialized())
  {
    ssc::messageManager()->sendInfo("No need to uninitialize, toolmanager is not initialized.");
    return;
  }
  mTracker->detachTools(mConfiguredTools); //not sure we have to detach all tools before we close, read NDI manual
  mTracker->close();
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
    ssc::messageManager()->sendError(QString("Folder %1 does not exist. System is not properly installed.").arg(linkDir.path()));
    return;
  }

  QDir devDir("/dev/");

  QStringList filters;
  // cu* applies to Mac, ttyUSB applies to Linux
  filters << "cu.usbserial*" << "cu.KeySerial*" << "serial" << "ttyUSB*" ; //NOTE: only works with current hardware using aurora or polaris.
  //filters << "cu.usbserial*" << "cu.USA19H*"; //NOTE: only works with current hardware using aurora or polaris.
  QStringList files = devDir.entryList(filters, QDir::System);

  if (files.empty())
  {
    ssc::messageManager()->sendError(QString("No usb connections found in /dev using filters %1").arg(filters.join(";")));
    return;
  }
  else
  {
    ssc::messageManager()->sendInfo(QString("Device files: %1").arg(files.join(",")));
  }

  QString device = devDir.filePath(files[0]);

  QFile(linkfile).remove();
  QFile devFile(device);
  // this call only succeeds if Custus is run as root.
  bool val = devFile.link(linkfile);
  if (!val)
  {
    ssc::messageManager()->sendError(QString("Symlink %1 creation to device %2 failed with code %3").arg(linkfile).arg(device).arg(devFile.error()));
  }
  else
  {
    ssc::messageManager()->sendInfo(QString("Created symlink %1 to device %2").arg(linkfile).arg(device));
  }

  devFile.setPermissions(
		   QFile::ReadOwner|QFile::WriteOwner|QFile::ExeOwner|
		   QFile::ReadGroup|QFile::WriteGroup|QFile::ExeGroup|
		   QFile::ReadOther|QFile::WriteOther|QFile::ExeOther);
}

QFileInfo ToolManager::getSymlink() const
{
  QString name("/Library/CustusX/igstk.links");
//  QDir linkDir("/Library/CustusX/igstk.links");
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
  ssc::messageManager()->sendDebug("Added landmark: "+landmark.getUid()+" with vector "+qstring_cast(landmark.getCoord()));
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
  for(;it != landmarks.end(); ++it)
  {
    ssc::toolManager()->removeLandmark(it->first);
  }
}

TrackerPtr ToolManager::getTracker()
{
  return mTracker;
}

ssc::ToolManager::ToolMapPtr ToolManager::getConfiguredTools()
{
  return mConfiguredTools;
}

ssc::ToolManager::ToolMapPtr ToolManager::getInitializedTools()
{
  return mInitializedTools;
}

ssc::ToolManager::ToolMapPtr ToolManager::getTools()
{
  ssc::ToolManager::ToolMapPtr allTools(new ToolMap);
  allTools->insert(mConfiguredTools->begin(), mConfiguredTools->end());
  allTools->insert(mInitializedTools->begin(), mInitializedTools->end());

  return allTools;
}

ssc::ToolPtr ToolManager::getTool(const QString& uid)
{
  ToolMapIter itInit = mInitializedTools->find(uid);
  ToolMapIter itConf = mConfiguredTools->find(uid);
  if (itInit != mInitializedTools->end())
  {
    return ((*itInit).second);
  } else if (itConf != mConfiguredTools->end())
  {
    return ((*itConf).second);
  }
  else
  {
    return boost::shared_ptr<ssc::Tool>();
  }
}

ssc::ToolPtr ToolManager::getDominantTool()
{
  return mDominantTool;
}

void ToolManager::setDominantTool(const QString& uid)
{
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

  ssc::ToolPtr newTool;

  ToolMapIter iter = mConfiguredTools->find(uid);
  if (iter != mConfiguredTools->end())
  {
    newTool = iter->second;
  }
  ToolMapIter it = mInitializedTools->find(uid);
  if (it != mInitializedTools->end())
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

std::map<QString, QString> ToolManager::getToolUidsAndNames() const
{
  std::map<QString, QString> uidsAndNames;

  ToolMapIter it = mInitializedTools->begin();
  while (it != mInitializedTools->end())
  {
    uidsAndNames[((*it).second)->getUid()] = ((*it).second)->getName();
    it++;
  }
  ToolMapIter iter = mConfiguredTools->begin();
  while (iter != mConfiguredTools->end())
  {
    uidsAndNames[((*iter).second)->getUid()] = ((*iter).second)->getName();
    iter++;
  }
  return uidsAndNames;
}

std::vector<QString> ToolManager::getToolNames() const
{
  std::vector<QString> names;
  ToolMapIter it = mInitializedTools->begin();
  while (it != mInitializedTools->end())
  {
    names.push_back(((*it).second)->getName());
    it++;
  }
  ToolMapIter iter = mConfiguredTools->begin();
  while (iter != mConfiguredTools->end())
  {
    names.push_back(((*iter).second)->getName());
    iter++;
  }
  return names;
}

std::vector<QString> ToolManager::getToolUids() const
{
  std::vector<QString> uids;
  ToolMapIter it = mInitializedTools->begin();
  while (it != mInitializedTools->end())
  {
    uids.push_back(((*it).second)->getUid());
    it++;
  }
  ToolMapIter iter = mConfiguredTools->begin();
  while (iter != mConfiguredTools->end())
  {
    uids.push_back(((*iter).second)->getUid());
    iter++;
  }
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

void ToolManager::saveTransformsAndTimestamps(QString filePathAndName)
{
  ToolMapIter it = mInitializedTools->begin();
  while (it != mInitializedTools->end())
  {
    ((*it).second)->saveTransformsAndTimestamps();
    it++;
  }
}

void ToolManager::setConfigurationFile(QString configurationFile)
{
  if(this->isConfigured())
  {
    ssc::messageManager()->sendWarning("You already configured, to reconfigure you have to restart CustusX3.");
    return;
  }
  mConfigurationFilePath = configurationFile;
}

void ToolManager::setLoggingFolder(QString loggingFolder)
{
  mLoggingFolder = loggingFolder;
}

void ToolManager::addInitializedTool(QString uid)
{  
  ssc::ToolManager::ToolMap::iterator it = mConfiguredTools->find(uid);  
  if (it == mConfiguredTools->end() || !it->second)
  {
    ssc::messageManager()->sendWarning("Tool with id " + uid
        + " was not found to be configured "
          ", thus could not add is as a connected tool.");
    return;
  }
  (*mInitializedTools)[it->first] = it->second;
  ssc::ToolPtr tool = it->second;

  //connect visible/hidden signal to domiantCheck
  connect(tool.get(), SIGNAL(toolVisible(bool)), this, SLOT(dominantCheckSlot()));
  
  mConfiguredTools->erase(it);
}

void ToolManager::checkTimeoutsAndRequestTransform()
{
  mPulseGenerator->CheckTimeouts();

  if (!mReferenceTool) // no need to request extra transforms from tools to the tracker, its already done
    return;

  ToolPtr refTool = boost::shared_dynamic_cast<Tool>(mReferenceTool);
  ToolMap::iterator it = mInitializedTools->begin();
  for(;it != mInitializedTools->end();++it)
  {
    ToolPtr connectedTool = boost::shared_dynamic_cast<Tool>(it->second);
    if(!refTool || !connectedTool)
      continue;
    connectedTool->getPointer()->RequestComputeTransformTo(refTool->getPointer());
  }
}

void ToolManager::trackerOpenSlot(bool)
{
  //TODO
  //ToolManager does not care about this at the moment,
  //but maybe it should?
}

void ToolManager::trackerInitializedSlot(bool value)
{
  mInitialized = value;
  if(mInitialized)
  {
    ssc::messageManager()->sendSuccess("ToolManager is initialized.");
    emit initialized();
  }
  else
  {
    ssc::messageManager()->sendInfo("ToolManager is uninitialized.");
    emit uninitialize();
  }
}

void ToolManager::trackerTrackingSlot(bool value)
{
  mTracking = value;
  if(mTracking)
  {
    ssc::messageManager()->sendSuccess("ToolManager started tracking.");
    mTimer->start(33);
    emit trackingStarted();
  }
  else
  {
    ssc::messageManager()->sendSuccess("ToolManager stopped tracking.");
    mTimer->stop();
    emit trackingStopped();
  }

}

void ToolManager::toolInitialized(bool value)
{
  Tool* tool = static_cast<Tool*>(this->sender());
  if(tool)
    this->addInitializedTool(tool->getUid());
  else
    ssc::messageManager()->sendWarning("Casting to tool failed... Contact programmer.");
}

void ToolManager::dominantCheckSlot()
{
  //make a sorted vector of all visible tools
  std::vector<ssc::ToolPtr> visibleTools;
  for(ToolMap::iterator it = mInitializedTools->begin();
      it != mInitializedTools->end(); ++it)
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
    const QString uid = visibleTools.at(0)->getUid();
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

  QDomElement landmarksNode = doc.createElement("landmarks");
  ssc::LandmarkMap::iterator it = mLandmarks.begin();
  for(; it != mLandmarks.end(); ++it)
  {
    QDomElement landmarkNode = doc.createElement("landmark");
    it->second.addXml(landmarkNode);
    landmarksNode.appendChild(landmarkNode);
  }
  base.appendChild(landmarksNode);

  //Tools
  QDomElement toolsNode = doc.createElement("tools");
  ssc::ToolManager::ToolMapPtr tools = getTools();
  ssc::ToolManager::ToolMap::iterator toolIt = tools->begin();
  for(; toolIt != tools->end(); toolIt++)
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
  mManualTool->set_prMt(ssc::Transform3D());
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
  ssc::ToolManager::ToolMapPtr tools = getTools();
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

void ToolManager::setUSProbeSector(ssc::ProbeSector probeSector)
{
  ToolPtr tool = boost::shared_dynamic_cast<Tool>(mDominantTool);
  if (tool)
  {
    tool->setUSProbeSector(probeSector);
  }
}

}//namespace cx
