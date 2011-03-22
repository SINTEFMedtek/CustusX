#include "cxIgstkTracker.h"

#include "sscMessageManager.h"
#include "cxTool.h"
#include "sscTypeConversions.h"
#include "cxIgstkTool.h"
#include <time.h>


namespace cx
{
IgstkTracker::IgstkTracker(InternalStructure internalStructure) :
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
  #if defined WIN32
    mCommunication->SetPortNumber( igstk::SerialCommunication::PortNumber4 );
  #else
    mCommunication->SetPortNumber( igstk::SerialCommunication::PortNumber0 );

  #endif //WIN32

  mCommunication->SetParity( igstk::SerialCommunication::NoParity );
  mCommunication->SetBaudRate( igstk::SerialCommunication::BaudRate115200 );
  mCommunication->SetDataBits( igstk::SerialCommunication::DataBits8 );
  mCommunication->SetStopBits( igstk::SerialCommunication::StopBits1 );
  mCommunication->SetHardwareHandshake(igstk::SerialCommunication::HandshakeOff);

  QString comLogging = mInternalStructure.mLoggingFolderName + "RecordedStreamByCustusX3.txt";
  mCommunication->SetCaptureFileName(cstring_cast(comLogging));
  mCommunication->SetCapture( true );

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
  mTrackerObserver->SetCallbackFunction(this, &IgstkTracker::trackerTransformCallback);
  mTracker->AddObserver(igstk::IGSTKEvent(), mTrackerObserver);
  mCommunication->AddObserver(igstk::IGSTKEvent(), mTrackerObserver);
  this->addLogging();
}

IgstkTracker::~IgstkTracker()
{}

IgstkTracker::Type IgstkTracker::getType() const
{
  return mInternalStructure.mType;
}

QString IgstkTracker::getName() const
{
  return mName;
}

QString IgstkTracker::getUid() const
{
  return mUid;
}

IgstkTracker::TrackerType* IgstkTracker::getPointer() const
{
  return mTracker;
}

void IgstkTracker::open()
{
  igstk::SerialCommunication::ResultType result = igstk::SerialCommunication::FAILURE;
  for(int i=0; i<5; ++i)
  {
    result = mCommunication->OpenCommunication();
    if(result == igstk::SerialCommunication::SUCCESS)
      break;
  }
  mTracker->RequestOpen();
}

void IgstkTracker::close()
{
  mTracker->RequestClose();
  mCommunication->CloseCommunication();
}

void IgstkTracker::attachTools(std::map<QString, IgstkToolPtr> tools)
{
  for(std::map<QString, IgstkToolPtr>::iterator it = tools.begin(); it != tools.end(); ++it )
  {
    IgstkToolPtr tool = it->second;

    if(tool && tool->getPointer())
    {
      if(tool->getTrackerType() != mInternalStructure.mType)
        ssc::messageManager()->sendWarning("Tracker is attaching a tool that is not of the correct type. Trackers type: "+qstring_cast(mInternalStructure.mType)+", tools tracker type: "+qstring_cast(tool->getTrackerType()));

      tool->getPointer()->RequestAttachToTracker(mTracker);

      if(tool->getType() == ssc::Tool::TOOL_REFERENCE)
        mTracker->RequestSetReferenceTool(tool->getPointer());
    }
  }
}

void IgstkTracker::detachTools(std::map<QString, IgstkToolPtr> tools)
{
  for(std::map<QString, IgstkToolPtr>::iterator it = tools.begin(); it != tools.end(); ++it )
  {
    IgstkToolPtr tool = it->second;

    if(tool && tool->getPointer())
    {
      tool->getPointer()->RequestDetachFromTracker();
    }
  }
}

void IgstkTracker::startTracking()
{
  mTracker->RequestStartTracking();
}

void IgstkTracker::stopTracking()
{
  mTracker->RequestStopTracking();
}

bool IgstkTracker::isValid() const
{
  return mValid;
}

bool IgstkTracker::isInitialized() const
{
  return mInitialized;
}

bool IgstkTracker::isTracking() const
{
  return mTracking;
}

void IgstkTracker::trackerTransformCallback(const itk::EventObject &event)
{
  //successes
  if (igstk::TrackerOpenEvent().CheckEvent(&event))
  {
    this->internalOpen(true);
    this->internalInitialized(true);
  }
  else if (igstk::TrackerCloseEvent().CheckEvent(&event))
  {
    this->internalOpen(false);
    this->internalInitialized(false);
  }
  else if (igstk::TrackerInitializeEvent().CheckEvent(&event))
  {
    //Never happens???
    //this->internalInitialized(true);
    //ssc::messageManager()->sendInfo("Tracker: "+mUid+" is initialized.");
    ssc::messageManager()->sendWarning("This never happens for some reason...  check code");
  }
  else if (igstk::TrackerStartTrackingEvent().CheckEvent(&event))
  {
    this->internalTracking(true);
  }
  else if (igstk::TrackerStopTrackingEvent().CheckEvent(&event))
  {
    this->internalTracking(false);
  }
  else if (igstk::TrackerUpdateStatusEvent().CheckEvent(&event))
  {
    //ssc::messageManager()->sendDebug("Tracker: "+mUid+" is updated."); //SPAM!
  }
  else if (igstk::TrackerToolTransformUpdateEvent().CheckEvent(&event))
  {
    //ssc::messageManager()->sendDebug("Tracker: "+mUid+" has updated a transform."); //SPAM
  }
  //communication success
  else if (igstk::CompletedEvent().CheckEvent(&event))
  {
    // this seems to appear after every transmit (several times/second)
    //ssc::messageManager()->sendInfo("Tracker: "+mUid+" set up communication correctly."); //SPAM
  }
  //failures
  else if (igstk::InvalidRequestErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendWarning("Tracker: "+mUid+" received an invalid request. This means that the internal igstk tracker did not accept the request. Do not know which request.");
  }
  else if (igstk::TrackerOpenErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not open.");
  }
  else if (igstk::TrackerCloseErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not close.");
  }
  else if (igstk::TrackerInitializeErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not initialize.");
  }
  else if (igstk::TrackerStartTrackingErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not start tracking.");
  }
  else if (igstk::TrackerStopTrackingErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not stop tracking.");
  }
  else if (igstk::TrackerUpdateStatusErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not update.");
  }
  //communication failure
  else if (igstk::InputOutputErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tracker: "+mUid+" cannot communicate with input/output.");
  }
  else if (igstk::InputOutputTimeoutEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tracker: "+mUid+" input/output communication timed out.");
  }
  else if (igstk::OpenPortErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not open communication with tracker.");
  }
  else if (igstk::ClosePortErrorEvent().CheckEvent(&event))
  {
    ssc::messageManager()->sendError("Tracker: "+mUid+" could not close communication with tracker.");
  }
}

void IgstkTracker::addLogging()
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

void IgstkTracker::internalOpen(bool value)
{
  if(mOpen == value)
    return;
  mOpen = value;

  ssc::messageManager()->sendInfo("Tracker: "+mUid+" is "+(value ? "open" : "closed")+".");
  emit open(mOpen);
}

void IgstkTracker::internalInitialized(bool value)
{
  if(mInitialized == value)
    return;
  mInitialized = value;

  ssc::messageManager()->sendInfo("Tracker: "+mUid+" is "+(value ? "" : "un")+"initialized.");
  emit initialized(mInitialized);
}

void IgstkTracker::internalTracking(bool value)
{
  if(mTracking == value)
    return;
  mTracking = value;

  ssc::messageManager()->sendInfo("Tracker: "+mUid+" is "+(value ? "" : "not ")+"tracking.");
  emit tracking(mTracking);
}
}//namespace cx
