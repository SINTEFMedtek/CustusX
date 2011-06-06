#ifndef CXIGSTKTOOLMANAGER_H_
#define CXIGSTKTOOLMANAGER_H_

#include <map>
#include <vector>
#include <QThread>
#include <QTimer>
#include <QMutex>
#include "cxIgstkTracker.h"
#include "cxTool.h"
#include "cxIgstkTool.h"

namespace cx
{

/**
 * \class IgstkToolManager
 *
 * \brief
 *
 * \date Mar 17, 2011
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class IgstkToolManager: public QObject
{
  Q_OBJECT

public:
  IgstkToolManager(IgstkTracker::InternalStructure trackerStructure, std::vector<IgstkTool::InternalStructure> toolStructures, IgstkTool::InternalStructure referenceToolStructure);
  virtual ~IgstkToolManager();

  std::map<QString, IgstkToolPtr> getTools(); ///< ThreadSafe
  IgstkToolPtr getRefereceTool(); ///< ThreadSafe

signals:
  void initialized(bool on); ///< when all trackers and tools are initialized == true, else false
  void tracking(bool on);
  void error();

public slots:
  void initializeSlot(bool on); ///< connects to the hardware
  void trackSlot(bool on); ///< tracking on or off

private slots:
  void trackerTrackingSlot(bool);
  void checkTimeoutsAndRequestTransformSlot();
  void deviceInitializedSlot(bool);
  void attachToolsWhenTrackerIsInitializedSlot(bool);

private:
  void createTracker(IgstkTracker::InternalStructure trackerStructure);
  void createTools(std::vector<IgstkTool::InternalStructure> toolStructures, IgstkTool::InternalStructure referenceToolStructure);
  IgstkToolPtr addIgstkTools(IgstkTool::InternalStructure& toolStructure);
  void setReferenceAndTrackerOnTools();

  void printStatus(); ///< just for debugging

  TrackerPtr mTracker;
  QMutex mToolMutex; ///< protects mTools
  std::map<QString, IgstkToolPtr> mTools;
  int mInitAnsweres; ///< keeps track of how many tools and trackers have gotten an answer from the hardware
  QMutex mReferenceMutex; ///< protects mReferenceTool
  IgstkToolPtr mReferenceTool;

  QTimer* mTimer; ///< timer controlling the demand of transforms
  igstk::PulseGenerator::Pointer mPulseGenerator;

  bool mInternalInitialized;

};
typedef boost::shared_ptr<IgstkToolManager> IgstkToolManagerPtr;

}

#endif /* CXIGSTKTOOLMANAGER_H_ */
