#include "cxTool.h"

#include <vtkPolyData.h>
#include <vtkConeSource.h>
#include <vtkSTLReader.h>
#include <QDir>
#include <QDateTime>
#include <QStringList>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "cxToolManager.h"
#include "cxDataLocations.h"

namespace cx
{

Tool::Tool(InternalStructure& internalStructure) :
  mInternalStructure(internalStructure),
  mValid(false),
  mTool(NULL),
  mToolObserver(ObserverType::New()),
  mTransforms(new Transform3DVector()),
  mTimestamps(new DoubleVector()),
  mPolyData(NULL),
  m_prMt(new ssc::Transform3D()),
  mConfigured(false),
  mVisible(false),
  mAttachedToTracker(false),
  mTracked(false),
  mToolTipOffset(0),
  mProbeSector(ssc::ProbeSector()),
  mProbeSectorConfiguration("")
{
  ssc::Tool::mUid = mInternalStructure.mUid;
  ssc::Tool::mName = mInternalStructure.mName;

  //For debugging
  //this->printInternalStructure();

  mToolObserver->SetCallbackFunction(this, &Tool::toolTransformCallback);

  if(this->verifyInternalStructure())
  {
    this->determineToolsCalibration();
    mTool = this->buildInternalTool();
    this->createPolyData();
    mValid = true;
  }else
  {
    ssc::messageManager()->sendError("Tool: "+ssc::Tool::mUid+" was created with invalid internal structure.");
    mValid = false;
  }

  // Read ultrasoundImageConfigs.xml file
  QString xmlFileName = cx::DataLocations::getRootConfigPath()+QString("/tool/ProbeCalibConfigs.xml");
  mXml = new ProbeXmlConfigParser(xmlFileName);
}

Tool::~Tool()
{}

ssc::Tool::Type Tool::getType() const
{
  return mInternalStructure.mType;
}

std::string Tool::getGraphicsFileName() const
{
  return mInternalStructure.mGraphicsFileName;
}

vtkPolyDataPtr Tool::getGraphicsPolyData() const
{
  return mPolyData;
}

void Tool::saveTransformsAndTimestamps()
{
  if(this->getType() == Tool::TOOL_REFERENCE)
    return;  //we don't save transforms and timestamps for reference tools

  QDateTime dateTime = QDateTime::currentDateTime();
  std::string stamp = dateTime.toString(QString("ddMMyyhhmmss")).toStdString();

  std::stringstream timestampsName;
  std::stringstream transformsName;
  timestampsName << mInternalStructure.mTransformSaveFileName 
  << ssc::Tool::mName << "_" << stamp << "_timestamps.txt";
  transformsName << mInternalStructure.mTransformSaveFileName 
  << ssc::Tool::mName << "_" << stamp << "_transforms.txt";
  
  //Save the timestamps
  std::ofstream timestamps;
  timestamps.open(timestampsName.str().c_str());
  timestamps << std::fixed;
  timestamps << mTimestamps->size();
  timestamps << std::endl;
  DoubleVector::iterator it1 = mTimestamps->begin();
  while(it1 != mTimestamps->end())
  {
    timestamps << (*it1) << " ";
    timestamps << std::endl;
    it1++;
  }
  mTimestamps.reset();
  timestamps.close();
  
  //Save the transforms
  std::ofstream transforms;
  transforms.open(transformsName.str().c_str());
  transforms << mTransforms->size();
  transforms << std::endl;
  Transform3DVector::iterator it2 = mTransforms->begin();
  while(it2 != mTransforms->end())
  {
    for(int row=0; row<3; row++)
    {
      transforms << (*it2)->matrix()->GetElement(row, 0) << " ";
      transforms << (*it2)->matrix()->GetElement(row, 1) << " ";
      transforms << (*it2)->matrix()->GetElement(row, 2) << " ";
      transforms << (*it2)->matrix()->GetElement(row, 3);
      transforms << std::endl;
    }
    transforms << std::endl;
    it2++;
  }
  mTransforms.reset();
  transforms.close();
}

void Tool::setTransformSaveFile(const std::string& filename)
{
  mInternalStructure.mTransformSaveFileName = filename;
}

ssc::Transform3D Tool::get_prMt() const
{
  return *m_prMt;
}

bool Tool::getVisible() const
{
  return mVisible;
}

/**
 * @return last recorded transform that will get you from tool- to patient ref space
 */
/*ssc::Transform3DPtr Tool::getLastTransform()
{
  ssc::Transform3DPtr lastTransform;
  if(mTransforms->size() > 0)
    lastTransform = mTransforms->at(mTransforms->size()-1);
  return lastTransform;
}*/

std::string Tool::getUid() const
{
  return ssc::Tool::mUid;
}

std::string Tool::getName() const
{
  return ssc::Tool::mName;
}

bool Tool::isCalibrated() const
{
  //TODO: What do we want to do here?
  return true;
}

double Tool::getTooltipOffset() const
{
  return mToolTipOffset;
}

void Tool::setTooltipOffset(double val)
{
  if (ssc::similar(val, mToolTipOffset))
    return;
  mToolTipOffset = val;
  emit tooltipOffset(mToolTipOffset);
}
Tool::TrackerToolType* Tool::getPointer() const
{
  return mTool;
}

bool Tool::isValid() const
{
  return mValid;
}

//copied from ManualTool, move to ssc::Tool?
void Tool::set_prMt(const ssc::Transform3D& prMt)
{
  QDateTime time;
  double timestamp = (double) time.time().msec();

  //QMutexLocker locker(&mMutex);
  m_prMt = ssc::Transform3DPtr(new ssc::Transform3D(prMt));
  //locker.unlock();

  emit toolTransformAndTimestamp( prMt, timestamp );
}

void Tool::toolTransformCallback(const itk::EventObject &event)
{
  if(igstk::CoordinateSystemTransformToEvent().CheckEvent(&event))
  {
    const igstk::CoordinateSystemTransformToEvent *transformEvent;
    transformEvent=dynamic_cast<const igstk::CoordinateSystemTransformToEvent*>( &event );
    if(!transformEvent)
      return;

    igstk::CoordinateSystemTransformToResult result = transformEvent->Get();
    igstk::Transform transform = result.GetTransform();
    if(transform.IsIdentity())
      return;
    if(!transform.IsValidNow())
    {
      //What to do? this happens alot, dunno why. Ignore? Seems to work ok.
      //TODO CA20100901: Probable cause: we work on the main (render) thread. This causes several hundred ms lag. Move IGSTK+Toolmanager internals to separate thread.
    }
    if (!mVisible)
      return; // quickfix replacement for IsValidNow()

    const igstk::CoordinateSystem* destination = result.GetDestination();
    ssc::ToolPtr refTool = ssc::toolManager()->getReferenceTool();

    if(refTool) //if we are tracking with a reftool it must be visible
    {
      //std::cout << "Checking that the incoming transforms destiantion is the referenceTOOL." << std::endl;
      ssc::Tool* tool = refTool.get();
      Tool* ref = dynamic_cast<Tool*>(tool);
      if(!ref->getPointer()->IsCoordinateSystem(destination))
        return;
      //std::cout << "RefTool is the destiantion." << std::endl;
    }else //if we dont have a reftool we use the tracker as patientref
    {
      //std::cout << "Checking that the incoming transforms destiantion is the TRACKER." << std::endl;
      TrackerPtr tracker = ToolManager::getInstance()->getTracker();
      if(!tracker || !tracker->getPointer()->IsCoordinateSystem(destination))
        return;
      //std::cout << "Tracker is the destiantion." << std::endl;
    }

    vtkMatrix4x4Ptr vtkMatrix =  vtkMatrix4x4Ptr::New();
    transform.ExportTransform(*vtkMatrix.GetPointer());

    const ssc::Transform3D prMt(vtkMatrix.GetPointer()); //prMt, transform from tool to patientref
    //ssc::Transform3D rMpr = *((ToolManager::getInstance()->get_rMpr()).get()); //rMpr, transform from patientref to global ref
    //ssc::Transform3D rMt = rMpr * prMt; //rMt, transform from tool to global ref
    double timestamp = transform.GetStartTime();

    m_prMt = ssc::Transform3DPtr(new ssc::Transform3D(prMt));

    mTransforms->push_back(m_prMt);
    mTimestamps->push_back(timestamp);


    emit toolTransformAndTimestamp((*m_prMt), timestamp);
    emit toolReport(TOOL_COORDINATESYSTEM_TRANSFORM, true, true, mUid);
    //ssc::messageManager()->sendInfo("Tool: "+mUid+" received a coordinatesystem transform."); //SPAM???
  }
  //Successes
  else if (igstk::TrackerToolConfigurationEvent().CheckEvent(&event))
  {
    mConfigured = true;
    emit toolReport(TOOL_HW_CONFIGURED, true, true, mUid);
    ssc::messageManager()->sendInfo("Tool: "+mUid+" is configured with the tracking system.");
  }
  else if (igstk::TrackerToolAttachmentToTrackerEvent().CheckEvent(&event))
  {
    mAttachedToTracker = true;
    emit toolReport(TOOL_ATTACHED_TO_TRACKER, true, true, mUid);
    ssc::messageManager()->sendInfo("Tool: "+mUid+" is attached to the tracker.");
  }
  else if (igstk::TrackerToolDetachmentFromTrackerEvent().CheckEvent(&event))
  {
    mAttachedToTracker = false;
    emit toolReport(TOOL_ATTACHED_TO_TRACKER, false, true, mUid);
    ssc::messageManager()->sendInfo("Tool: "+mUid+" is detached from the tracker.");
  }
  else if (igstk::TrackerToolMadeTransitionToTrackedStateEvent().CheckEvent(&event))
  {
    mVisible = true;
    emit toolReport(TOOL_VISIBLE, true, true, mUid);
    emit toolVisible(true); //signal inherited from ssc::Tool
    //ssc::messageManager()->sendInfo("Tool: "+mUid+" is visible."); //SPAM
  }
  else if (igstk::TrackerToolNotAvailableToBeTrackedEvent().CheckEvent(&event))
  {
    mVisible = false;
    emit toolReport(TOOL_VISIBLE, false, true, mUid);
    emit toolVisible(false); // signal inherited from ssc::Tool
    //ssc::messageManager()->sendInfo("Tool: "+mUid+" is hidden."); //SPAM
  }
  else if (igstk::ToolTrackingStartedEvent().CheckEvent(&event))
  {
    mTracked = true;
    emit toolReport(TOOL_TRACKED, true, true, mUid);
    ssc::messageManager()->sendInfo("Tool: "+mUid+" is tracked.");
  }
  else if (igstk::ToolTrackingStoppedEvent().CheckEvent(&event))
  {
    mTracked = false;
    emit toolReport(TOOL_TRACKED, false, true, mUid);
    ssc::messageManager()->sendInfo("Tool: "+mUid+" is not tracked anymore.");
  }
  //Failures
  else if (igstk::InvalidRequestErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_INVALID_REQUEST, false, true, mUid);
    ssc::messageManager()->sendWarning("Tool: "+mUid+" received an invalid request.");
  }
  else if (igstk::TrackerToolConfigurationErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_HW_CONFIGURED, true, false, mUid);
    ssc::messageManager()->sendError("Tool: "+mUid+" could not configure with the tracking system.");
  }
  else if (igstk::InvalidRequestToAttachTrackerToolErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_INVALID_REQUEST, true, false, mUid);
    ssc::messageManager()->sendError("Tool: "+mUid+" could not request to attach to tracker.");
  }
  else if (igstk::InvalidRequestToDetachTrackerToolErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_INVALID_REQUEST, false, false, mUid);
    ssc::messageManager()->sendError("Tool: "+mUid+" could not request to detach from tracker.");
  }
  else if (igstk::TrackerToolAttachmentToTrackerErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_ATTACHED_TO_TRACKER, true, false, mUid);
    ssc::messageManager()->sendError("Tool: "+mUid+" could not attach to tracker.");
  }
  else if (igstk::TrackerToolDetachmentFromTrackerErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_ATTACHED_TO_TRACKER, false, false, mUid);
    ssc::messageManager()->sendError("Tool: "+mUid+" could not detach from tracker.");
  }
  //Polaris specific failures
  else if (igstk::InvalidPolarisPortNumberErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_NDI_PORT_NUMBER, true, false, mUid);
    ssc::messageManager()->sendError("Polaris tool: "+mUid+" sendt invalid Polaris port number: "+ string_cast(mInternalStructure.mPortNumber) +".");
  }
  else if (igstk::InvalidPolarisSROMFilenameErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_NDI_SROM_FILENAME, true, false, mUid);
    ssc::messageManager()->sendError("Polaris tool: "+mUid+" sendt invalid ROM file: "+mInternalStructure.mSROMFilename);
  }
  else if (igstk::InvalidPolarisPartNumberErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_NDI_PART_NUMBER, true, false, mUid);
    ssc::messageManager()->sendError("Polaris tool: "+mUid+" has an invalid part number.");
  }
  //Aurora specific failures
  else if (igstk::InvalidAuroraPortNumberErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_NDI_PORT_NUMBER, true, false, mUid);
    ssc::messageManager()->sendError("Aurora tool: "+mUid+" has an invalid port number: "+ string_cast(mInternalStructure.mPortNumber)+".");
  }
  else if (igstk::InvalidAuroraSROMFilenameErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_NDI_SROM_FILENAME, true, false, mUid);
    ssc::messageManager()->sendError("Aurora tool: "+mUid+" sendt invalid ROM file: "+ mInternalStructure.mSROMFilename);
  }
  else if (igstk::InvalidAuroraPartNumberErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_NDI_PART_NUMBER, true, false, mUid);
    ssc::messageManager()->sendError("Aurora tool: "+mUid+" has an invalid part number.");
  }
  else if (igstk::InvalidAuroraChannelNumberErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_AURORA_CHANNEL_NUMBER, true, false, mUid);
    ssc::messageManager()->sendError("Polaris tool: "+mUid+" has an invalid channel number:"+string_cast(mInternalStructure.mChannelNumber) +".");
  }
}

bool Tool::verifyInternalStructure()
{
  if(mInternalStructure.mType == ssc::Tool::TOOL_NONE)
  {
    std::cout << "if(mInternalStructure.mType == ssc::Tool::TOOL_NONE)" << std::endl;
    return false;
  }
  if(mInternalStructure.mUid.empty())
  {
    std::cout << "" << std::endl;
    return false;
  }
  if(mInternalStructure.mTrackerType == Tracker::TRACKER_NONE)
  {
    std::cout << "if(mInternalStructure.mTrackerType == Tracker::TRACKER_NONE)" << std::endl;
    return false;
  }
  if((mInternalStructure.mTrackerType == Tracker::TRACKER_AURORA) && (mInternalStructure.mPortNumber >= 4))
  {
    std::cout << "if(mInternalStructure.mPortNumber >= 4)" << std::endl;
    return false;
  }
  if((mInternalStructure.mTrackerType == Tracker::TRACKER_AURORA) && (mInternalStructure.mChannelNumber >= 1))
  {
    std::cout << "if(mInternalStructure.mChannelNumber >= 1)" << std::endl;
    return false;
  }
  QDir dir;
  if(!mInternalStructure.mSROMFilename.empty() && !dir.exists(QString(mInternalStructure.mSROMFilename.c_str())))
  {
    std::cout << "if(!dir.exists(QString(mInternalStructure.mSROMFilename.c_str())))" << std::endl;
    std::cout << "mInternalStructure.mSROMFilename: " << mInternalStructure.mSROMFilename.c_str() << std::endl;
    return false;
  }
  if(!mInternalStructure.mCalibrationFilename.empty() && !dir.exists(QString(mInternalStructure.mCalibrationFilename.c_str())))
  {
    std::cout << "if(!dir.exists(QString(mInternalStructure.mCalibrationFilename.c_str())))" << std::endl;
    return false;
  }
  if(!mInternalStructure.mTransformSaveFileName.empty() && !dir.exists(QString(mInternalStructure.mTransformSaveFileName.c_str())))
  {
    std::cout << "if(!dir.exists(QString(mInternalStructure.mTransformSaveFileName.c_str())))" << std::endl;
    return false;
  }
  if(!mInternalStructure.mLoggingFolderName.empty() && !dir.exists(QString(mInternalStructure.mLoggingFolderName.c_str())))
  {
    std::cout << "if(!dir.exists(QString(mInternalStructure.mLoggingFolderName.c_str())))" << std::endl;
    return false;
  }

  return true;
}

Tool::TrackerToolType* Tool::buildInternalTool()
{
  TrackerToolType* tool = NULL;

  switch (mInternalStructure.mTrackerType)
  {
  case Tracker::TRACKER_NONE:
    break;
  case Tracker::TRACKER_POLARIS_SPECTRA:
  case Tracker::TRACKER_POLARIS_VICRA:
  case Tracker::TRACKER_POLARIS:
    mTempPolarisTool = PolarisTrackerToolType::New();
    //this->addLogging(mTempPolarisTool);
    mTempPolarisTool->AddObserver(igstk::IGSTKEvent(), mToolObserver);
    if(!mInternalStructure.mWireless) //we only support wireless atm
      return tool = mTempPolarisTool.GetPointer();
    mTempPolarisTool->RequestSelectWirelessTrackerTool();
    mTempPolarisTool->RequestSetSROMFileName(mInternalStructure.mSROMFilename);
    mTempPolarisTool->RequestConfigure();
    mTempPolarisTool->SetCalibrationTransform(mCalibrationTransform);
    tool = mTempPolarisTool.GetPointer();
    break;
  case Tracker::TRACKER_AURORA:
    mTempAuroraTool = AuroraTrackerToolType::New();
    //this->addLogging(mTempAuroraTool);
    mTempAuroraTool->AddObserver(igstk::IGSTKEvent(), mToolObserver);
    if(mInternalStructure.m5DOF)
    {
      mTempAuroraTool->RequestSelect5DOFTrackerTool();
      mTempAuroraTool->RequestSetPortNumber(mInternalStructure.mPortNumber);
      mTempAuroraTool->RequestSetChannelNumber(mInternalStructure.mChannelNumber);
    }
    else
    {
      mTempAuroraTool->RequestSelect6DOFTrackerTool();
      mTempAuroraTool->RequestSetPortNumber(mInternalStructure.mPortNumber);
    }
    mTempAuroraTool->RequestConfigure();
    mTempAuroraTool->SetCalibrationTransform(mCalibrationTransform);
    tool = mTempAuroraTool.GetPointer();
    break;
  case Tracker::TRACKER_MICRON:
    //TODO: implement
    break;
  default:
    break;
  }
  return tool;
}

void Tool::createPolyData()
{
  QDir dir;
  if(!mInternalStructure.mGraphicsFileName.empty() && dir.exists(QString(mInternalStructure.mGraphicsFileName.c_str())))
  {
    vtkSTLReaderPtr reader = vtkSTLReader::New();
    reader->SetFileName(mInternalStructure.mGraphicsFileName.c_str());
    mPolyData = reader->GetOutput();
  }
  else
  {
    vtkConeSourcePtr coneSource = vtkConeSource::New();
    coneSource->SetResolution(25);
    coneSource->SetRadius(10);
    coneSource->SetHeight(100);

    coneSource->SetDirection(0,0,1);
    double newCenter[3];
    coneSource->GetCenter(newCenter);
    newCenter[2] = newCenter[2] - coneSource->GetHeight()/2;
    coneSource->SetCenter(newCenter);

    mPolyData = coneSource->GetOutput();
  }
}

void Tool::determineToolsCalibration()
{
  itk::Matrix<double, 3, 3> calMatrix;
  itk::Versor<double> rotation;
  itk::Vector<double, 3> translation;

  /* File must be in the form
   * rot_00 rot_01 rot_02 trans_0
   * rot_10 rot_11 rot_12 trans_1
   * rot_20 rot_21 rot_22 trans_2
   */
  std::ifstream inputStream;
  inputStream.open(mInternalStructure.mCalibrationFilename.c_str());
  if(inputStream.is_open())
  {
    std::string line;
    int lineNumber = 0;
    while(!inputStream.eof() && lineNumber<3)
    {
      getline(inputStream, line);

      for(int i = 0; i<4; i++)
      {
        //Tolerating more than one blank space between numbers
        while(line.find(" ") == 0)
        {
          line.erase(0,1);
        }
        std::string::size_type pos = line.find(" ");
        std::string str;
        if(pos != std::string::npos)
        {
          str = line.substr(0, pos);
        }
        else
        {
          str = line;
        }
        double d = atof(str.c_str());
        if(i<3)
        {
          calMatrix(lineNumber, i) = d;
        }
        if(i == 3)
        {
          translation.SetElement(lineNumber, d);
        }
        line.erase(0, pos);
      }
      lineNumber++;
    }
    rotation.Set(calMatrix);
    mCalibrationTransform.SetTranslationAndRotation(translation, rotation, 1.0,
                                    igstk::TimeStamp::GetLongestPossibleTime());
  }
  inputStream.close();
}

ssc::Transform3D Tool::getCalibration_sMt() const
{
	ssc::Transform3D sMt;
	mCalibrationTransform.ExportTransform(*(sMt.matrix().GetPointer()));
	return sMt;
}

void Tool::addLogging(TrackerToolType* trackerTool)
{
  std::ofstream* loggerFile = new std::ofstream();
  std::string logFile = mInternalStructure.mLoggingFolderName + "Tool_" + mName +"_Logging.txt";
  loggerFile->open( logFile.c_str() );
  mLogger = igstk::Logger::New();
  mLogOutput = itk::StdStreamLogOutput::New();
  mLogOutput->SetStream(*loggerFile);
  mLogger->AddLogOutput(mLogOutput);
  mLogger->SetPriorityLevel(itk::Logger::DEBUG);

  trackerTool->SetLogger(mLogger);
}

void Tool::printInternalStructure()
{
  std::cout << "------------------------------------------------------------------" << std::endl;
  std::cout << "mType: " << mInternalStructure.mType << std::endl;
  std::cout << "mName: " << mInternalStructure.mName << std::endl;
  std::cout << "mUid: " << mInternalStructure.mUid << std::endl;
  std::cout << "mTrackerType: " << mInternalStructure.mTrackerType << std::endl;
  std::cout << "mSROMFilename: " << mInternalStructure.mSROMFilename << std::endl;
  std::cout << "mPortNumber: " << mInternalStructure.mPortNumber << std::endl;
  std::cout << "mChannelNumber: " << mInternalStructure.mChannelNumber << std::endl;
  std::cout << "mWireless: " << mInternalStructure.mWireless << std::endl;
  std::cout << "m5DOF: " << mInternalStructure.m5DOF << std::endl;
  std::cout << "mCalibrationFilename: " << mInternalStructure.mCalibrationFilename << std::endl;
  std::cout << "mGraphicsFileName: " << mInternalStructure.mGraphicsFileName << std::endl;
  std::cout << "mTransformSaveFileName: " << mInternalStructure.mTransformSaveFileName << std::endl;
  std::cout << "mLoggingFolderName: " << mInternalStructure.mLoggingFolderName  << std::endl;
  std::cout << "------------------------------------------------------------------" << std::endl;
}
  
ssc::ProbeSector Tool::getProbeSector() const
{ 
  return mProbeSector;
}
void Tool::setUSProbeSector(ssc::ProbeSector probeSector)
{
  mProbeSector = probeSector;
}
  
std::string Tool::getInstrumentId() const
{
  return mInternalStructure.mInstrumentId;
}
std::string Tool::getInstrumentScannerId() const
{
  return mInternalStructure.mInstrumentScannerId;
}

QStringList Tool::getUSSectorConfigList() const
{
  QStringList rtSourceList = mXml->getRtSourceList(qstring_cast(this->getInstrumentScannerId()),
      qstring_cast(this->getInstrumentId()));
  QStringList configIdList = mXml->getConfigIdList(qstring_cast(this->getInstrumentScannerId()),
      qstring_cast(this->getInstrumentId()), rtSourceList.at(0));
  return configIdList;
}

QString Tool::getProbeSectorConfigurationString() const
{
  return mProbeSectorConfiguration;
}

void Tool::setProbeSectorConfigurationString(QString configString)
{
  QStringList rtSourceList = mXml->getRtSourceList(qstring_cast(this->getInstrumentScannerId()),
      qstring_cast(this->getInstrumentId()));
  if(rtSourceList.isEmpty())
    return;
  ProbeXmlConfigParser::Configuration config = mXml->getConfiguration(qstring_cast(this->getInstrumentScannerId()),
      qstring_cast(this->getInstrumentId()), rtSourceList.at(0), configString);
  if(config.isEmpty())
    return;
  double depthStart = config.mOffset;
  double depthEnd = config.mDepth + depthStart;
  if (config.mWidthDeg > 0.1) // Sector probe
  {
    double width = config.mWidthDeg * M_PI / 180.0;//width in radians
    ssc::ProbeSector probeSector = ssc::ProbeSector(ssc::ProbeSector::tSECTOR, depthStart, depthEnd, width);
    this->setUSProbeSector(probeSector);
  }
  else //Linear probe
  {
    int widtInPixels = config.mRightEdge - config.mLeftEdge;
    double width = config.mPixelWidth * widtInPixels; //width in mm
    ssc::ProbeSector probeSector = ssc::ProbeSector(ssc::ProbeSector::tLINEAR, depthStart, depthEnd, width);
    this->setUSProbeSector(probeSector);
  }

  mProbeSectorConfiguration = configString;
}

void Tool::addXml(QDomNode& dataNode)
{
  QDomDocument doc = dataNode.ownerDocument();
  dataNode.toElement().setAttribute("uid", qstring_cast(this->getUid()));
  if (!mProbeSectorConfiguration.isEmpty())
  {
    QDomElement configNode = doc.createElement("probeSectorConfiguration");
    configNode.appendChild(doc.createTextNode(mProbeSectorConfiguration));
    dataNode.appendChild(configNode);
  }
}

void Tool::parseXml(QDomNode& dataNode)
{
  if (dataNode.isNull())
    return;
  mProbeSectorConfiguration = dataNode.namedItem("probeSectorConfiguration").toElement().text();
  //Need to call set function to make sure the values will be applied
  setProbeSectorConfigurationString(mProbeSectorConfiguration);
  emit probeSectorConfigurationChanged();
}

}//namespace cx
