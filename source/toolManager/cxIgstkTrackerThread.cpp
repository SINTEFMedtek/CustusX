#include <cxIgstkTrackerThread.h>
#include "sscMessageManager.h"
#include "cxIgstkTool.h"
#include <cxIgstkToolManager.h>

namespace cx
{

IgstkTrackerThread::IgstkTrackerThread(IgstkTracker::InternalStructure trackerStructure, std::vector<Tool::InternalStructure> toolStructures)
{
  mInitTrackerStructure = trackerStructure;
  mInitToolStructures = toolStructures;
}

IgstkTrackerThread::~IgstkTrackerThread()
{
}

std::map<QString, IgstkToolPtr> IgstkTrackerThread::getTools()
{
  return mManager->getTools();
}

void IgstkTrackerThread::initialize(bool on)
{
  emit requestInitialize(on);
}

void IgstkTrackerThread::track(bool on)
{
  emit requestTrack(on);
}

void IgstkTrackerThread::run()
{
  // configure
  mManager.reset(new IgstkToolManager(mInitTrackerStructure, mInitToolStructures));
  connect(mManager.get(), SIGNAL(initialized(bool)), this, SIGNAL(initialized(bool)));
  connect(mManager.get(), SIGNAL(tracking(bool)),    this, SIGNAL(tracking(bool)));
  connect(this, SIGNAL(requestInitialize(bool)), mManager.get(), SLOT(initializeSlot(bool)));
  connect(this, SIGNAL(requestTrack(bool)), mManager.get(), SLOT(trackSlot(bool)));

  emit configured(true);

  // run event loop
  this->exec();

  // cleanup/deconfigure
  mManager.reset();
  emit configured(false);
}

} //namespace cx
