#include "cxTracker.h"

#include "sscMessageManager.h"
#include "cxTool.h"
#include "sscTypeConversions.h"


namespace cx
{
Tracker::Tracker(InternalStructure internalStructure) :
  mInternalStructure(internalStructure),
  mValid(false),
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
    mCommunication->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
    //mCommunication->SetPortNumber( igstk::SerialCommunication::PortNumber1 );
  #endif //_WINDOWS

  mCommunication->SetParity( igstk::SerialCommunication::NoParity );
  mCommunication->SetBaudRate( igstk::SerialCommunication::BaudRate115200 );
  mCommunication->SetDataBits( igstk::SerialCommunication::DataBits8 );
  mCommunication->SetStopBits( igstk::SerialCommunication::StopBits1 );
  mCommunication->SetHardwareHandshake(igstk::SerialCommunication::HandshakeOff);

  //mCommunication->SetCaptureFileName( "RecordedStreamByCustusX3.txt" );
  //mCommunication->SetCapture( true );
  //mCommunication->SetCapture( false );

  switch (mInternalStructure.mType)
  {
  case TRACKER_NONE:
    mUid = mName = "None";
    ssc::messageManager()->sendError("Tracker is of type TRACKER_NONE, this means it's not valid.");
    mValid = false;
    return;
    break;
  case TRACKER_POLARIS:
    mUid = mName = "Polaris";
    mTempPolarisTracker = PolarisTrackerType::New();
    mTempPolarisTracker->SetCommunication(mCommunication);
    ssc::messageManager()->sendInfo("Tracker is set to Polaris");
    mTracker = mTempPolarisTracker.GetPointer();
    mValid = true;
    break;
  case TRACKER_POLARIS_SPECTRA:
    mUid = mName = "Polaris Spectra";
    mTempPolarisTracker = PolarisTrackerType::New();
    mTempPolarisTracker->SetCommunication(mCommunication);
    ssc::messageManager()->sendInfo("Tracker is set to Polaris Spectra");
    mTracker = mTempPolarisTracker.GetPointer();
    mValid = true;
    break;
  case TRACKER_POLARIS_VICRA:
    mUid = mName = "Polaris Vicra";
    mTempPolarisTracker = PolarisTrackerType::New();
    mTempPolarisTracker->SetCommunication(mCommunication);
    ssc::messageManager()->sendInfo("Tracker is set to Polaris Vicra");
    mTracker = mTempPolarisTracker.GetPointer();
    mValid = true;
    break;
  case TRACKER_AURORA:
    mUid = mName = "Aurora";
    mTempAuroraTracker = AuroraTrackerType::New();
    mTempAuroraTracker->SetCommunication(mCommunication);
    ssc::messageManager()->sendInfo("Tracker: Aurora");
    mTracker = mTempAuroraTracker.GetPointer();
    mValid = true;
    break;
  case TRACKER_MICRON:
    mUid = mName = "Micron";
    ssc::messageManager()->sendInfo("Tracker is set to Micron");
    //TODO: implement support for a micron tracker...
    mValid = false;
    break;
  default:
    break;
  }
  mTrackerObserver->SetCallbackFunction(this, &Tracker::trackerTransformCallback);
  mTracker->AddObserver(igstk::IGSTKEvent(), mTrackerObserver);
  mCommunication->AddObserver(igstk::IGSTKEvent(), mTrackerObserver);
  //this->addLogging();
}

Tracker::~Tracker()
{
  //std::cout << "Tracker::~Tracker()" << std::endl;
}

Tracker::Type Tracker::getType() const
{
  return mInternalStructure.mType;
}

QString Tracker::getName() const
{
  return mName;
}

QString Tracker::getUid() const
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
  std::map<QString, ssc::ToolPtr> toolMap = *tools.get();
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

bool Tracker::isValid() const
{
  return mValid;
}

void Tracker::trackerTransformCallback(const itk::EventObject &event)
{
  //successes
  if (igstk::TrackerOpenEvent().CheckEvent(&event))
  {
    mInitialized = true;
    mOpen = true;
    emit trackerReport(TRACKER_OPEN, true, true, mUid);
    ssc::messageManager()->sendInfo("Tracker: "+mUid+" is open.");
  }
  else if (igstk::TrackerCloseEvent().CheckEvent(&event))
  {
    mInitialized = false;
    mOpen = false;
    emit trackerReport(TRACKER_OPEN, false, true, mUid);
    ssc::messageManager()->sendInfo("Tracker: "+mUid+" is closed.");
  }
  else if (igstk::TrackerInitializeEvent().CheckEvent(&event))
  {
    mInitialized = true;
    emit trackerReport(TRACKER_INITIALIZED, true, true, mUid);
    ssc::messageManager()->sendInfo("Tracker: "+mUid+" is initialized.");
  }
  else if (igstk::TrackerStartTrackingEvent().CheckEvent(&event))
  {
    mTracking = true;
    emit trackerReport(TRACKER_TRACKING, true, true, mUid);
    ssc::messageManager()->sendInfo("Tracker: "+mUid+" is tracking.");
  }
  else if (igstk::TrackerStopTrackingEvent().CheckEvent(&event))
  {
    mTracking = false;
    emit trackerReport(TRACKER_TRACKING, false, true, mUid);
    ssc::messageManager()->sendInfo("Tracker: "+mUid+" is stopping.");
  }
  else if (igstk::TrackerUpdateStatusEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_UPDATE_STATUS, true, true, mUid);
    //ssc::messageManager()->sendDebug("Tracker: "+mUid+" is updated."); //SPAM!
  }
  else if (igstk::TrackerToolTransformUpdateEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_TOOL_TRANSFORM_UPDATED, true, true, mUid);
    //ssc::messageManager()->sendDebug("Tracker: "+mUid+" has updated a transform."); //SPAM
  }
  //communication success
  else if (igstk::CompletedEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_COMMUNICATION_COMPLETE, true, true, mUid);
    // this seems to appear after every transmit (several times/second
    //ssc::messageManager()->sendInfo("Tracker: "+mUid+" set up communication correctly.");
  }
  //failures
  else if (igstk::InvalidRequestErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_INVALID_REQUEST, false, true, mUid);
    ssc::messageManager()->sendWarning("Tracker: "+mUid+" received an invalid request.");
  }
  else if (igstk::TrackerOpenErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_OPEN, true, false, mUid);
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not open.");
  }
  else if (igstk::TrackerCloseErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_OPEN, false, false, mUid);
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not close.");
  }
  else if (igstk::TrackerInitializeErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_INITIALIZED, true, false, mUid);
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not initialize.");
  }
  else if (igstk::TrackerStartTrackingErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_TRACKING, true, false, mUid);
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not start tracking.");
  }
  else if (igstk::TrackerStopTrackingErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_TRACKING, false, false, mUid);
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not stop tracking.");
  }
  else if (igstk::TrackerUpdateStatusErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_UPDATE_STATUS, true, false, mUid);
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not update.");
  }
  //communication failure
  else if (igstk::InputOutputErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_COMMUNICATION_INPUT_OUTPUT_ERROR, true, false, mUid);
    ssc::messageManager()->sendError("Tracker: "+mUid+" cannot communicate with input/output.");
  }
  else if (igstk::InputOutputTimeoutEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_COMMUNICATION_INPUT_OUTPUT_TIMEOUT, true, false, mUid);
    ssc::messageManager()->sendError("Tracker: "+mUid+" input/output communication timed out.");
  }
  else if (igstk::OpenPortErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_COMMUNICATION_OPEN_PORT_ERROR, true, false, mUid);
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not open communication with tracker.");
  }
  else if (igstk::ClosePortErrorEvent().CheckEvent(&event))
  {
    emit trackerReport(TRACKER_COMMUNICATION_OPEN_PORT_ERROR, false, false, mUid);
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not close communication with tracker.");
  }
}

void Tracker::addLogging()
{
  std::ofstream* loggerFile = new std::ofstream();
  QString logFile = mInternalStructure.mLoggingFolderName + "Tracker_Logging.txt";
  loggerFile->open( cstring_cast(logFile) );
  mTrackerLogger = igstk::Logger::New();
  mTrackerLogOutput = itk::StdStreamLogOutput::New();
  mTrackerLogOutput->SetStream(*loggerFile);
  mTrackerLogger->AddLogOutput(mTrackerLogOutput);
  mTrackerLogger->SetPriorityLevel(itk::Logger::DEBUG);

  mTracker->SetLogger(mTrackerLogger);
  mCommunication->SetLogger(mTrackerLogger);
}
}//namespace cx
