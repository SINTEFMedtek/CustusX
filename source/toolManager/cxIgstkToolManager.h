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

/**
 * cxIgstkToolManager.h
 *
 * \brief
 *
 * \date Mar 17, 2011
 * \author jbake
 */
namespace cx
{

/**
 * cxIgstkToolManager.h
 *
 * \brief
 *
 * \date Mar 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class IgstkToolManager: public QObject
{
  Q_OBJECT

public:
  IgstkToolManager(IgstkTracker::InternalStructure trackerStructure, std::vector<Tool::InternalStructure> toolStructures);
  virtual ~IgstkToolManager();

  std::map<QString, IgstkToolPtr> getTools(); ///< ThreadSafe
  IgstkToolPtr getRefereceTool(); ///< ThreadSafe

signals:
  void initialized(bool on); ///< trackers and tools are initialized
  void tracking(bool on);

public slots:
  void initializeSlot(bool on); ///< connects to the hardware
  void trackSlot(bool on); ///< tracking on or off

private slots:
  void trackerTrackingSlot(bool);
  void checkTimeoutsAndRequestTransformSlot();
  void deviceInitializedSlot(bool);

private:
  void createTracker(IgstkTracker::InternalStructure trackerStructure);
  void createTools(std::vector<Tool::InternalStructure> toolStructures);
    void setReference();

  TrackerPtr mTracker;
  QMutex mToolMutex; ///< protects mTools
  std::map<QString, IgstkToolPtr> mTools;
  int mInitAnsweres; ///< keeps track of how many tools and trackers have gotten an answer from the hardware
  QMutex mReferenceMutex; ///< protects mReferenceTool
  IgstkToolPtr mReferenceTool;

  QTimer* mTimer; ///< timer controlling the demand of transforms
  igstk::PulseGenerator::Pointer mPulseGenerator;

};
typedef boost::shared_ptr<IgstkToolManager> IgstkToolManagerPtr;

}

#endif /* CXIGSTKTOOLMANAGER_H_ */
