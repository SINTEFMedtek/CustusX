#include <cxIgstkTool.h>

#include <QDir>
#include <QDateTime>
#include <QStringList>
#include <QTextStream>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"

namespace cx
{

IgstkTool::IgstkTool(Tool::InternalStructure internalStructure) :
    mToolObserver(itk::ReceptorMemberCommand<IgstkTool>::New()),
    mValid(false),
    mVisible(false),
    mAttachedToTracker(false)
{
  qRegisterMetaType<Transform3D>("Transform3D");

  mInternalStructure = internalStructure;

  mToolObserver->SetCallbackFunction(this, &IgstkTool::toolTransformCallback);

  if(this->verifyInternalStructure())
  {
    mTool = this->buildInternalTool();
    this->addLogging();
    mValid = true;
  }else
  {
    ssc::messageManager()->sendError(mInternalStructure.mUid+" was created with invalid internal structure.");
    mValid = false;
  }
}

IgstkTool::~IgstkTool()
{}

Tool::InternalStructure IgstkTool::getInternalStructure()
{
  return mInternalStructure;
}

QString IgstkTool::getUid()
{
  return mInternalStructure.mUid;
}

igstk::TrackerTool::Pointer IgstkTool::getPointer() const
{
  return mTool;
}

IgstkTracker::Type IgstkTool::getTrackerType()
{
  return mInternalStructure.mTrackerType;
}

ssc::Tool::Type IgstkTool::getType() const
{
  return mInternalStructure.mType;
}

bool IgstkTool::isValid() const
{
  return mValid;
}

bool IgstkTool::isInitialized() const
{
  return mAttachedToTracker;
}

bool IgstkTool::isTracked() const
{
  return mTracked;
}

bool IgstkTool::isVisible() const
{
  return mVisible;
}

void IgstkTool::setReference(IgstkToolPtr refTool)
{
  mReferenceTool = refTool;
}

void IgstkTool::setTracker(TrackerPtr tracker)
{
  mTracker = tracker;
}

void IgstkTool::toolTransformCallback(const itk::EventObject &event)
{
  if(igstk::CoordinateSystemTransformToEvent().CheckEvent(&event))
  {
    const igstk::CoordinateSystemTransformToEvent *transformEvent;
    transformEvent = dynamic_cast<const igstk::CoordinateSystemTransformToEvent*>( &event );
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
    IgstkToolPtr strongReference = mReferenceTool.lock();

    if(strongReference) //if we are tracking with a reftool it must be visible
    {
      if(!strongReference->getPointer()->IsCoordinateSystem(destination))
        return;
    }else //if we dont have a reftool we use the tracker as patientref
    {
      TrackerPtr tracker = mTracker.lock();
      if(!tracker || !tracker->getPointer()->IsCoordinateSystem(destination))
        return;
    }

    vtkMatrix4x4Ptr vtkMatrix =  vtkMatrix4x4Ptr::New();
    transform.ExportTransform(*vtkMatrix.GetPointer());

    const ssc::Transform3D prMt(vtkMatrix.GetPointer()); //prMt, transform from tool to patientref
    double timestamp = transform.GetStartTime();

    emit toolTransformAndTimestamp(prMt, timestamp);
  }
  //Successes
  else if (igstk::TrackerToolConfigurationEvent().CheckEvent(&event))
  {
    //this->internalConfigured(true);
    ssc::messageManager()->sendInfo("Tool: "+mInternalStructure.mUid+" is successfully configured with the tracking system.");
  }
  else if (igstk::TrackerToolAttachmentToTrackerEvent().CheckEvent(&event))
  {
    this->internalAttachedToTracker(true);
  }
  else if (igstk::TrackerToolDetachmentFromTrackerEvent().CheckEvent(&event))
  {
    this->internalAttachedToTracker(false);
  }
  else if (igstk::TrackerToolMadeTransitionToTrackedStateEvent().CheckEvent(&event))
  {
    this->internalVisible(true);
    //ssc::messageManager()->sendInfo("Tool: "+mInternalStructure.mUid+" is visible."); //SPAM
  }
  else if (igstk::TrackerToolNotAvailableToBeTrackedEvent().CheckEvent(&event))
  {
    this->internalVisible(false);
    //ssc::messageManager()->sendInfo("Tool: "+mInternalStructure.mUid+" is hidden."); //SPAM
  }
  else if (igstk::ToolTrackingStartedEvent().CheckEvent(&event))
  {
    this->internalTracked(true);
    ssc::messageManager()->sendInfo("Tool: "+mInternalStructure.mUid+" is tracked.");
  }
  else if (igstk::ToolTrackingStoppedEvent().CheckEvent(&event))
  {
    this->internalTracked(false);
    ssc::messageManager()->sendInfo("Tool: "+mInternalStructure.mUid+" is not tracked anymore.");
  }
  //Failures
  else if (igstk::InvalidRequestErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendWarning("Tool: "+mInternalStructure.mUid+" received an invalid request.  This means that the internal igstk trackertool did not accept the request. Do not know which request.");
  }
  else if (igstk::TrackerToolConfigurationErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tool: "+mInternalStructure.mUid+" could not configure with the tracking system.");
  }
  else if (igstk::InvalidRequestToAttachTrackerToolErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tool: "+mInternalStructure.mUid+" could not request to attach to tracker.");
  }
  else if (igstk::InvalidRequestToDetachTrackerToolErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tool: "+mInternalStructure.mUid+" could not request to detach from tracker.");
  }
  else if (igstk::TrackerToolAttachmentToTrackerErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tool: "+mInternalStructure.mUid+" could not attach to tracker.");
  }
  else if (igstk::TrackerToolDetachmentFromTrackerErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tool: "+mInternalStructure.mUid+" could not detach from tracker.");
  }
  //Polaris specific failures
  else if (igstk::InvalidPolarisPortNumberErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Polaris tool: "+mInternalStructure.mUid+" sendt invalid Polaris port number: "+ qstring_cast(mInternalStructure.mPortNumber) +".");
  }
  else if (igstk::InvalidPolarisSROMFilenameErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Polaris tool: "+mInternalStructure.mUid+" sendt invalid ROM file: "+mInternalStructure.mSROMFilename);
  }
  else if (igstk::InvalidPolarisPartNumberErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Polaris tool: "+mInternalStructure.mUid+" has an invalid part number.");
  }
  //Aurora specific failures
  else if (igstk::InvalidAuroraPortNumberErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Aurora tool: "+mInternalStructure.mUid+" has an invalid port number: "+ qstring_cast(mInternalStructure.mPortNumber)+".");
  }
  else if (igstk::InvalidAuroraSROMFilenameErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Aurora tool: "+mInternalStructure.mUid+" sendt invalid ROM file: "+ mInternalStructure.mSROMFilename);
  }
  else if (igstk::InvalidAuroraPartNumberErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Aurora tool: "+mInternalStructure.mUid+" has an invalid part number.");
  }
  else if (igstk::InvalidAuroraChannelNumberErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Polaris tool: "+mInternalStructure.mUid+" has an invalid channel number:"+qstring_cast(mInternalStructure.mChannelNumber) +".");
  }
}

bool IgstkTool::verifyInternalStructure()
{
  bool retval = true;
  QString verificationError("Internal verification of tool "+ mInternalStructure.mUid+" failed! REASON: ");
  if(mInternalStructure.mType == ssc::Tool::TOOL_NONE)
  {
    ssc::messageManager()->sendError(verificationError+" Tag <tool>::<type> is invalid ["+qstring_cast(mInternalStructure.mType)+"]. Valid types: [pointer, usprobe, reference]");
    retval = false;
  }
  if(mInternalStructure.mUid.isEmpty())
  {
    ssc::messageManager()->sendError(verificationError+" Tag <tool>::<uid> is empty. Give tool a unique id.");
    retval = false;
  }
  if(mInternalStructure.mTrackerType == IgstkTracker::TRACKER_NONE)
  {
    ssc::messageManager()->sendError(verificationError+" Tag <sensor>::<type> is invalid ["+qstring_cast(mInternalStructure.mTrackerType)+"]. Valid types: [polaris, spectra, vicra, aurora, micron (NOT SUPPORTED YET)]");
    retval = false;
  }
  if((mInternalStructure.mTrackerType == IgstkTracker::TRACKER_AURORA) && (mInternalStructure.mPortNumber >= 4))
  {
    ssc::messageManager()->sendError(verificationError+" Tag <sensor>::<portnumber> is invalid ["+qstring_cast(mInternalStructure.mPortNumber)+"]. Valid numbers: [0, 1, 2, 3]");
    retval = false;
  }
  if((mInternalStructure.mTrackerType == IgstkTracker::TRACKER_AURORA) && (mInternalStructure.mChannelNumber >= 1))
  {
    ssc::messageManager()->sendError(verificationError+" Tag <sensor>::<channelnumber> is invalid ["+qstring_cast(mInternalStructure.mChannelNumber)+"]. Valid numbers: [0, 1]");
    retval = false;
  }
  QDir dir;
  if(!mInternalStructure.mSROMFilename.isEmpty() && !dir.exists(mInternalStructure.mSROMFilename))
  {
    ssc::messageManager()->sendError(verificationError+" Tag <sensor>::<rom_file> is invalid ["+mInternalStructure.mSROMFilename+"]. Valid path: relative path to existing rom file.");
    retval = false;
  }
  if(!mInternalStructure.mCalibrationFilename.isEmpty() && !dir.exists(mInternalStructure.mCalibrationFilename))
  {
    ssc::messageManager()->sendError(verificationError+" Tag <calibration>::<cal_file> is invalid ["+mInternalStructure.mCalibrationFilename+"]. Valid path: relative path to existing calibration file.");
    retval = false;
  }
  if(!mInternalStructure.mTransformSaveFileName.isEmpty() && !dir.exists(mInternalStructure.mTransformSaveFileName))
  {
    ssc::messageManager()->sendError(verificationError+" Logging folder is invalid. Contact programmer! :)");
    retval = false;
  }
  if(!mInternalStructure.mLoggingFolderName.isEmpty() && !dir.exists(mInternalStructure.mLoggingFolderName))
  {
    ssc::messageManager()->sendError(verificationError+" Logging folder is invalid. Contact programmer! :)");
    retval = false;
  }

  return retval;
}

igstk::TrackerTool::Pointer IgstkTool::buildInternalTool()
{
  igstk::TrackerTool::Pointer tool;

  igstk::PolarisTrackerTool::Pointer tempPolarisTool;
  igstk::AuroraTrackerTool::Pointer tempAuroraTool;

  switch (mInternalStructure.mTrackerType)
  {
  case IgstkTracker::TRACKER_NONE:
    break;
  case IgstkTracker::TRACKER_POLARIS_SPECTRA:
  case IgstkTracker::TRACKER_POLARIS_VICRA:
  case IgstkTracker::TRACKER_POLARIS:
    tempPolarisTool = igstk::PolarisTrackerTool::New();
    tempPolarisTool->AddObserver(igstk::IGSTKEvent(), mToolObserver);
    if(!mInternalStructure.mWireless) //we only support wireless atm
      return tool = tempPolarisTool.GetPointer();
    tempPolarisTool->RequestSelectWirelessTrackerTool();
    tempPolarisTool->RequestSetSROMFileName(string_cast(mInternalStructure.mSROMFilename));
    tempPolarisTool->RequestConfigure();
    tempPolarisTool->SetCalibrationTransform(mInternalStructure.mCalibration);
    tool = tempPolarisTool;
    break;
  case IgstkTracker::TRACKER_AURORA:
    tempAuroraTool = igstk::AuroraTrackerTool::New();
    tempAuroraTool->AddObserver(igstk::IGSTKEvent(), mToolObserver);
    if(mInternalStructure.m5DOF)
    {
      tempAuroraTool->RequestSelect5DOFTrackerTool();
      tempAuroraTool->RequestSetPortNumber(mInternalStructure.mPortNumber);
      tempAuroraTool->RequestSetChannelNumber(mInternalStructure.mChannelNumber);
    }
    else
    {
      tempAuroraTool->RequestSelect6DOFTrackerTool();
      tempAuroraTool->RequestSetPortNumber(mInternalStructure.mPortNumber);
    }
    tempAuroraTool->RequestConfigure();
    tempAuroraTool->SetCalibrationTransform(mInternalStructure.mCalibration);
    tool = tempAuroraTool;
    break;
  case IgstkTracker::TRACKER_MICRON:
    //TODO: implement
    break;
  default:
    break;
  }
  return tool;
}

void IgstkTool::setCalibrationTransform(igstk::Transform calibration)
{
  mInternalStructure.mCalibration = calibration;
  mTool->SetCalibrationTransform(calibration);
}

void IgstkTool::internalAttachedToTracker(bool value)
{
  if(mAttachedToTracker == value)
    return;
  mAttachedToTracker = value;
  ssc::messageManager()->sendInfo("Tool: "+mInternalStructure.mUid+" is "+ (value ? "at":"de") +"tached "+(value ? "to" : "from")+" the tracker.");
  emit attachedToTracker(mAttachedToTracker);
}

void IgstkTool::internalTracked(bool value)
{
  if(mTracked == value)
    return;
  mTracked = value;

  emit tracked(mTracked);
}

void IgstkTool::internalVisible(bool value)
{
  if(mVisible == value)
    return;
  mVisible = value;
  emit toolVisible(mVisible);
}

void IgstkTool::addLogging()
{
  std::ofstream* loggerFile = new std::ofstream();
  QString logFile = mInternalStructure.mLoggingFolderName + "Tool_" + mInternalStructure.mName +"_Logging.txt";
  loggerFile->open( cstring_cast(logFile) );
  mLogger = igstk::Logger::New();
  mLogOutput = itk::StdStreamLogOutput::New();
  mLogOutput->SetStream(*loggerFile);
  mLogger->AddLogOutput(mLogOutput);
  mLogger->SetPriorityLevel(itk::Logger::DEBUG);

  mTool->SetLogger(mLogger);
}

void IgstkTool::printInternalStructure()
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
  std::cout << "mCalibration: " << std::endl;
  mInternalStructure.mCalibration.Print(std::cout, itk::Indent());
  std::cout << "mCalibrationFilename: " << mInternalStructure.mCalibrationFilename << std::endl;
  std::cout << "mGraphicsFileName: " << mInternalStructure.mGraphicsFileName << std::endl;
  std::cout << "mTransformSaveFileName: " << mInternalStructure.mTransformSaveFileName << std::endl;
  std::cout << "mLoggingFolderName: " << mInternalStructure.mLoggingFolderName  << std::endl;
  std::cout << "------------------------------------------------------------------" << std::endl;
}

} //namespace cx
