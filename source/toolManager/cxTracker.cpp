#include "cxTracker.h"

#include "cxTool.h"
#include "cxMessageManager.h"

namespace cx
{
Tracker::Tracker(InternalStructure internalStructure) :
  mInternalStructure(internalStructure),
  mUid(""),
  mName(""),
  mCommunication(CommunicationType::New()),
  mTrackerObserver(ObserverType::New()),
  mOpen(false),
  mInitialized(false),
  mTracking(false)
{
  #if defined (_WINDOWS)
    mCommunication->SetPortNumber( igstk::SerialCommunication::PortNumber4 );
  #else
    //mCommunication->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
    mCommunication->SetPortNumber( igstk::SerialCommunication::PortNumber1 );
  #endif //_WINDOWS

  mCommunication->SetParity( igstk::SerialCommunication::NoParity );
  mCommunication->SetBaudRate( igstk::SerialCommunication::BaudRate115200 );
  mCommunication->SetDataBits( igstk::SerialCommunication::DataBits8 );
  mCommunication->SetStopBits( igstk::SerialCommunication::StopBits1 );
  mCommunication->SetHardwareHandshake(igstk::SerialCommunication::HandshakeOff);

  mCommunication->SetCaptureFileName( "RecordedStreamByCustusX3.txt" );
  mCommunication->SetCapture( true );
  //mCommunication->SetCapture( false );

  switch (mInternalStructure.mType)
  {
  case TRACKER_NONE:
    mUid = mName = "None";
    messageManager()->sendError("Tracker is of type TRACKER_NONE, this means it's not valid.");
    return;
    break;
  case TRACKER_POLARIS:
    mUid = mName = "Polaris";
    mTempPolarisTracker = PolarisTrackerType::New();
    mTempPolarisTracker->SetCommunication(mCommunication);
    messageManager()->sendInfo("Tracker: Polaris");
    mTracker = mTempPolarisTracker.GetPointer();
    break;
  case TRACKER_POLARIS_SPECTRA:
    mUid = mName = "Polaris Spectra";
    mTempPolarisTracker = PolarisTrackerType::New();
    mTempPolarisTracker->SetCommunication(mCommunication);
    messageManager()->sendInfo("Tracker: Polaris Spectra");
    mTracker = mTempPolarisTracker.GetPointer();
    break;
  case TRACKER_POLARIS_VICRA:
    mUid = mName = "Polaris Vicra";
    mTempPolarisTracker = PolarisTrackerType::New();
    mTempPolarisTracker->SetCommunication(mCommunication);
    messageManager()->sendInfo("Tracker: Polaris Vicra");
    mTracker = mTempPolarisTracker.GetPointer();
    break;
  case TRACKER_AURORA:
    mUid = mName = "Aurora";
    mTempAuroraTracker = AuroraTrackerType::New();
    mTempAuroraTracker->SetCommunication(mCommunication);
    messageManager()->sendInfo("Tracker: Aurora");
    mTracker = mTempAuroraTracker.GetPointer();
    break;
  case TRACKER_MICRON:
    mUid = mName = "Micron";
    messageManager()->sendInfo("Tracker: Micron");
    //TODO: implement support for a micron tracker...
    break;
  default:
    break;
  }
  mTrackerObserver->SetCallbackFunction(this, &Tracker::trackerTransformCallback);
  mTracker->AddObserver(igstk::IGSTKEvent(), mTrackerObserver);
  mCommunication->AddObserver(igstk::IGSTKEvent(), mTrackerObserver);
  this->addLogging();
}
Tracker::~Tracker()
{}
Tracker::Type Tracker::getType() const
{
  return mInternalStructure.mType;
}
std::string Tracker::getName() const
{
  return mName;
}
std::string Tracker::getUid() const
{
  return mUid;
}
Tracker::TrackerType* Tracker::getPointer() const
{
  return mTracker;
}
void Tracker::open()
{
  mCommunication->OpenCommunication();
  mTracker->RequestOpen();
}
void Tracker::attachTools(ToolMapPtr tools)
{
  std::map<std::string, ssc::ToolPtr> toolMap = *tools.get();
  for(ToolMap::iterator it = toolMap.begin(); it != toolMap.end(); ++it )
  {
    ToolPtr tool = boost::shared_static_cast<Tool>((*it).second);

    //ssc::Tool* toolPtr = ((*it).second).get();
    //Tool* tool = static_cast<Tool*>(toolPtr);
    if(tool && tool->getPointer())
    {
      tool->getPointer()->RequestAttachToTracker(mTracker);
      if(tool->getType() == ssc::Tool::TOOL_REFERENCE)
        mTracker->RequestSetReferenceTool(tool->getPointer());
    }
  }
}
void Tracker::startTracking()
{
  mTracker->RequestStartTracking();
}
void Tracker::stopTracking()
{
  mTracker->RequestStopTracking();
}
void Tracker::trackerTransformCallback(const itk::EventObject &event)
{
  //successes
  if (igstk::TrackerOpenEvent().CheckEvent(&event))
  {
    mInitialized = true;
    mOpen = true;
    emit trackerReport(TRACKER_OPEN, true, true, mUid);
  }
  else if (igstk::TrackerCloseEvent().CheckEvent(&event))
  {
    mInitialized = false;
    mOpen = false;
    emit trackerReport(TRACKER_OPEN, false, true, mUid);
  }
  else if (igstk::TrackerInitializeEvent().CheckEvent(&event))
  {
    mInitialized = true;
    emit trackerReport(TRACKER_INITIALIZED, true, true, mUid);
  }
  else if (igstk::TrackerStartTrackingEvent().CheckEvent(&event))
  {
    mTracking = true;
    emit trackerReport(TRACKER_TRACKING, true, true, mUid);
  }
  else if (igstk::TrackerStopTrackingEvent().CheckEvent(&event))
  {
    mTracking = false;
    emit trackerReport(TRACKER_TRACKING, false, true, mUid);
  }
  else if (igstk::TrackerUpdateStatusEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_UPDATE_STATUS, true, true, mUid);
  }
  else if (igstk::TrackerToolTransformUpdateEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_TOOL_TRANSFORM_UPDATED, true, true, mUid);
  }
  //communication success
  else if (igstk::CompletedEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_COMMUNICATION_COMPLETE, true, true, mUid);
  }
  //failures
  else if (igstk::InvalidRequestErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_INVALID_REQUEST, false, true, mUid);
  }
  else if (igstk::TrackerOpenErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_OPEN, true, false, mUid);
  }
  else if (igstk::TrackerCloseErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_OPEN, false, false, mUid);
  }
  else if (igstk::TrackerInitializeErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_INITIALIZED, true, false, mUid);
  }
  else if (igstk::TrackerStartTrackingErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_TRACKING, true, false, mUid);
  }
  else if (igstk::TrackerStopTrackingErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_TRACKING, false, false, mUid);
  }
  else if (igstk::TrackerUpdateStatusErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_UPDATE_STATUS, true, false, mUid);
  }
  //communication failure
  else if (igstk::InputOutputErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_COMMUNICATION_INPUT_OUTPUT_ERROR, true, false, mUid);
  }
  else if (igstk::InputOutputTimeoutEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_COMMUNICATION_INPUT_OUTPUT_TIMEOUT, true, false, mUid);
  }
  else if (igstk::OpenPortErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_COMMUNICATION_OPEN_PORT_ERROR, true, false, mUid);
  }
  else if (igstk::ClosePortErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_COMMUNICATION_OPEN_PORT_ERROR, false, false, mUid);
  }
}
void Tracker::addLogging()
{
  std::ofstream* loggerFile = new std::ofstream();
  std::string logFile = mInternalStructure.mLoggingFolderName + "Tracker_Logging.txt";
  loggerFile->open( logFile.c_str() );
  mTrackerLogger = igstk::Logger::New();
  mTrackerLogOutput = itk::StdStreamLogOutput::New();
  mTrackerLogOutput->SetStream(*loggerFile);
  mTrackerLogger->AddLogOutput(mTrackerLogOutput);
  mTrackerLogger->SetPriorityLevel(itk::Logger::DEBUG);

  mTracker->SetLogger(mTrackerLogger);
  mCommunication->SetLogger(mTrackerLogger);
}
}//namespace cx
