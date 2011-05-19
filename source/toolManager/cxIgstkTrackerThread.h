#ifndef CXIGSTKTRACKERTHREAD_H_
#define CXIGSTKTRACKERTHREAD_H_

#include <map>
#include <vector>
#include <QThread>
#include <QTimer>
#include "cxIgstkTracker.h"
#include "cxIgstkTrackerThread.h"
#include "cxTool.h"
#include "cxIgstkTool.h"
#include <cxIgstkToolManager.h>

namespace cx
{

/**
 * cxIgstkTrackerThread.h
 *
 * \brief
 *
 * \date Mar 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class IgstkTrackerThread: public QThread
{
  Q_OBJECT

public:
  IgstkTrackerThread(IgstkTracker::InternalStructure trackerStructure, std::vector<IgstkTool::InternalStructure> toolStructures, IgstkTool::InternalStructure referenceToolStructure);
  virtual ~IgstkTrackerThread();

  void initialize(bool on); ///< connects to the hardware. Threadsafe.
  void track(bool on); ///< tracking on or off. Threadsafe.

  std::map<QString, IgstkToolPtr> getTools(); ///< ThreadSafe.
  IgstkToolPtr getRefereceTool(); ///< ThreadSafe

signals:
  void configured(bool on);
  void initialized(bool on); ///< system is initialized
  void tracking(bool on);

  void requestInitialize(bool on); ///< internal signal
  void requestTrack(bool on); ///< internal signal

private slots:

private:
  virtual void run();

  IgstkTracker::InternalStructure mInitTrackerStructure;
  std::vector<IgstkTool::InternalStructure> mInitToolStructures;
  IgstkTool::InternalStructure mInitReferenceToolStructure;

  IgstkToolManagerPtr mManager;
};
typedef boost::shared_ptr<IgstkTrackerThread> IgstkTrackerThreadPtr;

}

#endif /* CXIGSTKTRACKERTHREAD_H_ */
