#ifndef CXTOOL_H_
#define CXTOOL_H_

#include "sscTool.h"

#include <limits.h>
#include <QTimer>
#include <boost/shared_ptr.hpp>
#include <igstkPolarisTrackerTool.h>
#include <igstkAuroraTrackerTool.h>
#include <igstkTransform.h>
#include <igstkLogger.h>
#include <itkStdStreamLogOutput.h>
#include "sscTransform3D.h"
#include "cxIgstkTracker.h"
#include "vtkForwardDeclarations.h"

class QStringList;

namespace ssc
{
}

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

typedef boost::shared_ptr<class Probe> ProbePtr;


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

  /* //only used for documentation purposes
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
  */

  /**A tools internal structure \warning make sure you set all the members to an appropriate value.*/
  struct InternalStructure
  {
    ssc::Tool::Type   mType;                  ///< the tools type
    QString           mName;                  ///< the tools name
    QString           mUid;                   ///< the tools unique id
    IgstkTracker::Type     mTrackerType;           ///< what product the tool belongs to
    QString           mSROMFilename;          ///< path to the tools SROM file
    unsigned int      mPortNumber;            ///< the port number the tool is connected to
    unsigned int      mChannelNumber;         ///< the channel the tool is connected to
    std::map<int, ssc::Vector3D>     mReferencePoints;        ///< optional point on the frame, specifying a known reference point, 0,0,0 is default, in sensor space
    bool              mWireless;              ///< whether or not the tool is wireless
    bool              m5DOF;                  ///< whether or not the tool have 5 DOF
    igstk::Transform  mCalibration;           ///< transform read from mCalibrationFilename
    QString           mCalibrationFilename;   ///< path to the tools calibration file
    QString           mGraphicsFileName;      ///< path to this tools graphics file
    QString           mTransformSaveFileName; ///< path to where transforms should be saved
    QString           mLoggingFolderName;     ///< path to where log should be saved
    QString           mInstrumentId;          ///< The instruments id
    QString           mInstrumentScannerId;   ///< The id of the ultrasound scanner if the instrument is a probe
    InternalStructure() :
      mType(ssc::Tool::TOOL_NONE), mName(""), mUid(""), mTrackerType(IgstkTracker::TRACKER_NONE),
      mSROMFilename(""), mPortNumber(UINT_MAX), mChannelNumber(UINT_MAX), mReferencePoints(),
      mWireless(true), m5DOF(true), mCalibrationFilename(""), mGraphicsFileName(""),
      mTransformSaveFileName(""), mLoggingFolderName(""), mInstrumentId(""),
      mInstrumentScannerId("") {}; ///< sets up default values for all the members
  };

  Tool(IgstkToolPtr igstkTool);
  ~Tool();

  virtual ssc::Tool::Type getType() const;
  virtual QString getGraphicsFileName() const;
  virtual vtkPolyDataPtr getGraphicsPolyData() const;
  virtual ssc::TimedTransformMapPtr getPositionHistory();
  virtual ssc::Transform3D get_prMt() const;
  virtual bool getVisible() const;
  virtual bool isInitialized() const;
  virtual QString getUid() const;
  virtual QString getName() const;
  virtual int getIndex() const{return 0;};
  virtual ssc::ProbeData getProbeSector() const;
  virtual ssc::ProbePtr getProbe() const;
  virtual double getTimestamp() const{ return 0; }; //	TODO
  virtual double getTooltipOffset() const; ///< get a virtual offset extending from the tool tip.
  virtual void setTooltipOffset(double val);///< set a virtual offset extending from the tool tip.

  virtual bool isCalibrated() const; ///< true if calibration is different from identity
  virtual ssc::Transform3D getCalibration_sMt() const; ///< get the calibration transform from tool space to sensor space (where the spheres or similar live)
  virtual void setCalibration_sMt(ssc::Transform3D calibration); ///< requests to use the calibration and replaces the tools calibration file
  QString getCalibrationFileName() const; ///< returns the path to the tools calibration file

  IgstkTracker::Type getTrackerType(); ///< the type of tracker this tool belongs to

  virtual std::map<int, ssc::Vector3D> getReferencePoints() const; ///< Get the optional reference points from this tool
  virtual bool hasReferencePointWithId(int id);

  virtual ssc::TimedTransformMap getSessionHistory(double startTime, double stopTime); ///< Get a tools transforms from within a given session

  bool isValid() const; ///< whether this tool is constructed correctly or not

  void addXml(QDomNode& dataNode);
  void parseXml(QDomNode& dataNode);

  typedef ssc::Transform3D Transform3D;
signals:
  void attachedToTracker(bool);

private slots:
  void toolTransformAndTimestampSlot(Transform3D matrix, double timestamp); ///< timestamp is in milliseconds
  void calculateTpsSlot();
  void toolVisibleSlot(bool);

private:
  Tool(){}; ///< do not use this constructor
  void writeCalibrationToFile();
  bool verifyInternalStructure(); ///< checks the values of the internal structure to see if they seem reasonable
  TrackerToolType* buildInternalTool(); ///< builds the igstk trackertool
  void createPolyData(); ///< creates the polydata either from file or a vtkConeSource

  void printInternalStructure(); ///< FOR DEBUGGING

  IgstkToolPtr mTool;

  ssc::TimedTransformMapPtr mPositionHistory;
  vtkPolyDataPtr mPolyData;                         ///< the polydata used to represent the tool graphically
  ssc::Transform3DPtr m_prMt;                       ///< the transform from the tool to the patient reference

  bool mValid;                                      ///< whether this tool is constructed correctly or not
  bool mConfigured;         ///< whether or not the tool is properly configured
  bool mTracked;            ///< whether the tool is being tracked or not

  double mToolTipOffset; ///< distance from tool where point should be shown

  ProbePtr mProbe;

  QTimer mTpsTimer;

};
typedef boost::shared_ptr<Tool> ToolPtr;

} //namespace cx
#endif /* CXTOOL_H_ */
