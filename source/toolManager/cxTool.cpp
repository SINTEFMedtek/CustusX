#include "cxTool.h"

#include <vtkPolyData.h>
#include <vtkConeSource.h>
#include <vtkSTLReader.h>
#include <QDir>
#include <QDateTime>

#include "cxToolManager.h"

namespace cx
{

Tool::Tool(InternalStructure internalStructure) :
  mInternalStructure(internalStructure),
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
  mToolTipOffset(0)
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
  }
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
  QDateTime dateTime = QDateTime::currentDateTime();
  std::string stamp = dateTime.toString(QString("ddMMyyhhmmss")).toStdString();

  std::stringstream timestampsName;
  std::stringstream transformsName;
  //timestampsName << mInternalStructure.mLoggingFolderName
  timestampsName << mInternalStructure.mTransformSaveFileName 
  << ssc::Tool::mName << "_" << stamp << "_timestamps.txt";
  //transformsName << mInternalStructure.mLoggingFolderName
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
  mToolTipOffset = val;
}
Tool::TrackerToolType* Tool::getPointer() const
{
  return mTool;
}
void Tool::toolTransformCallback(const itk::EventObject &event)
{
  if(igstk::CoordinateSystemTransformToEvent().CheckEvent(&event))
  {
    //Maybe we need to request this transform somewhere?

    const igstk::CoordinateSystemTransformToEvent *transformEvent;
    transformEvent=dynamic_cast
                  <const igstk::CoordinateSystemTransformToEvent*>( &event );
    if(!transformEvent)
      return;

    igstk::CoordinateSystemTransformToResult result = transformEvent->Get();
    igstk::Transform transform = result.GetTransform();
    if(transform.IsIdentity())
      return;
    if(!transform.IsValidNow())
    {
      //What to do? this happens alot, dunno why. Ignore? Seems to work ok.
    }

    const igstk::CoordinateSystem* destination = result.GetDestination();
    ssc::ToolPtr refTool = ToolManager::getInstance()->getReferenceTool();
    if(!refTool) //hmmm why is this done? seems meaningless...
    {            //its because we only request transforms from tool to reftool
      ssc::Tool* tool = refTool.get();
      Tool* ref = dynamic_cast<Tool*>(tool);
      if(!ref->getPointer()->IsCoordinateSystem(destination))
        return;
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
    //emit toolTransformAndTimestamp(rMt, timestamp);
    emit toolTransformAndTimestamp((*m_prMt), timestamp);
    emit toolReport(TOOL_COORDINATESYSTEM_TRANSFORM, true, true, mUid);
  }
  //Successes
  else if (igstk::TrackerToolConfigurationEvent().CheckEvent(&event))
  {
    mConfigured = true;
    emit toolReport(TOOL_HW_CONFIGURED, true, true, mUid);
  }
  else if (igstk::TrackerToolAttachmentToTrackerEvent().CheckEvent(&event))
  {
    mAttachedToTracker = true;
    emit toolReport(TOOL_ATTACHED_TO_TRACKER, true, true, mUid);
  }
  else if (igstk::TrackerToolDetachmentFromTrackerEvent().CheckEvent(&event))
  {
    mAttachedToTracker = false;
    emit toolReport(TOOL_ATTACHED_TO_TRACKER, false, true, mUid);
  }
  else if (igstk::TrackerToolMadeTransitionToTrackedStateEvent().CheckEvent(
      &event))
  {
    mVisible = true;
    emit toolReport(TOOL_VISIBLE, true, true, mUid);
    emit toolVisible(true); //signal inherited from ssc::Tool
  }
  else if (igstk::TrackerToolNotAvailableToBeTrackedEvent().CheckEvent(&event))
  {
    mVisible = false;
    emit toolReport(TOOL_VISIBLE, false, true, mUid);
    emit toolVisible(false); // signal inherited from ssc::Tool
  }
  else if (igstk::ToolTrackingStartedEvent().CheckEvent(&event))
  {
    mTracked = true;
    emit toolReport(TOOL_TRACKED, true, true, mUid);
  }
  else if (igstk::ToolTrackingStoppedEvent().CheckEvent(&event))
  {
    mTracked = false;
    emit toolReport(TOOL_TRACKED, false, true, mUid);
  }
  //Failures
  else if (igstk::InvalidRequestErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_INVALID_REQUEST, false, true, mUid);
  }
  else if (igstk::TrackerToolConfigurationErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_HW_CONFIGURED, true, false, mUid);
  }
  else if (igstk::InvalidRequestToAttachTrackerToolErrorEvent().CheckEvent(
      &event))
  {
    emit toolReport(TOOL_INVALID_REQUEST, true, false, mUid);
  }
  else if (igstk::InvalidRequestToDetachTrackerToolErrorEvent().CheckEvent(
      &event))
  {
    emit toolReport(TOOL_INVALID_REQUEST, false, false, mUid);
  }
  else if (igstk::TrackerToolAttachmentToTrackerErrorEvent().CheckEvent(
      &event))
  {
    emit toolReport(TOOL_ATTACHED_TO_TRACKER, true, false, mUid);
  }
  else if (igstk::TrackerToolDetachmentFromTrackerErrorEvent().CheckEvent(
      &event))
  {
    emit toolReport(TOOL_ATTACHED_TO_TRACKER, false, false, mUid);
  }
  //Polaris specific failures
  else if (igstk::InvalidPolarisPortNumberErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_NDI_PORT_NUMBER, true, false, mUid);
  }
  else if (igstk::InvalidPolarisSROMFilenameErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_NDI_SROM_FILENAME, true, false, mUid);
  }
  else if (igstk::InvalidPolarisPartNumberErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_NDI_PART_NUMBER, true, false, mUid);
  }
  //Aurora specific failures
  else if (igstk::InvalidAuroraPortNumberErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_NDI_PORT_NUMBER, true, false, mUid);
  }
  else if (igstk::InvalidAuroraSROMFilenameErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_NDI_SROM_FILENAME, true, false, mUid);
  }
  else if (igstk::InvalidAuroraPartNumberErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_NDI_PART_NUMBER, true, false, mUid);
  }
  else if (igstk::InvalidAuroraChannelNumberErrorEvent().CheckEvent(&event))
  {
    emit toolReport(TOOL_AURORA_CHANNEL_NUMBER, true, false, mUid);
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
  if(mInternalStructure.mPortNumber >= 4)
  {
    std::cout << "if(mInternalStructure.mPortNumber >= 4)" << std::endl;
    return false;
  }
  if(mInternalStructure.mChannelNumber >= 1)
  {
    std::cout << "if(mInternalStructure.mChannelNumber >= 1)" << std::endl;
    return false;
  }
  QDir dir;
  if(!mInternalStructure.mSROMFilename.empty() && !dir.exists(QString(mInternalStructure.mSROMFilename.c_str())))
  {
    std::cout << "if(!dir.exists(QString(mInternalStructure.mSROMFilename.c_str())))" << std::endl;
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
    this->addLogging(mTempPolarisTool);
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
}//namespace cx
