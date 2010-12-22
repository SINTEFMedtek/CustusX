#define _USE_MATH_DEFINES
#include "cxTool.h"

#include <vtkPolyData.h>
#include <vtkConeSource.h>
#include <vtkSTLReader.h>
#include <QDir>
#include <QDateTime>
#include <QStringList>
#include <QTextStream>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscProbeSector.h"
#include "cxToolManager.h"
#include "cxDataLocations.h"
#include "cxCreateProbeDataFromConfiguration.h"

namespace cx
{

Tool::Tool(InternalStructure& internalStructure) :
  mInternalStructure(internalStructure),
  mValid(false),
  mTool(NULL),
  mToolObserver(ObserverType::New()),
//  mTransforms(new Transform3DVector()),
//  mTimestamps(new DoubleVector()),
  mPositionHistory(new ssc::TimedTransformMap()),
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

  QStringList configs = this->getUSSectorConfigList();
  if (!configs.isEmpty())
    this->setProbeSectorConfigIdString(configs[0]);
}

Tool::~Tool()
{}

ssc::Tool::Type Tool::getType() const
{
  return mInternalStructure.mType;
}

QString Tool::getGraphicsFileName() const
{
  return mInternalStructure.mGraphicsFileName;
}

vtkPolyDataPtr Tool::getGraphicsPolyData() const
{
  return mPolyData;
}

ssc::TimedTransformMapPtr Tool::getPositionHistory()
{
  return mPositionHistory;
}

//void Tool::saveTransformsAndTimestamps()
//{
//  if(this->getType() == Tool::TOOL_REFERENCE)
//    return;  //we don't save transforms and timestamps for reference tools
//
//  QDateTime dateTime = QDateTime::currentDateTime();
//  QString stamp = dateTime.toString(QString("ddMMyyhhmmss"));
//
//  std::stringstream timestampsName;
//  std::stringstream transformsName;
//  timestampsName << mInternalStructure.mTransformSaveFileName
//  << ssc::Tool::mName << "_" << stamp << "_timestamps.txt";
//  transformsName << mInternalStructure.mTransformSaveFileName
//  << ssc::Tool::mName << "_" << stamp << "_transforms.txt";
//
//  //Save the timestamps
//  std::ofstream timestamps;
//  timestamps.open(timestampsName.str().c_str());
//  timestamps << std::fixed;
//  timestamps << mTimestamps->size();
//  timestamps << std::endl;
//  DoubleVector::iterator it1 = mTimestamps->begin();
//  while(it1 != mTimestamps->end())
//  {
//    timestamps << (*it1) << " ";
//    timestamps << std::endl;
//    it1++;
//  }
//  mTimestamps.reset();
//  timestamps.close();
//
//  //Save the transforms
//  std::ofstream transforms;
//  transforms.open(transformsName.str().c_str());
//  transforms << mTransforms->size();
//  transforms << std::endl;
//  Transform3DVector::iterator it2 = mTransforms->begin();
//  while(it2 != mTransforms->end())
//  {
//    for(int row=0; row<3; row++)
//    {
//      transforms << (*it2)->matrix()->GetElement(row, 0) << " ";
//      transforms << (*it2)->matrix()->GetElement(row, 1) << " ";
//      transforms << (*it2)->matrix()->GetElement(row, 2) << " ";
//      transforms << (*it2)->matrix()->GetElement(row, 3);
//      transforms << std::endl;
//    }
//    transforms << std::endl;
//    it2++;
//  }
//  mTransforms.reset();
//  transforms.close();
//}
//
//void Tool::setTransformSaveFile(const QString& filename)
//{
//  mInternalStructure.mTransformSaveFileName = filename;
//}

ssc::Transform3D Tool::get_prMt() const
{
  return *m_prMt;
}

bool Tool::getVisible() const
{
  return mVisible;
}

QString Tool::getUid() const
{
  return ssc::Tool::mUid;
}

QString Tool::getName() const
{
  return ssc::Tool::mName;
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
      ssc::Tool* tool = refTool.get();
      Tool* ref = dynamic_cast<Tool*>(tool);
      if(!ref->getPointer()->IsCoordinateSystem(destination))
        return;
    }else //if we dont have a reftool we use the tracker as patientref
    {
      TrackerPtr tracker = ToolManager::getInstance()->getTracker();
      if(!tracker || !tracker->getPointer()->IsCoordinateSystem(destination))
        return;
    }

    vtkMatrix4x4Ptr vtkMatrix =  vtkMatrix4x4Ptr::New();
    transform.ExportTransform(*vtkMatrix.GetPointer());

    const ssc::Transform3D prMt(vtkMatrix.GetPointer()); //prMt, transform from tool to patientref
    double timestamp = transform.GetStartTime();

    m_prMt = ssc::Transform3DPtr(new ssc::Transform3D(prMt));

    (*mPositionHistory)[timestamp] = *m_prMt;
//    mTransforms->push_back(m_prMt);
//    mTimestamps->push_back(timestamp);


    emit toolTransformAndTimestamp((*m_prMt), timestamp);
  }
  //Successes
  else if (igstk::TrackerToolConfigurationEvent().CheckEvent(&event))
  {
    this->internalConfigured(true);
    ssc::messageManager()->sendInfo("Tool: "+mUid+" is successfully configured with the tracking system.");
  }
  else if (igstk::TrackerToolAttachmentToTrackerEvent().CheckEvent(&event))
  {
    this->internalAttachedToTracker(true);
    ssc::messageManager()->sendInfo("Tool: "+mUid+" is attached to the tracker.");
  }
  else if (igstk::TrackerToolDetachmentFromTrackerEvent().CheckEvent(&event))
  {
    this->internalAttachedToTracker(false);
    ssc::messageManager()->sendInfo("Tool: "+mUid+" is detached from the tracker.");
  }
  else if (igstk::TrackerToolMadeTransitionToTrackedStateEvent().CheckEvent(&event))
  {
    this->internalVisible(true);
    //ssc::messageManager()->sendInfo("Tool: "+mUid+" is visible."); //SPAM
  }
  else if (igstk::TrackerToolNotAvailableToBeTrackedEvent().CheckEvent(&event))
  {
    this->internalVisible(false);
    //ssc::messageManager()->sendInfo("Tool: "+mUid+" is hidden."); //SPAM
  }
  else if (igstk::ToolTrackingStartedEvent().CheckEvent(&event))
  {
    this->internalTracked(true);
    ssc::messageManager()->sendSuccess("Tool: "+mUid+" is tracked.");
  }
  else if (igstk::ToolTrackingStoppedEvent().CheckEvent(&event))
  {
    this->internalTracked(false);
    ssc::messageManager()->sendInfo("Tool: "+mUid+" is not tracked anymore.");
  }
  //Failures
  else if (igstk::InvalidRequestErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendWarning("Tool: "+mUid+" received an invalid request.  This means that the internal igstk trackertool did not accept the request. Do not know which request.");
  }
  else if (igstk::TrackerToolConfigurationErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tool: "+mUid+" could not configure with the tracking system.");
  }
  else if (igstk::InvalidRequestToAttachTrackerToolErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tool: "+mUid+" could not request to attach to tracker.");
  }
  else if (igstk::InvalidRequestToDetachTrackerToolErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tool: "+mUid+" could not request to detach from tracker.");
  }
  else if (igstk::TrackerToolAttachmentToTrackerErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tool: "+mUid+" could not attach to tracker.");
  }
  else if (igstk::TrackerToolDetachmentFromTrackerErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tool: "+mUid+" could not detach from tracker.");
  }
  //Polaris specific failures
  else if (igstk::InvalidPolarisPortNumberErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Polaris tool: "+mUid+" sendt invalid Polaris port number: "+ qstring_cast(mInternalStructure.mPortNumber) +".");
  }
  else if (igstk::InvalidPolarisSROMFilenameErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Polaris tool: "+mUid+" sendt invalid ROM file: "+mInternalStructure.mSROMFilename);
  }
  else if (igstk::InvalidPolarisPartNumberErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Polaris tool: "+mUid+" has an invalid part number.");
  }
  //Aurora specific failures
  else if (igstk::InvalidAuroraPortNumberErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Aurora tool: "+mUid+" has an invalid port number: "+ qstring_cast(mInternalStructure.mPortNumber)+".");
  }
  else if (igstk::InvalidAuroraSROMFilenameErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Aurora tool: "+mUid+" sendt invalid ROM file: "+ mInternalStructure.mSROMFilename);
  }
  else if (igstk::InvalidAuroraPartNumberErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Aurora tool: "+mUid+" has an invalid part number.");
  }
  else if (igstk::InvalidAuroraChannelNumberErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Polaris tool: "+mUid+" has an invalid channel number:"+qstring_cast(mInternalStructure.mChannelNumber) +".");
  }
}

bool Tool::verifyInternalStructure()
{
  QString verificationError("Internal verification of tool "+mUid+" failed! REASON: ");
  if(mInternalStructure.mType == ssc::Tool::TOOL_NONE)
  {
    ssc::messageManager()->sendError(verificationError+" Tag <tool>::<type> is invalid ["+qstring_cast(mInternalStructure.mType)+"]. Valid types: [pointer, usprobe, reference]");
    return false;
  }
  if(mInternalStructure.mUid.isEmpty())
  {
    ssc::messageManager()->sendError(verificationError+" Tag <tool>::<uid> is empty. Give tool a unique id.");
    return false;
  }
  if(mInternalStructure.mTrackerType == Tracker::TRACKER_NONE)
  {
    ssc::messageManager()->sendError(verificationError+" Tag <sensor>::<type> is invalid ["+qstring_cast(mInternalStructure.mTrackerType)+"]. Valid types: [polaris, spectra, vicra, aurora, micron (NOT SUPPORTED YET)]");
    return false;
  }
  if((mInternalStructure.mTrackerType == Tracker::TRACKER_AURORA) && (mInternalStructure.mPortNumber >= 4))
  {
    ssc::messageManager()->sendError(verificationError+" Tag <sensor>::<portnumber> is invalid ["+qstring_cast(mInternalStructure.mPortNumber)+"]. Valid numbers: [0, 1, 2, 3]");
    return false;
  }
  if((mInternalStructure.mTrackerType == Tracker::TRACKER_AURORA) && (mInternalStructure.mChannelNumber >= 1))
  {
    ssc::messageManager()->sendError(verificationError+" Tag <sensor>::<channelnumber> is invalid ["+qstring_cast(mInternalStructure.mChannelNumber)+"]. Valid numbers: [0, 1]");
    return false;
  }
  QDir dir;
  if(!mInternalStructure.mSROMFilename.isEmpty() && !dir.exists(mInternalStructure.mSROMFilename))
  {
    ssc::messageManager()->sendError(verificationError+" Tag <sensor>::<rom_file> is invalid ["+mInternalStructure.mSROMFilename+"]. Valid path: relative path to existing rom file.");
    return false;
  }
  if(!mInternalStructure.mCalibrationFilename.isEmpty() && !dir.exists(mInternalStructure.mCalibrationFilename))
  {
    ssc::messageManager()->sendError(verificationError+" Tag <calibration>::<cal_file> is invalid ["+mInternalStructure.mCalibrationFilename+"]. Valid path: relative path to existing calibration file.");
    return false;
  }
  if(!mInternalStructure.mTransformSaveFileName.isEmpty() && !dir.exists(mInternalStructure.mTransformSaveFileName))
  {
    ssc::messageManager()->sendError(verificationError+" Logging folder is invalid. Contact programmer! :)");
    return false;
  }
  if(!mInternalStructure.mLoggingFolderName.isEmpty() && !dir.exists(mInternalStructure.mLoggingFolderName))
  {
    ssc::messageManager()->sendError(verificationError+" Logging folder is invalid. Contact programmer! :)");
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
    this->addLogging(mTempPolarisTool);
    mTempPolarisTool->AddObserver(igstk::IGSTKEvent(), mToolObserver);
    if(!mInternalStructure.mWireless) //we only support wireless atm
      return tool = mTempPolarisTool.GetPointer();
    mTempPolarisTool->RequestSelectWirelessTrackerTool();
    mTempPolarisTool->RequestSetSROMFileName(string_cast(mInternalStructure.mSROMFilename));
    mTempPolarisTool->RequestConfigure();
    mTempPolarisTool->SetCalibrationTransform(mCalibrationTransform);
    tool = mTempPolarisTool.GetPointer();
    break;
  case Tracker::TRACKER_AURORA:
    mTempAuroraTool = AuroraTrackerToolType::New();
    this->addLogging(mTempAuroraTool);
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
  if(!mInternalStructure.mGraphicsFileName.isEmpty() && dir.exists(mInternalStructure.mGraphicsFileName))
  {
    vtkSTLReaderPtr reader = vtkSTLReaderPtr::New();
    reader->SetFileName(cstring_cast(mInternalStructure.mGraphicsFileName));
    mPolyData = reader->GetOutput();
  }
  else
  {
    vtkConeSourcePtr coneSource = vtkConeSourcePtr::New();
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
  inputStream.open(cstring_cast(mInternalStructure.mCalibrationFilename));
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

bool Tool::isCalibrated() const
{
  ssc::Transform3D identity;
  ssc::Transform3D sMt;
  mCalibrationTransform.ExportTransform(*(sMt.matrix().GetPointer()));
  return !ssc::similar(sMt, identity);
}

ssc::Transform3D Tool::getCalibration_sMt() const
{
  ssc::Transform3D sMt;
  mCalibrationTransform.ExportTransform(*(sMt.matrix().GetPointer()));
  return sMt;
}

void Tool::setCalibration_sMt(ssc::Transform3D calibration)
{
  //apply the calibration
  mCalibrationTransform.ImportTransform(*calibration.matrix());
  mTool->SetCalibrationTransform(mCalibrationTransform);

  ssc::Transform3D sMt;
  mCalibrationTransform.ExportTransform(*(sMt.matrix().GetPointer()));
  ssc::messageManager()->sendInfo("Set "+mName+"s calibration to \n"+qstring_cast(sMt));

  //write to file
  this->writeCalibrationToFile();
}

QString Tool::getCalibrationFileName() const
{
  return mInternalStructure.mCalibrationFilename;
}

Tracker::Type Tool::getTrackerType()
{
  return mInternalStructure.mTrackerType;
}

void Tool::addLogging(TrackerToolType* trackerTool)
{
  std::ofstream* loggerFile = new std::ofstream();
  QString logFile = mInternalStructure.mLoggingFolderName + "Tool_" + mName +"_Logging.txt";
  loggerFile->open( cstring_cast(logFile) );
  mLogger = igstk::Logger::New();
  mLogOutput = itk::StdStreamLogOutput::New();
  mLogOutput->SetStream(*loggerFile);
  mLogger->AddLogOutput(mLogOutput);
  mLogger->SetPriorityLevel(itk::Logger::DEBUG);

  trackerTool->SetLogger(mLogger);
}

void Tool::internalAttachedToTracker(bool value)
{
  if(mAttachedToTracker == value)
    return;
  mAttachedToTracker = value;
  emit attachedToTracker(mAttachedToTracker);
}

void Tool::internalTracked(bool value)
{
  if(mTracked == value)
    return;
  mTracked = value;
}

void Tool::internalConfigured(bool value)
{
  if(mConfigured == value)
    return;
  mConfigured = value;
}

void Tool::internalVisible(bool value)
{
  if(mVisible == value)
    return;
  mVisible = value;
  emit toolVisible(mVisible);
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
  std::cout << "mReferencePoints: " << string_cast(mInternalStructure.mReferencePoints.size()) << std::endl;
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
  emit toolProbeSector();
}
  
QString Tool::getInstrumentId() const
{
  return mInternalStructure.mInstrumentId;
}
QString Tool::getInstrumentScannerId() const
{
  return mInternalStructure.mInstrumentScannerId;
}

QStringList Tool::getUSSectorConfigList() const
{
  QStringList rtSourceList = mXml->getRtSourceList(this->getInstrumentScannerId(), this->getInstrumentId());
  if (rtSourceList.empty())
    return QStringList();
  QStringList configIdList = mXml->getConfigIdList(this->getInstrumentScannerId(), this->getInstrumentId(), rtSourceList.at(0));
  return configIdList;
}

QString Tool::getNameOfProbeSectorConfigId(QString configString) ///< get a name for the given configuration
{
  QStringList rtSourceList = mXml->getRtSourceList(this->getInstrumentScannerId(), this->getInstrumentId());
  if(rtSourceList.isEmpty())
    return "";
  ProbeXmlConfigParser::Configuration config = mXml->getConfiguration(this->getInstrumentScannerId(), this->getInstrumentId(), rtSourceList.at(0), configString);
  return config.mName;
}

QString Tool::getProbeSectorConfigIdString() const
{
  return mProbeSectorConfiguration;
}
QString Tool::getConfigurationString() const
{
  QStringList rtSourceList = mXml->getRtSourceList(this->getInstrumentScannerId(), this->getInstrumentId());
    if(rtSourceList.isEmpty())
      return "";
  QString retval = this->getInstrumentScannerId() + ":" + this->getInstrumentId()
      + ":" + rtSourceList.at(0) + ":" + this->getProbeSectorConfigIdString();
  return retval;
}

void Tool::setProbeSectorConfigIdString(QString configString)
{
  QStringList rtSourceList = mXml->getRtSourceList(this->getInstrumentScannerId(), this->getInstrumentId());
  if(rtSourceList.isEmpty())
    return;
  ProbeXmlConfigParser::Configuration config = mXml->getConfiguration(this->getInstrumentScannerId(), this->getInstrumentId(), rtSourceList.at(0), configString);
  if(config.isEmpty())
    return;

  ssc::ProbeSector probeSector = createProbeDataFromConfiguration(config);
//  std::cout << "setting probe settings data" << std::endl;
  mProbeSectorConfiguration = configString;
  this->setUSProbeSector(probeSector);
}

ProbeXmlConfigParser::Configuration Tool::getConfiguration() const
{
  ProbeXmlConfigParser::Configuration config;
  QStringList rtSourceList = mXml->getRtSourceList(this->getInstrumentScannerId(), this->getInstrumentId());
  if(rtSourceList.isEmpty())
    return config;
  config = mXml->getConfiguration(this->getInstrumentScannerId(), this->getInstrumentId(), rtSourceList.at(0), this->getProbeSectorConfigIdString());
  return config;
}

std::map<int, ssc::Vector3D> Tool::getReferencePoints() const
{
  return mInternalStructure.mReferencePoints;
}

bool Tool::hasReferencePointWithId(int id)
{
  bool retval = false;
  if(!(ssc::similar(this->getReferencePoints()[id], ssc::Vector3D(0.000,0.000,0.000))))
      retval = true;
  return retval;
}

ssc::TimedTransformMap Tool::getSessionHistory(double startTime, double stopTime)
{
  ssc::TimedTransformMap::iterator startIt = mPositionHistory->lower_bound(startTime);
  ssc::TimedTransformMap::iterator stopIt = mPositionHistory->upper_bound(stopTime);

  ssc::TimedTransformMap retval(startIt, stopIt);;
  return retval;
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
  if (mProbeSectorConfiguration.isEmpty())
    return;
  setProbeSectorConfigIdString(mProbeSectorConfiguration);
  emit probeSectorConfigurationChanged();
}

void Tool::writeCalibrationToFile()
{
  QFile calibrationFile(this);
  if(!mInternalStructure.mCalibrationFilename.isEmpty() && QFile::exists(mInternalStructure.mCalibrationFilename))
  {
    //Calibration file exists, overwrite
    calibrationFile.setFileName(mInternalStructure.mCalibrationFilename);
  }
  else
  {
    //Make a new file, use rom file name as base name
    QString calibrationFileName = mInternalStructure.mSROMFilename.remove(".rom", Qt::CaseInsensitive);
    calibrationFileName.append(".cal");
    calibrationFile.setFileName(calibrationFileName);
  }
  ssc::Transform3D sMt;
  mCalibrationTransform.ExportTransform(*(sMt.matrix().GetPointer()));

  if(!calibrationFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    ssc::messageManager()->sendError("Could not open "+mUid+"s calibrationfile: "+calibrationFile.fileName());
    return;
  }

  QTextStream streamer(&calibrationFile);
  streamer << qstring_cast(sMt);
  streamer << endl;

  calibrationFile.close();

  ssc::messageManager()->sendInfo("Replaced calibration in "+calibrationFile.fileName());
}


}//namespace cx
