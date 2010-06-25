#ifndef CXTRACKER_H_
#define CXTRACKER_H_

#include <QObject>

#include <map>
#include <boost/shared_ptr.hpp>
#include <igstkLogger.h>
#include <igstkTracker.h>
#include <igstkPolarisTracker.h>
#include <igstkAuroraTracker.h>
#include <itkStdStreamLogOutput.h>
#ifdef WIN32
  #include "igstkSerialCommunicationForWindows.h"
#else
  #include "igstkSerialCommunicationForPosix.h"
#endif
#include "sscTool.h"

namespace cx
{
typedef std::map<std::string, ssc::ToolPtr> ToolMap;
typedef boost::shared_ptr<ToolMap> ToolMapPtr;

/**
 * \class Tracker
 *
 * \brief Class representing the navigation system.
 *
 * \date Nov 7, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
class Tracker : public QObject
{
  Q_OBJECT
public:
  #ifdef WIN32
  /** The type of serial communication used on a windows platform.*/
    typedef igstk::SerialCommunicationForWindows CommunicationType;
  #else
    /** The type of serial communication used on Mac and Linux.*/
    typedef igstk::SerialCommunicationForPosix CommunicationType;
  #endif
  typedef igstk::Tracker TrackerType;
  typedef igstk::PolarisTracker PolarisTrackerType;
  typedef igstk::AuroraTracker AuroraTrackerType;

  enum Type
  {
    TRACKER_NONE,             ///< Not specified
    TRACKER_POLARIS,          ///< NDIs Polaris tracker
    TRACKER_POLARIS_SPECTRA,  ///< NDIs Polaris Spectra tracker
    TRACKER_POLARIS_VICRA,    ///< NDIs Polaris Vicra tracker
    TRACKER_AURORA,           ///< NDIs Aurora tracker
    TRACKER_MICRON            ///< Claron Technologys Micron tracker
  };

  enum Message
  {
    TRACKER_INVALID_REQUEST,                    ///< internal state machine didn't accept the request
    TRACKER_OPEN,                               ///< hardware accepted the tracker as open
    TRACKER_INITIALIZED,                        ///< hardware accepted the tracker as initialized
    TRACKER_TRACKING,                           ///< tracker is tracking
    TRACKER_UPDATE_STATUS,                      ///< tracker got an update event
    TRACKER_TOOL_TRANSFORM_UPDATED,             ///< a tool attached to the tracker got a new transform
    TRACKER_COMMUNICATION_COMPLETE,             ///< communication port completed a task successfully
    TRACKER_COMMUNICATION_INPUT_OUTPUT_ERROR,   ///< communication port failed at completing a task
    TRACKER_COMMUNICATION_INPUT_OUTPUT_TIMEOUT, ///< communication port timed out
    TRACKER_COMMUNICATION_OPEN_PORT_ERROR       ///< communication port tried to open or close
  };
  typedef Tracker::Message TrackerMessage;
  typedef std::string stdString;

  /**A trackers internal structure \warning make sure you set all the members to an appropriate value.*/
  struct InternalStructure
  {
    Type            mType;              ///< the trackers type
    std::string     mLoggingFolderName; ///< path to where log should be saved
    InternalStructure() :
      mType(TRACKER_NONE), mLoggingFolderName("") {}; ///< set default values for the internal structure
  };

  Tracker(InternalStructure internalStructure);
  ~Tracker();

  Type getType() const;               ///< returns the trackers type
  std::string getName() const;        ///< get the trackers name
  std::string getUid() const;         ///< get the tracker unique id
  TrackerType* getPointer() const;    ///< return a pointer to the internal tracker base
  void open();                        ///< open the tracker for communication
  void attachTools(ToolMapPtr tools); ///< attach a list of tools to the tracker hw
  void startTracking();               ///< start tracking
  void stopTracking();                ///< stop tracking

  bool isValid() const;               ///< whether this tracker is constructed correctly or not

signals:
   /**
   * Signal that reports signals received by the the tool
   * \param message What happended to the tool
   * \param state   Whether the tool was trying to enter or leave a state
   * \param success Whether or not the request was a success
   * \param uid     The tools unique id
   */
  void trackerReport(TrackerMessage message, bool state, bool success, stdString uid);

protected:
  typedef itk::ReceptorMemberCommand<Tracker> ObserverType;

  Tracker(){}; ///< do not use this one
  void trackerTransformCallback(const itk::EventObject &eventVar); ///< callback receiving events from the observer
  void addLogging(); ///< adds logging to the internal igstk components

  InternalStructure mInternalStructure; ///< the trackers type
  bool mValid;                          ///< whether this tracker is constructed correctly or not
  std::string       mUid;               ///< the trackers unique id
  std::string       mName;              ///< the trackers name
  TrackerType*      mTracker;           ///< pointer to the base class of the internal igstk tracker

  PolarisTrackerType::Pointer       mTempPolarisTracker;    ///< pointer to a temp polaris tracker
  AuroraTrackerType::Pointer        mTempAuroraTracker;     ///< pointer to a temp aurora tracker
  CommunicationType::Pointer        mCommunication;         ///< pointer to the serial communication used to communicate with the NDI trackers
  ObserverType::Pointer             mTrackerObserver;       ///< observer listening for igstk events
  igstk::Logger::Pointer            mTrackerLogger;         ///< logging the internal igstk behavior
  itk::StdStreamLogOutput::Pointer  mTrackerLogOutput;      ///< output to write the log to

  bool mOpen;         ///< whether or not the tracker is open
  bool mInitialized;  ///< whether or not the tracker is initialized
  bool mTracking;     ///< whether or not the tracker is tracking
};
typedef boost::shared_ptr<Tracker> TrackerPtr;
}//namespace cx

#endif /* CXTRACKER_H_ */
