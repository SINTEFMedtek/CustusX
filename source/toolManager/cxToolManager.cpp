#include "cxToolManager.h"

#include <QTimer>
#include <QDir>
#include <QList>
#include <QDomDocument>
#include <QMetaType>
#include <vtkDoubleArray.h>
#include "sscTypeConversions.h"
#include "sscRegistrationTransform.h"
#include "cxDataManager.h"
#include "cxTool.h"
#include "cxTracker.h"
#include "cxMessageManager.h"

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
      mInitialized(false), mTracking(false), mTrackerTag("tracker"),
      mTrackerTypeTag("type"), mToolfileTag("toolfile"), mToolTag("tool"),
      mToolTypeTag("type"), mToolIdTag("id"), mToolNameTag("name"),
      mToolGeoFileTag("geo_file"), mToolSensorTag("sensor"),
      mToolSensorTypeTag("type"), mToolSensorWirelessTag("wireless"),
      mToolSensorDOFTag("DOF"), mToolSensorPortnumberTag("portnumber"),
      mToolSensorChannelnumberTag("channelnumber"), mToolSensorRomFileTag(
          "rom_file"), mToolCalibrationTag("calibration"),
      mToolCalibrationFileTag("cal_file"), mPulseGenerator(
          igstk::PulseGenerator::New())
{
  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(
      checkTimeoutsAndRequestTransform()));

  m_rMpr_History.reset(new ssc::RegistrationHistory());
  connect(m_rMpr_History.get(), SIGNAL(currentChanged()), this, SIGNAL(
      rMprChanged()));

  igstk::RealTimeClock::Initialize();

  mPulseGenerator->RequestSetFrequency(30.0);
  mPulseGenerator->RequestStart();

  initializeManualTool();
  this->setDominantTool("tool_manual");
}
ToolManager::~ToolManager()
{
}

void ToolManager::initializeManualTool()
{
  if (!mManualTool)
  {
    //adding a manual tool as default
    mManualTool.reset(new ssc::ManualTool("tool_manual"));
    //mManualTool->setName("Mouse tool");
    (*mConfiguredTools)["tool_manual"] = mManualTool;
    mManualTool->setVisible(true);
    this->addConnectedTool("tool_manual");
  }

  ssc::Transform3D prMt =
      this->get_rMpr()->inv() *
      ssc::createTransformRotateY(M_PI) *
      ssc::createTransformRotateZ(M_PI_2);
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
  if (!this->pathsExists())
  {
    return;
  }

  QDomNodeList trackerNode;
  QList<QDomNodeList> toolNodeList;
  if (!this->readConfigurationFile(trackerNode, toolNodeList))
    return;

  mTracker = this->configureTracker(trackerNode);
  mConfiguredTools = this->configureTools(toolNodeList);

//  if (!mConfiguredTools->empty())
//  {
//    ToolMapConstIter it = mConfiguredTools->begin();
//    while (it != mConfiguredTools->end())
//    {
//      ssc::ToolPtr tool = (*it).second;
//      if (tool->getType() != ssc::Tool::TOOL_REFERENCE)
//      {
//        //use manual tools
//        ssc::Transform3D transform = this->getManualTool()->get_prMt();
//        tool->set_prMt(transform);
//        this->setDominantTool(tool->getUid());
//        break;
//      }
//      it++;
//    }
//  }

  // replaced above code with simpler version... ?
  this->setDominantTool(this->getManualTool()->getUid());

  this->connectSignalsAndSlots();

  mConfigured = true;
  messageManager()->sendInfo("ToolManager is configured.");
  emit configured();
}
void ToolManager::initialize()
{
  if (!mConfigured)
  {
    messageManager()->sendWarning(
        "Please configure before trying to initialize.");
    return;
  }
  mTracker->open();
  mTracker->attachTools(mConfiguredTools);
}
void ToolManager::startTracking()
{
  if (!mInitialized)
  {
    messageManager()->sendWarning(
        "Please initialize before trying to start tracking.");
    return;
  }
  mTracker->startTracking();
}
void ToolManager::stopTracking()
{
  if (!mTracking)
  {
    messageManager()->sendWarning(
        "Please start tracking before trying to stop tracking.");
    return;
  }
  mTracker->stopTracking();
}
void ToolManager::saveToolsSlot()
{
  saveTransformsAndTimestamps();
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

  std::cout << "void ToolManager::setDominantTool( "+uid+" )" << std::endl;

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
    //uidsAndNames.insert(std::pair<std::string, std::string>(((*it).second)->getUid(), ((*it).second)->getName()));
    uidsAndNames[((*it).second)->getUid()] = ((*it).second)->getName();
    it++;
  }
  ToolMapConstIter iter = mConfiguredTools->begin();
  while (iter != mConfiguredTools->end())
  {
    //uidsAndNames.insert(std::pair<std::string, std::string>(((*iter).second)->getUid(), ((*iter).second)->getName()));
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
  //	m_rMpr = val;
  //	emit rMprChanged();
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
    //TODO: Check/move following line, filename should probably be set elsewhere
    ((*it).second)->setTransformSaveFile(filePathAndName);
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

void ToolManager::receiveToolReport(ToolMessage message, bool state,
    bool success, stdString uid)
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
  messageManager()->sendInfo(report);
}
void ToolManager::receiveTrackerReport(Tracker::Message message, bool state,
    bool success, std::string uid)
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
      //Should this really be done here?
      mInitialized = success;
      emit initialized();
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
      messageManager()->sendInfo("ToolManager is initialized.");
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
  messageManager()->sendInfo(report);
}
bool ToolManager::pathsExists()
{
  QDir dir;
  bool confiurationFileExists = QFile::exists(QString(
      mConfigurationFilePath.c_str()));
  bool loggingFolderExists = dir.exists(QString(mLoggingFolder.c_str()));
  if (!confiurationFileExists)
  {
    messageManager()->sendInfo(mConfigurationFilePath + " does not exists.");
    return false;
  }
  if (!loggingFolderExists)
  {
    messageManager()->sendInfo(mLoggingFolder + " does not exists.");
    return false;
  }
  return true;
}
bool ToolManager::readConfigurationFile(QDomNodeList& trackerNodeList, QList<
    QDomNodeList>& toolNodeList)
{
  QFile configurationFile(QString(mConfigurationFilePath.c_str()));
  QFileInfo configurationFileInfo(configurationFile);
  QString configurationPath = configurationFileInfo.path() + "/";

  if (!configurationFile.open(QIODevice::ReadOnly))
  {
    messageManager()->sendInfo("Could not open " + mConfigurationFilePath + ".");
    return false;
  }
  QDomDocument configureDoc;
  if (!configureDoc.setContent(&configurationFile))
  {
    messageManager()->sendInfo("Could not set the xml content of the file "
        + mConfigurationFilePath);
    return false;
  }

  //tracker
  trackerNodeList
      = configureDoc.elementsByTagName(QString(mTrackerTag.c_str()));

  //tools
  QDomNodeList toolFileList = configureDoc.elementsByTagName(QString(
      mToolfileTag.c_str()));
  for (int i = 0; i < toolFileList.count(); i++)
  {
    std::string iString = "" + i;
    QDomNode filenameNode = toolFileList.item(i).firstChild();
    if (filenameNode.isNull())
    {
      messageManager()->sendInfo("Toolfiletag " + iString
          + " does not containe any usefull info. Skipping this tool.");
      continue;
    }
    QString filename = filenameNode.nodeValue();
    if (filename.isEmpty())
    {
      messageManager()->sendInfo("Toolfiletag " + iString
          + " does not contain readable text. Skipping this tool.");
      continue;
    }
    QFile toolFile(configurationPath + filename);
    QDir dir;
    if (!toolFile.exists())
    {
      messageManager()->sendInfo(filename.toStdString()
          + " does not exists. Skipping this tool.");
      continue;
    } else
    {
      messageManager()->sendInfo(filename.toStdString() + " exists.");
    }
    QDomDocument toolDoc;
    if (!toolDoc.setContent(&toolFile))
    {
      messageManager()->sendInfo("Could not set the xml content of the file "
          + filename.toStdString());
      continue;
    }
    QDomNodeList toolList =
        toolDoc.elementsByTagName(QString(mToolTag.c_str()));
    toolNodeList.push_back(toolList);
  }
  return true;
}
TrackerPtr ToolManager::configureTracker(QDomNodeList& trackerNodeList)
{
  std::vector<TrackerPtr> trackers;
  Tracker::InternalStructure internalStructure;
  for (int i = 0; i < trackerNodeList.count(); i++)
  {
    std::string iString = "" + i;
    QDomNode trackerNode = trackerNodeList.at(i);
    const QDomElement trackerType = trackerNode.firstChildElement(QString(
        mTrackerTypeTag.c_str()));
    if (trackerType.isNull())
    {
      messageManager()->sendInfo("Tracker " + iString
          + " does not have the required tag <type>.");
      continue;
    }
    QString text = trackerType.text();
    if (text.contains("polaris", Qt::CaseInsensitive))
    {
      if (text.contains("spectra", Qt::CaseInsensitive))
      {
        internalStructure.mType = Tracker::TRACKER_POLARIS_SPECTRA;
      } else if (text.contains("vicra", Qt::CaseInsensitive))
      {
        internalStructure.mType = Tracker::TRACKER_POLARIS_VICRA;
      } else
      {
        internalStructure.mType = Tracker::TRACKER_POLARIS;
      }
    } else if (text.contains("aurora", Qt::CaseInsensitive))
    {
      internalStructure.mType = Tracker::TRACKER_AURORA;
    } else if (text.contains("micron", Qt::CaseInsensitive))
    {
      internalStructure.mType = Tracker::TRACKER_MICRON;
    } else
    {
      internalStructure.mType = Tracker::TRACKER_NONE;
    }
    internalStructure.mLoggingFolderName = mLoggingFolder;
    trackers.push_back(TrackerPtr(new Tracker(internalStructure)));
  }
  if (trackers.empty())
  {
    internalStructure.mType = Tracker::TRACKER_NONE;
    internalStructure.mLoggingFolderName = mLoggingFolder;
    trackers.push_back(TrackerPtr(new Tracker(internalStructure)));
  }
  return trackers.at(0);
}
ssc::ToolManager::ToolMapPtr ToolManager::configureTools(
    QList<QDomNodeList>& toolNodeList)
{
  QFile configurationFile(QString(mConfigurationFilePath.c_str()));
  QFileInfo configurationFileInfo(configurationFile);
  QString configurationPath = configurationFileInfo.path() + "/";

  ssc::ToolManager::ToolMapPtr tools(new ssc::ToolManager::ToolMap());
  QDomNode node;
  for (int i = 0; i < toolNodeList.size(); i++)
  {
    Tool::InternalStructure internalStructure;
    QDomNodeList toolNodes = toolNodeList.at(i);
    if (toolNodes.size() < 1)
    {
      messageManager()->sendInfo("Found no <tool> tags in the toolxmlfile.");
      continue;
    }
    QDomNode toolNode = toolNodes.item(0); //A toolfile should only contain 1 tool tag
    if (toolNode.isNull())
    {
      messageManager()->sendInfo("Could not read the <tool> tag.");
      continue;
    }

    QDomElement toolTypeElement = toolNode.firstChildElement(QString(
        mToolTypeTag.c_str()));
    QString toolTypeText = toolTypeElement.text();
    if (toolTypeText.contains("reference", Qt::CaseInsensitive))
    {
      internalStructure.mType = ssc::Tool::TOOL_REFERENCE;
    } else if (toolTypeText.contains("pointer", Qt::CaseInsensitive))
    {
      internalStructure.mType = ssc::Tool::TOOL_POINTER;
    } else if (toolTypeText.contains("usprobe", Qt::CaseInsensitive))
    {
      internalStructure.mType = ssc::Tool::TOOL_US_PROBE;
    } else
    {
      internalStructure.mType = ssc::Tool::TOOL_NONE;
    }

    QDomElement toolIdElement = toolNode.firstChildElement(QString(
        mToolIdTag.c_str()));
    QString toolIdText = toolIdElement.text();
    internalStructure.mUid = toolIdText.toStdString();

    QDomElement toolNameElement = toolNode.firstChildElement(QString(
        mToolNameTag.c_str()));
    QString toolNameText = toolNameElement.text();
    internalStructure.mName = toolNameText.toStdString();

    QDomElement toolGeofileElement = toolNode.firstChildElement(QString(
        mToolGeoFileTag.c_str()));
    QString toolGeofileText = toolGeofileElement.text();
    if (!toolGeofileText.isEmpty())
      toolGeofileText = configurationPath + toolGeofileText;
    internalStructure.mGraphicsFileName = toolGeofileText.toStdString();

    QDomElement toolSensorElement = toolNode.firstChildElement(QString(
        mToolSensorTag.c_str()));
    if (toolSensorElement.isNull())
    {
      messageManager()->sendInfo(
          "Could not find the <sensor> tag under the <tool> tag. Aborting tihs tool.");
      continue;
    }
    QDomElement toolSensorTypeElement = toolSensorElement.firstChildElement(
        QString(mToolSensorTypeTag.c_str()));
    QString toolSensorTypeText = toolSensorTypeElement.text();
    if (toolSensorTypeText.contains("polaris", Qt::CaseInsensitive))
    {
      if (toolSensorTypeText.contains("spectra", Qt::CaseInsensitive))
      {
        internalStructure.mTrackerType = Tracker::TRACKER_POLARIS_SPECTRA;
      } else if (toolSensorTypeText.contains("vicra", Qt::CaseInsensitive))
      {
        internalStructure.mTrackerType = Tracker::TRACKER_POLARIS_VICRA;
      } else
      {
        internalStructure.mTrackerType = Tracker::TRACKER_POLARIS;
      }
    } else if (toolSensorTypeText.contains("aurora", Qt::CaseInsensitive))
    {
      internalStructure.mTrackerType = Tracker::TRACKER_AURORA;
    } else if (toolSensorTypeText.contains("micron", Qt::CaseInsensitive))
    {
      internalStructure.mTrackerType = Tracker::TRACKER_MICRON;
    } else
    {
      internalStructure.mTrackerType = Tracker::TRACKER_NONE;
    }

    QDomElement toolSensorWirelessElement =
        toolSensorElement.firstChildElement(QString(
            mToolSensorWirelessTag.c_str()));
    QString toolSensorWirelessText = toolSensorWirelessElement.text();
    if (toolSensorWirelessText.contains("yes", Qt::CaseInsensitive))
      internalStructure.mWireless = true;
    else if (toolSensorWirelessText.contains("no", Qt::CaseInsensitive))
      internalStructure.mWireless = false;

    QDomElement toolSensorDOFElement = toolSensorElement.firstChildElement(
        QString(mToolSensorDOFTag.c_str()));
    QString toolSensorDOFText = toolSensorDOFElement.text();
    if (toolSensorDOFText.contains("5", Qt::CaseInsensitive))
      internalStructure.m5DOF = true;
    else if (toolSensorDOFText.contains("6", Qt::CaseInsensitive))
      internalStructure.m5DOF = false;

    QDomElement toolSensorPortnumberElement =
        toolSensorElement.firstChildElement(QString(
            mToolSensorPortnumberTag.c_str()));
    QString toolSensorPortnumberText = toolSensorPortnumberElement.text();
    internalStructure.mPortNumber = toolSensorPortnumberText.toInt();

    QDomElement toolSensorChannelnumberElement =
        toolSensorElement.firstChildElement(QString(
            mToolSensorChannelnumberTag.c_str()));
    QString toolSensorChannelnumberText = toolSensorChannelnumberElement.text();
    internalStructure.mChannelNumber = toolSensorChannelnumberText.toInt();

    QDomElement toolSensorRomFileElement = toolSensorElement.firstChildElement(
        QString(mToolSensorRomFileTag.c_str()));
    QString toolSensorRomFileText = toolSensorRomFileElement.text();
    if (!toolSensorRomFileText.isEmpty())
      toolSensorRomFileText = configurationPath + toolSensorRomFileText;
    internalStructure.mSROMFilename = toolSensorRomFileText.toStdString();

    QDomElement toolCalibrationElement = toolNode.firstChildElement(QString(
        mToolCalibrationTag.c_str()));
    if (toolCalibrationElement.isNull())
    {
      messageManager()->sendInfo(
          "Could not find the <calibration> tag under the <tool> tag. Aborting this tool.");
      continue;
    }
    QDomElement toolCalibrationFileElement =
        toolCalibrationElement.firstChildElement(QString(
            mToolCalibrationFileTag.c_str()));
    QString toolCalibrationFileText = toolCalibrationFileElement.text();
    if (!toolCalibrationFileText.isEmpty())
      toolCalibrationFileText = configurationPath + toolCalibrationFileText;
    internalStructure.mCalibrationFilename
        = toolCalibrationFileText.toStdString();

    internalStructure.mTransformSaveFileName = mLoggingFolder;
    internalStructure.mLoggingFolderName = mLoggingFolder;

    Tool* cxTool = new Tool(internalStructure);
    ssc::ToolPtr tool(cxTool);
    if (tool->getType() == ssc::Tool::TOOL_REFERENCE)
    {
      mReferenceTool = tool;
    }
    //tools->insert(std::pair<std::string, ssc::ToolPtr>(tool->getUid(), tool));
    (*tools)[tool->getUid()] = tool;
  }
  return tools;
}
void ToolManager::addConnectedTool(std::string uid)
{  
  ssc::ToolManager::ToolMap::iterator it = mConfiguredTools->find(uid);  
  if (it == mConfiguredTools->end() || !it->second)
  {
    messageManager()->sendInfo("Tool with id " + uid
        + " was not found to be configured "
          ", thus could not add is as a connected tool.");
    return;
  }
  //mConnectedTools->insert(std::pair<std::string, ssc::ToolPtr>((*it).first, (*it).second));
  (*mConnectedTools)[it->first] = it->second;
  ssc::ToolPtr tool = it->second;
  //connect visible/hidden signal to domiantCheck
  connect(tool.get(), SIGNAL(toolVisible(bool)),
          this, SLOT(dominantCheckSlot()));
  
  mConfiguredTools->erase(it);
  messageManager()->sendInfo("Tool with id " + uid
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
  messageManager()->sendInfo("Signals and slots have been connected.");
}
void ToolManager::checkTimeoutsAndRequestTransform()
{
  mPulseGenerator->CheckTimeouts();

  if (!mReferenceTool)
    return;

  ToolPtr refTool = boost::shared_dynamic_cast<Tool>(mReferenceTool);
  ToolMap::iterator it = mConnectedTools->begin();
  for(;it != mConnectedTools->end();++it)
  {
    //Tool* refTool = static_cast<Tool*> (mReferenceTool.get());
    //static_cast<Tool*> ((*it).second.get())->getPointer()->RequestComputeTransformTo(refTool->getPointer());
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
