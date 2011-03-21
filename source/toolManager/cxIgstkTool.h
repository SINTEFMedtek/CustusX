#ifndef CXIGSTKTOOL_H_
#define CXIGSTKTOOL_H_

#include <QObject>
#include "cxTool.h"

#include <limits.h>
#include <boost/shared_ptr.hpp>
#include <igstkPolarisTrackerTool.h>
#include <igstkAuroraTrackerTool.h>
#include <igstkTransform.h>
#include <igstkLogger.h>
#include <itkStdStreamLogOutput.h>
#include "sscTransform3D.h"
#include "cxIgstkTracker.h"
#include "vtkForwardDeclarations.h"

namespace cx
{

class IgstkTool;
typedef boost::shared_ptr<IgstkTool> IgstkToolPtr;
typedef boost::weak_ptr<IgstkTool> IgstkToolWeakPtr;

/**
 * \class IgstkTool
 *
 * \brief  Class for controlling the igstk tracking (hardware) interface.
 *
 * \date Mar 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class IgstkTool : public QObject
{
  Q_OBJECT
public:
  IgstkTool(Tool::InternalStructure internalStructure);
  virtual ~IgstkTool();

  Tool::InternalStructure getInternalStructure();
  QString getUid();

  igstk::TrackerTool::Pointer getPointer() const; ///< return a pointer to the internal tools base object
  IgstkTracker::Type getTrackerType();
  ssc::Tool::Type getType() const;

  bool isValid() const; ///< Thread safe, volatile
  bool isInitialized() const; ///< Thread safe, volatile
  bool isTracked() const; ///< Thread safe, volatile
  bool isVisible() const; ///< Thread safe, volatile

  void setReference(IgstkToolPtr);
  void setTracker(TrackerPtr tracker);
  void setCalibrationTransform(igstk::Transform calibration);

  void printInternalStructure();

  typedef ssc::Transform3D Transform3D;

signals:
  void attachedToTracker(bool);
  void tracked(bool);
  void toolVisible(bool);
  void toolTransformAndTimestamp(Transform3D matrix, double timestamp);
  void tps(int); ///< transforms per second

private:
  void toolTransformCallback(const itk::EventObject &event);
  bool verifyInternalStructure();
  igstk::TrackerTool::Pointer buildInternalTool();
  void determineToolsCalibration();
  void internalAttachedToTracker(bool value);
  void internalTracked(bool value);
  void internalVisible(bool value);
  void addLogging(); ///< adds igstk logging to the internal igstk trackertool

  Tool::InternalStructure                         mInternalStructure;   ///< the structure that defines the tool characteristics
  igstk::TrackerTool::Pointer                     mTool;                ///< pointer to the base class of the igstk tool
  IgstkToolWeakPtr                                mReferenceTool;       ///< the tool that is used as a reference to the tracking system
  TrackerWeakPtr                                  mTracker;             ///< the tracker this tool belongs to
  itk::ReceptorMemberCommand<IgstkTool>::Pointer  mToolObserver;        ///< observer listening for igstk events
  igstk::Logger::Pointer                          mLogger;              ///< logging the internal igstk behavior
  itk::StdStreamLogOutput::Pointer                mLogOutput;           ///< output to write the log to

  //internal states
  //TODO is volatile enough for thread safety?
  volatile bool mValid;              ///< whether or not the tool is valid or not
  volatile bool mVisible;            ///< whether or not the tool is visible to the tracking system
  volatile bool mAttachedToTracker;  ///< whether the tool is attached to a tracker or not
  volatile bool mTracked;            ///< whether the tool is being tracked or not

};

}

#endif /* CXIGSTKTOOL_H_ */
