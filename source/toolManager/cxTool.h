#ifndef CXTOOL_H_
#define CXTOOL_H_

#include "sscTool.h"

#include <limits.h>
#include <boost/shared_ptr.hpp>
#include <igstkPolarisTrackerTool.h>
#include <igstkAuroraTrackerTool.h>
#include <igstkTransform.h>
#include <igstkLogger.h>
#include <itkStdStreamLogOutput.h>
#include "sscTransform3D.h"
#include "cxTracker.h"

typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;
typedef vtkSmartPointer<class vtkConeSource> vtkConeSourcePtr;
typedef vtkSmartPointer<class vtkSTLReader> vtkSTLReaderPtr;

namespace itk
{
class EventObject;
}

namespace cx
{
typedef std::vector<double> DoubleVector;
typedef boost::shared_ptr<DoubleVector> DoubleVectorPtr;
typedef std::vector<ssc::Transform3DPtr> Transform3DVector;
typedef boost::shared_ptr<Transform3DVector> Transform3DVectorPtr;

/**
 * \class Tool
 *
 * \brief Class representing the tools a navigation system can recognize.
 *
 * \date Nov 6, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
class Tool: public ssc::Tool
{
  Q_OBJECT
public:
  typedef igstk::TrackerTool TrackerToolType;
  typedef igstk::PolarisTrackerTool PolarisTrackerToolType;
  typedef igstk::AuroraTrackerTool AuroraTrackerToolType;
  typedef igstk::Transform TransformType;

  enum Message
  {
    TOOL_INVALID_REQUEST,             ///< internal state machine didn't accept the request
    TOOL_HW_CONFIGURED,               ///< hardware accepted tool as configured
    TOOL_ATTACHED_TO_TRACKER,         ///< tool accepted by hardware as attached
    TOOL_VISIBLE,                     ///< tool visible to hardware
    TOOL_TRACKED,                     ///< tool tracked by hardware
    TOOL_COORDINATESYSTEM_TRANSFORM,  ///< transform received
    TOOL_NDI_PORT_NUMBER,             ///< hardware responds to NDI port number
    TOOL_NDI_SROM_FILENAME,           ///< hardware responds to NDI SROM filename
    TOOL_NDI_PART_NUMBER,             ///< hardware responds to NDI part number
    TOOL_AURORA_CHANNEL_NUMBER        ///< hardware responds to Aurora channel number
  };
  typedef Tool::Message ToolMessage;
  typedef std::string stdString;

  /**A tools internal structure \warning make sure you set all the members to an appropriate value.*/
  struct InternalStructure
  {
    ssc::Tool::Type   mType;                  ///< the tools type
    std::string       mName;                  ///< the tools name
    std::string       mUid;                   ///< the tools unique id
    Tracker::Type     mTrackerType;           ///< what product the tool belongs to
    std::string       mSROMFilename;          ///< path to the tools SROM file
    unsigned int      mPortNumber;            ///< the port number the tool is connected to
    unsigned int      mChannelNumber;         ///< the channel the tool is connected to
    bool              mWireless;              ///< whether or not the tool is wireless
    bool              m5DOF;                  ///< whether or not the tool have 5 DOF
    std::string       mCalibrationFilename;   ///< path to the tools calibration file
    std::string       mGraphicsFileName;      ///< path to this tools graphics file
    std::string       mTransformSaveFileName; ///< path to where transforms should be saved
    std::string       mLoggingFolderName;     ///< path to where log should be saved
    InternalStructure() :
      mType(ssc::Tool::TOOL_NONE), mName(""), mUid(""), mTrackerType(Tracker::TRACKER_NONE),
      mSROMFilename(""), mPortNumber(UINT_MAX), mChannelNumber(UINT_MAX),
      mWireless(true), m5DOF(true), mCalibrationFilename(""), mGraphicsFileName(""),
      mTransformSaveFileName(""), mLoggingFolderName(""){}; ///< sets up default values for all the members
  };

  Tool(InternalStructure& internalStructur);   ///< constructor
  ~Tool();                                    ///< destructor

  virtual ssc::Tool::Type getType() const;
  virtual std::string getGraphicsFileName() const;
  virtual vtkPolyDataPtr getGraphicsPolyData() const;
  virtual void saveTransformsAndTimestamps();
  virtual void setTransformSaveFile(const std::string& filename);
  virtual ssc::Transform3D get_prMt() const;
  virtual bool getVisible() const;
  //virtual ssc::Transform3DPtr getLastTransform();
  virtual std::string getUid() const;
  virtual std::string getName() const;
  virtual int getIndex() const{return 0;};
  virtual bool isCalibrated() const; //TODO
  virtual ssc::ProbeSector getProbeSector() const;//{ return ssc::ProbeSector(); }; //TODO
  virtual double getTimestamp() const{ return 0; }; //	TODO
  virtual double getTooltipOffset() const; ///< get a virtual offset extending from the tool tip.
  virtual void setTooltipOffset(double val);///< set a virtual offset extending from the tool tip.
  virtual void set_prMt(const ssc::Transform3D& transform);

  TrackerToolType* getPointer() const; ///< return a pointer to the internal tools base object
  bool isValid() const; ///< whether this tool is constructed correctly or not

signals:
  /**
   * Signal that reports signals received by the the tool
   * \param message What happened to the tool
   * \param state   Whether the tool was trying to enter or leave a state
   * \param success Whether or not the request was a success
   * \param uid     The tools unique id
   */
  void toolReport(ToolMessage message, bool state, bool success, stdString uid);

protected:
  typedef itk::ReceptorMemberCommand<Tool> ObserverType;

  Tool(){}; ///< do not use this constructor
  void toolTransformCallback(const itk::EventObject &eventVar); ///< handles all incoming events from igstk
  bool verifyInternalStructure(); ///< checks the values of the internal structure to see if they seem reasonable
  TrackerToolType* buildInternalTool(); ///< builds the igstk trackertool
  void createPolyData(); ///< creates the polydata either from file or a vtkConeSource
  void determineToolsCalibration(); ///< reads the calibration file and saves it as igstk::Transform
  void addLogging(TrackerToolType* trackerTool); ///< adds igstk logging to the internal igstk trackertool
  void printInternalStructure(); ///< for debugging

  InternalStructure mInternalStructure;             ///< the tools internal structure
  bool mValid;                                      ///< whether this tool is constructed correctly or not
  TrackerToolType* mTool;                           ///< pointer to the base class of the igstk tool
  PolarisTrackerToolType::Pointer mTempPolarisTool; ///< internal container for a temp polaris tool
  AuroraTrackerToolType::Pointer mTempAuroraTool;   ///< internal container for a temp aurora too
  ObserverType::Pointer mToolObserver;           ///< observer listening for igstk events
  TransformType mCalibrationTransform;              ///< a matrix representing the tools calibration
  Transform3DVectorPtr mTransforms;                 ///< all transforms received by the tool
  DoubleVectorPtr mTimestamps;                      ///< all timestamps received by the tool
  vtkPolyDataPtr mPolyData;                         ///< the polydata used to represent the tool graphically
  ssc::Transform3DPtr m_prMt;                       ///< the transform from the tool to the patient reference
  igstk::Logger::Pointer mLogger;                   ///< logging the internal igstk behavior
  itk::StdStreamLogOutput::Pointer mLogOutput;      ///< output to write the log to

  bool mConfigured;         ///< whether or not the tool is properly configured
  bool mVisible;            ///< whether or not the tool is visible to the tracking system
  bool mAttachedToTracker;  ///< whether the tool is attached to a tracker or not
  bool mTracked;            ///< whether the tool is being tracked or not

  double mToolTipOffset; ///< distance from tool where point should be shown
};
typedef boost::shared_ptr<Tool> ToolPtr;
} //namespace cx
#endif /* CXTOOL_H_ */
