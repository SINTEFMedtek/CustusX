#include "cxToolManager.h"

#include <QTimer>
#include <QDir>
#include <QList>
#include <QDomDocument>
#include <QMetaType>
#include <vtkDoubleArray.h>
#include "cxTool.h"
#include "cxTracker.h"
#include "cxMessageManager.h"
/**
 * cxToolManager.cpp
 *
 * \brief
 *
 * \date Nov 6, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */

namespace cx
{
// Doxygen ignores code between \cond and \endcond
/// \cond
ToolManager* ToolManager::mCxInstance = NULL;
/// \endcond
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
  mMessageManager(MessageManager::getInstance()),
  mConfigurationFilePath(""),
  mLoggingFolder(""),
  mTracker(TrackerPtr()),
  //mConfiguredTools(new ssc::ToolManager::ToolMap),
  mConnectedTools(new ssc::ToolManager::ToolMap),
  mDominantTool(ssc::ToolPtr()),
  mReferenceTool(ssc::ToolPtr()),
  m_rMpr(ssc::Transform3DPtr(new ssc::Transform3D())),
  mConfigured(false),
  mInitialized(false),
  mTracking(false),
  mTrackerTag("tracker"),
  mTrackerTypeTag("type"),
  mToolfileTag("toolfile"),
  mToolTag("tool"),
  mToolTypeTag("type"),
  mToolIdTag("id"),
  mToolNameTag("name"),
  mToolGeoFileTag("geo_file"),
  mToolSensorTag("sensor"),
  mToolSensorTypeTag("type"),
  mToolSensorWirelessTag("wireless"),
  mToolSensorDOFTag("DOF"),
  mToolSensorPortnumberTag("portnumber"),
  mToolSensorChannelnumberTag("channelnumber"),
  mToolSensorRomFileTag("rom_file"),
  mToolCalibrationTag("calibration"),
  mToolCalibrationFileTag("cal_file"),
  mPulseGenerator(igstk::PulseGenerator::New()),
  mToolSamples(vtkDoubleArray::New())
{
  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(checkTimeoutsAndRequestTransform()));

  igstk::RealTimeClock::Initialize();

  mPulseGenerator->RequestSetFrequency( 30.0);
  mPulseGenerator->RequestStart();

  mToolSamples->SetNumberOfComponents(4);
}
ToolManager::~ToolManager()
{}
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
  if(!this->pathsExists())
    return;

  QDomNodeList trackerNode;
  QList<QDomNodeList> toolNodeList;
  if(!this->readConfigurationFile(trackerNode, toolNodeList))
    return;

  mTracker = this->configureTracker(trackerNode);
  mConfiguredTools = this->configureTools(toolNodeList);

  if(!mConfiguredTools->empty())
  {
    ToolMapConstIter it = mConfiguredTools->begin();
    while (it != mConfiguredTools->end())
    {
      ssc::ToolPtr tool = (*it).second;
      if(tool->getType() != ssc::Tool::TOOL_REFERENCE)
      {
        this->setDominantTool(tool->getUid());
        break;
      }
      it++;
    }
  }

  this->connectSignalsAndSlots();

  mConfigured = true;
  emit toolManagerReport("ToolManager is configured.");
  emit configured();
}
void ToolManager::initialize()
{
  if(!mConfigured)
  {
    mMessageManager->sendWarning("Please configure before trying to initialize.");
    return;
  }
  mTracker->open();
  mTracker->attachTools(mConfiguredTools);
}
void ToolManager::startTracking()
{
  if(!mInitialized)
  {
    mMessageManager->sendWarning("Please initialize before trying to start tracking.");
    return;
  }
  mTracker->startTracking();
}
void ToolManager::stopTracking()
{
  if(!mTracking)
  {
    mMessageManager->sendWarning("Please start tracking before trying to stop tracking.");
    return;
  }
  mTracker->stopTracking();
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
void ToolManager::setDominantTool(const std::string& uid)
{
  ToolMapConstIter iter = mConfiguredTools->find(uid);
  if (iter != mConfiguredTools->end())
  {
    mDominantTool = ((*iter).second);
    emit dominantToolChanged(uid);
  }
  ToolMapConstIter it = mConnectedTools->find(uid);
  if (it != mConnectedTools->end())
  {
    mDominantTool = ((*it).second);
    emit dominantToolChanged(uid);
  }
}
std::map<std::string, std::string> ToolManager::getToolUidsAndNames() const
{
  std::map<std::string, std::string> uidsAndNames;

  ToolMapConstIter it = mConnectedTools->begin();
  while (it != mConnectedTools->end())
  {
    uidsAndNames.insert(std::pair<std::string, std::string>(
        ((*it).second)->getUid(), ((*it).second)->getName()));
    it++;
  }
  ToolMapConstIter iter = mConfiguredTools->begin();
  while (iter != mConfiguredTools->end())
  {
    uidsAndNames.insert(std::pair<std::string, std::string>(
        ((*iter).second)->getUid(), ((*iter).second)->getName()));
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
  return m_rMpr;
}
void ToolManager::set_rMpr(const ssc::Transform3DPtr& val)
{
  ssc::Transform3D trans = *val.get();
  std::cout << "cxToolManager::set_rMpr: \n" << trans << std::endl;
  m_rMpr = val;
}
ssc::ToolPtr ToolManager::getReferenceTool() const
{
  return mReferenceTool;
}
void ToolManager::saveTransformsAndTimestamps(
    std::string filePathAndName)
{
  ToolMapConstIter it = mConnectedTools->begin();
  while (it != mConnectedTools->end())
  {
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
vtkDoubleArrayPtr ToolManager::getToolSamples()
{
  return mToolSamples;
}
void ToolManager::receiveToolReport(ToolMessage message, bool state, bool success, stdString uid)
{
  std::string toolUid = ""+uid;
  std::string report ="";

  switch (message)
  {
  case Tool::TOOL_INVALID_REQUEST:
    report.append(toolUid+" performed an invalid request.");
    break;
  case Tool::TOOL_HW_CONFIGURED:
    report.append(toolUid+" is ");
    if(!success)
      report.append("not ");
    report.append(" hardware configured successfully.");
    break;
  case Tool::TOOL_ATTACHED_TO_TRACKER:
    report.append(toolUid+" was ");
    if(!success)
      report.append("not ");
    if(!state)
      report.append("detached from ");
    else
      report.append("attached to ");
    report.append("the tracker.");
    if(success && state)
      this->addConnectedTool(uid);
    break;
  case Tool::TOOL_VISIBLE:
    report.append(toolUid+" is ");
    if(!state)
      report.append("not ");
    report.append("visible.");
    break;
  case Tool::TOOL_TRACKED:
    report.append(toolUid+" ");
    if(state)
      report.append("started tracking ");
    else
      report.append("stopped tracking ");
    if(!success)
      report.append("not ");
    report.append("successfully.");
    break;
  case Tool::TOOL_COORDINATESYSTEM_TRANSFORM:
    report.append(toolUid+" got a new transform and timestamp.");
    break;
  case Tool::TOOL_NDI_PORT_NUMBER:
    report.append(toolUid+" could not used the given port number.");
    break;
  case Tool::TOOL_NDI_SROM_FILENAME:
    report.append(toolUid+" could not used the given SROM file.");
    break;
  case Tool::TOOL_NDI_PART_NUMBER:
    report.append(toolUid+" did not have a valid part number.");
    break;
  case Tool::TOOL_AURORA_CHANNEL_NUMBER:
    report.append(toolUid+" could not use the given Aurora channel number.");
    break;
  default:
    report.append(toolUid+" reported an unknown message.");
    break;
  }
  emit toolManagerReport(report);
}
void ToolManager::receiveTrackerReport(Tracker::Message message, bool state, bool success, std::string uid)
{
  std::string trackerUid = uid;
  std::string report = "";

  switch (message)
  {
  case Tracker::TRACKER_INVALID_REQUEST:
    report.append(trackerUid+" performed an invalid request.");
    break;
  case Tracker::TRACKER_OPEN:
    report.append(trackerUid+" is ");
    if(!success)
      report.append("not ");
    if(state)
    {
      mInitialized = success;
      emit initialized();
      report.append("open.");
    }
    else
    {
      mInitialized = !success;
      report.append("closed.");
    }
    break;
  case Tracker::TRACKER_INITIALIZED:
    report.append(trackerUid+" is ");
    if(!success)
    {
      report.append("not ");
      mInitialized = false;
    }
    else
    {
      mInitialized = true;
      emit initialized();
      emit toolManagerReport("ToolManager is initialized.");
    }
    report.append("initialized.");
    break;
  case Tracker::TRACKER_TRACKING:
    report.append(trackerUid+" could ");
    if(!success)
      report.append("not ");
    if(state)
      report.append("start tracking.");
    else
      report.append("stop tracking.");
    if(success && state)
    {
      mTracking = true;
      emit trackingStarted();
      mTimer->start(33);
    }
    if(success && !state)
    {
      mTracking = false;
      emit trackingStopped();
      mTimer->stop();
    }
    break;
  case Tracker::TRACKER_UPDATE_STATUS:
    report.append(trackerUid+" has a tool that is ");
    if(!success)
      report.append("not ");
    report.append("updated successfully.");
    break;
  case Tracker::TRACKER_TOOL_TRANSFORM_UPDATED:
    report.append(trackerUid+" has a tool that is ");
    if(!success)
      report.append("not ");
    report.append("updated successfully.");
    break;
  case Tracker::TRACKER_COMMUNICATION_COMPLETE:
    return; //this is spamming the screen....
    report.append("Communication did something ");
    if(!success)
      report.append("not ");
    report.append("successfully.");
    break;
  case Tracker::TRACKER_COMMUNICATION_INPUT_OUTPUT_ERROR:
    report.append(trackerUid+": TRACKER_COMMUNICATION_INPUT_OUTPUT_ERROR");
    break;
  case Tracker::TRACKER_COMMUNICATION_INPUT_OUTPUT_TIMEOUT:
    report.append(trackerUid+": TRACKER_COMMUNICATION_INPUT_OUTPUT_TIMEOUT");
    break;
  case Tracker::TRACKER_COMMUNICATION_OPEN_PORT_ERROR:
    report.append(trackerUid+": TRACKER_COMMUNICATION_OPEN_PORT_ERROR");
    break;
  default:
    report.append(trackerUid+" reported an unknown message.");
    break;
  }
  emit toolManagerReport(report);
}
bool ToolManager::pathsExists()
{
  QDir dir;
  bool confiurationFileExists = QFile::exists(QString(mConfigurationFilePath.c_str()));
  bool loggingFolderExists = dir.exists(QString(mLoggingFolder.c_str()));
  if(!confiurationFileExists)
  {
    emit toolManagerReport(mConfigurationFilePath+" does not exists.");
    return false;
  }
  if(!loggingFolderExists)
  {
    emit toolManagerReport(mLoggingFolder+" does not exists.");
    return false;
  }
  return true;
}
bool ToolManager::readConfigurationFile(QDomNodeList& trackerNodeList, QList<QDomNodeList>& toolNodeList)
{
  QFile configurationFile(QString(mConfigurationFilePath.c_str()));
  QFileInfo configurationFileInfo(configurationFile);
  QString configurationPath = configurationFileInfo.path()+"/";

  if(!configurationFile.open(QIODevice::ReadOnly))
  {
    toolManagerReport("Could not open "+mConfigurationFilePath+".");
    return false;
  }
  QDomDocument configureDoc;
  if(!configureDoc.setContent(&configurationFile))
  {
    toolManagerReport("Could not set the xml content of the file "+mConfigurationFilePath);
    return false;
  }

  //tracker
  trackerNodeList = configureDoc.elementsByTagName(QString(mTrackerTag.c_str()));

  //tools
  QDomNodeList toolFileList = configureDoc.elementsByTagName(QString(mToolfileTag.c_str()));
  for(int i=0; i<toolFileList.count(); i++)
  {
    std::string iString = ""+i;
    QDomNode filenameNode = toolFileList.item(i).firstChild();
    if(filenameNode.isNull())
    {
      emit toolManagerReport("Toolfiletag "+iString+" does not containe any usefull info. Skipping this tool.");
      continue;
    }
    QString filename = filenameNode.nodeValue();
    if(filename.isEmpty())
    {
      emit toolManagerReport("Toolfiletag "+iString+" does not contain readable text. Skipping this tool.");
      continue;
    }
    QFile toolFile(configurationPath+filename);
    QDir dir;
    if(!toolFile.exists())
    {
      emit toolManagerReport(filename.toStdString()+" does not exists. Skipping this tool.");
      continue;
    }
    else
    {
      emit toolManagerReport(filename.toStdString()+" exists.");
    }
    QDomDocument toolDoc;
    if(!toolDoc.setContent(&toolFile))
    {
      emit toolManagerReport("Could not set the xml content of the file "+filename.toStdString());
      continue;
    }
    QDomNodeList toolList = toolDoc.elementsByTagName(QString(mToolTag.c_str()));
    toolNodeList.push_back(toolList);
  }
  return true;
}
TrackerPtr ToolManager::configureTracker(QDomNodeList& trackerNodeList)
{
  std::vector<TrackerPtr> trackers;
  Tracker::InternalStructure internalStructure;
  for(int i=0; i< trackerNodeList.count(); i++)
  {
    std::string iString = ""+i;
    QDomNode trackerNode = trackerNodeList.at(i);
    const QDomElement trackerType = trackerNode.firstChildElement(QString(mTrackerTypeTag.c_str()));
    if(trackerType.isNull())
    {
      emit toolManagerReport("Tracker "+iString+" does not have the required tag <type>.");
      continue;
    }
    QString text = trackerType.text();
    if(text.contains("polaris", Qt::CaseInsensitive))
    {
      if(text.contains("spectra", Qt::CaseInsensitive))
      {
        internalStructure.mType = Tracker::TRACKER_POLARIS_SPECTRA;
      }
      else if(text.contains("vicra", Qt::CaseInsensitive))
      {
        internalStructure.mType = Tracker::TRACKER_POLARIS_VICRA;
      }
      else
      {
        internalStructure.mType = Tracker::TRACKER_POLARIS;
      }
    }
    else if(text.contains("aurora", Qt::CaseInsensitive))
    {
      internalStructure.mType = Tracker::TRACKER_AURORA;
    }
    else if(text.contains("micron", Qt::CaseInsensitive))
    {
      internalStructure.mType = Tracker::TRACKER_MICRON;
    }
    else
    {
      internalStructure.mType = Tracker::TRACKER_NONE;
    }
    internalStructure.mLoggingFolderName = mLoggingFolder;
    trackers.push_back(TrackerPtr(new Tracker(internalStructure)));
  }
  if(trackers.empty())
  {
    internalStructure.mType = Tracker::TRACKER_NONE;
    internalStructure.mLoggingFolderName = mLoggingFolder;
    trackers.push_back(TrackerPtr(new Tracker(internalStructure)));
  }
  return trackers.at(0);
}
ssc::ToolManager::ToolMapPtr ToolManager::configureTools(QList<QDomNodeList>& toolNodeList)
{
  QFile configurationFile(QString(mConfigurationFilePath.c_str()));
  QFileInfo configurationFileInfo(configurationFile);
  QString configurationPath = configurationFileInfo.path()+"/";

  ssc::ToolManager::ToolMapPtr tools(new ssc::ToolManager::ToolMap());
  QDomNode node;
  for(int i=0; i<toolNodeList.size(); i++)
  {
    Tool::InternalStructure internalStructure;
    QDomNodeList toolNodes = toolNodeList.at(i);
    if(toolNodes.size() < 1)
    {
      emit toolManagerReport("Found no <tool> tags in the toolxmlfile.");
      continue;
    }
    QDomNode toolNode = toolNodes.item(0); //A toolfile should only contain 1 tool tag
    if(toolNode.isNull())
    {
      emit toolManagerReport("Could not read the <tool> tag.");
      continue;
    }

    QDomElement toolTypeElement = toolNode.firstChildElement(QString(mToolTypeTag.c_str()));
    QString toolTypeText = toolTypeElement.text();
    if(toolTypeText.contains("reference", Qt::CaseInsensitive))
    {
      internalStructure.mType = ssc::Tool::TOOL_REFERENCE;
    }
    else if(toolTypeText.contains("pointer", Qt::CaseInsensitive))
    {
      internalStructure.mType = ssc::Tool::TOOL_POINTER;
    }
    else if(toolTypeText.contains("usprobe", Qt::CaseInsensitive))
    {
      internalStructure.mType = ssc::Tool::TOOL_US_PROBE;
    }
    else
    {
      internalStructure.mType = ssc::Tool::TOOL_NONE;
    }

    QDomElement toolIdElement = toolNode.firstChildElement(QString(mToolIdTag.c_str()));
    QString toolIdText = toolIdElement.text();
    internalStructure.mUid = toolIdText.toStdString();

    QDomElement toolNameElement = toolNode.firstChildElement(QString(mToolNameTag.c_str()));
    QString toolNameText = toolNameElement.text();
    internalStructure.mName = toolNameText.toStdString();

    QDomElement toolGeofileElement = toolNode.firstChildElement(QString(mToolGeoFileTag.c_str()));
    QString toolGeofileText = toolGeofileElement.text();
    if(!toolGeofileText.isEmpty())
      toolGeofileText = configurationPath + toolGeofileText;
    internalStructure.mGraphicsFileName = toolGeofileText.toStdString();

    QDomElement toolSensorElement = toolNode.firstChildElement(QString(mToolSensorTag.c_str()));
    if(toolSensorElement.isNull())
    {
      emit toolManagerReport("Could not find the <sensor> tag under the <tool> tag. Aborting tihs tool.");
      continue;
    }
    QDomElement toolSensorTypeElement = toolSensorElement.firstChildElement(QString(mToolSensorTypeTag.c_str()));
    QString toolSensorTypeText = toolSensorTypeElement.text();
    if(toolSensorTypeText.contains("polaris", Qt::CaseInsensitive))
    {
      if(toolSensorTypeText.contains("spectra", Qt::CaseInsensitive))
      {
        internalStructure.mTrackerType = Tracker::TRACKER_POLARIS_SPECTRA;
      }
      else if(toolSensorTypeText.contains("vicra", Qt::CaseInsensitive))
      {
        internalStructure.mTrackerType = Tracker::TRACKER_POLARIS_VICRA;
      }
      else
      {
        internalStructure.mTrackerType = Tracker::TRACKER_POLARIS;
      }
    }
    else if(toolSensorTypeText.contains("aurora", Qt::CaseInsensitive))
    {
      internalStructure.mTrackerType = Tracker::TRACKER_AURORA;
    }
    else if(toolSensorTypeText.contains("micron", Qt::CaseInsensitive))
    {
      internalStructure.mTrackerType = Tracker::TRACKER_MICRON;
    }
    else
    {
      internalStructure.mTrackerType = Tracker::TRACKER_NONE;
    }

    QDomElement toolSensorWirelessElement = toolSensorElement.firstChildElement(QString(mToolSensorWirelessTag.c_str()));
    QString toolSensorWirelessText = toolSensorWirelessElement.text();
    if(toolSensorWirelessText.contains("yes", Qt::CaseInsensitive))
      internalStructure.mWireless = true;
    else if(toolSensorWirelessText.contains("no", Qt::CaseInsensitive))
      internalStructure.mWireless = false;

    QDomElement toolSensorDOFElement = toolSensorElement.firstChildElement(QString(mToolSensorDOFTag.c_str()));
    QString toolSensorDOFText = toolSensorDOFElement.text();
    if(toolSensorDOFText.contains("5", Qt::CaseInsensitive))
      internalStructure.m5DOF = true;
    else if(toolSensorDOFText.contains("6", Qt::CaseInsensitive))
      internalStructure.m5DOF = false;

    QDomElement toolSensorPortnumberElement = toolSensorElement.firstChildElement(QString(mToolSensorPortnumberTag.c_str()));
    QString toolSensorPortnumberText = toolSensorPortnumberElement.text();
    internalStructure.mPortNumber = toolSensorPortnumberText.toInt();

    QDomElement toolSensorChannelnumberElement = toolSensorElement.firstChildElement(QString(mToolSensorChannelnumberTag.c_str()));
    QString toolSensorChannelnumberText = toolSensorChannelnumberElement.text();
    internalStructure.mChannelNumber = toolSensorChannelnumberText.toInt();

    QDomElement toolSensorRomFileElement = toolSensorElement.firstChildElement(QString(mToolSensorRomFileTag.c_str()));
    QString toolSensorRomFileText = toolSensorRomFileElement.text();
    if(!toolSensorRomFileText.isEmpty())
      toolSensorRomFileText = configurationPath + toolSensorRomFileText;
    internalStructure.mSROMFilename = toolSensorRomFileText.toStdString();

    QDomElement toolCalibrationElement = toolNode.firstChildElement(QString(mToolCalibrationTag.c_str()));
    if(toolCalibrationElement.isNull())
    {
      emit toolManagerReport("Could not find the <calibration> tag under the <tool> tag. Aborting this tool.");
      continue;
    }
    QDomElement toolCalibrationFileElement = toolCalibrationElement.firstChildElement(QString(mToolCalibrationFileTag.c_str()));
    QString toolCalibrationFileText = toolCalibrationFileElement.text();
    if(!toolCalibrationFileText.isEmpty())
      toolCalibrationFileText = configurationPath + toolCalibrationFileText;
    internalStructure.mCalibrationFilename = toolCalibrationFileText.toStdString();

    internalStructure.mTransformSaveFileName = mLoggingFolder;
    internalStructure.mLoggingFolderName = mLoggingFolder;

    Tool* cxTool = new Tool(internalStructure);
    ssc::ToolPtr tool(cxTool);
    if(tool->getType() == ssc::Tool::TOOL_REFERENCE)
    {
      mReferenceTool = tool;
    }
    tools->insert(std::pair<std::string, ssc::ToolPtr>(tool->getUid(), tool));
  }
  return tools;
}
void ToolManager::addConnectedTool(std::string uid)
{
  ssc::ToolManager::ToolMap::iterator it = mConfiguredTools->find(uid);
  if(it == mConfiguredTools->end())
  {
    emit toolManagerReport("Tool with id "+uid+" was not found to be configured "
        ", thus could not add is as a connected tool.");
    return;
  }
  mConnectedTools->insert(std::pair<std::string, ssc::ToolPtr>((*it).first, (*it).second));
  mConfiguredTools->erase(it);
  emit toolManagerReport("Tool with id "+uid+" was moved from the configured to the connected map.");
}
void ToolManager::connectSignalsAndSlots()
{
  typedef Tracker::Message TrackerMessage;
  qRegisterMetaType<TrackerMessage>("TrackerMessage");
  typedef Tool::Message ToolMessage;
  qRegisterMetaType<ToolMessage>("ToolMessage");
  typedef std::string stdString;
  qRegisterMetaType<stdString>("stdString");

  connect(mTracker.get(), SIGNAL(trackerReport(TrackerMessage, bool, bool, stdString)),
         this, SLOT(receiveTrackerReport(TrackerMessage, bool, bool, stdString)));

  ssc::ToolManager::ToolMap::iterator it = mConfiguredTools->begin();
  while(it != mConfiguredTools->end())
  {
    connect(((*it).second).get(), SIGNAL(toolReport(ToolMessage, bool, bool, stdString)),
           this, SLOT(receiveToolReport(ToolMessage, bool, bool, stdString)));
    it++;
  }
  emit toolManagerReport("Signals and slots have been connected.");
}
void ToolManager::checkTimeoutsAndRequestTransform()
{
  mPulseGenerator->CheckTimeouts();

  if(mReferenceTool.get() == NULL)
    return;

  ToolMap::iterator it = mConnectedTools->begin();
  while(it != mConnectedTools->end())
  {
    Tool* refTool = static_cast<Tool*>(mReferenceTool.get());
    static_cast<Tool*>((*it).second.get())->getPointer()->RequestComputeTransformTo(refTool->getPointer());
    it++;
  }
}
void ToolManager::addToolSampleSlot(double x, double y, double z, unsigned int index)
{
  double addToolSample[4] = {x, y, z, (double)index};

  int numberOfLandmarks = mToolSamples->GetNumberOfTuples();
  //if index exists, we treat it as an edit operation
  for(int i=0; i<= numberOfLandmarks-1; i++)
  {
    double* landmark = mToolSamples->GetTuple(i);
    if(landmark[3] == index)
    {
      mToolSamples->SetTupleValue(i, addToolSample);
      emit toolSampleAdded(x, y, z, index);
      return;
    }
  }
  //else it's an add operation
  mToolSamples->InsertNextTupleValue(addToolSample);
  emit toolSampleAdded(x,y,z,index);
}
void ToolManager::removeToolSampleSlot(double x, double y, double z, unsigned int index)
{
  int numberOfLandmarks = mToolSamples->GetNumberOfTuples();
  for(int i=0; i<= numberOfLandmarks-1; i++)
  {
    double* landmark = mToolSamples->GetTuple(i);
    if(landmark[3] == index)
    {
      mToolSamples->RemoveTuple(i);
      emit toolSampleRemoved(x, y, z, index);
    }
  }
}
}//namespace cx
