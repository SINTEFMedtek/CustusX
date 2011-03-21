/**
 * cxIgstkToolManager.cpp
 *
 * \brief
 *
 * \date Mar 17, 2011
 * \author jbake
 */

#include <cxIgstkToolManager.h>

#include "sscMessageManager.h"

namespace cx
{

IgstkToolManager::IgstkToolManager(IgstkTracker::InternalStructure trackerStructure, std::vector<Tool::InternalStructure> toolStructures) :
    mInitAnsweres(0)
{
  mTimer = 0;

  this->createTracker(trackerStructure);
  this->createTools(toolStructures);
  this->setReference();

  connect(mTracker.get(), SIGNAL(tracking(bool)), this, SIGNAL(tracking(bool)));

  connect(mTracker.get(), SIGNAL(initialized(bool)), this, SLOT(deviceInitializedSlot(bool)));
  connect(mTracker.get(), SIGNAL(tracking(bool)), this, SLOT(trackerTrackingSlot(bool)));

  mTimer = new QTimer();
  connect(mTimer, SIGNAL(timeout()), this, SLOT(checkTimeoutsAndRequestTransformSlot()));

  igstk::RealTimeClock::Initialize();

  mPulseGenerator = igstk::PulseGenerator::New();
  mPulseGenerator->RequestSetFrequency(30.0);
  mPulseGenerator->RequestStart();
}

IgstkToolManager::~IgstkToolManager()
{
  mTracker->stopTracking();
}

std::map<QString, IgstkToolPtr> IgstkToolManager::getTools()
{
  QMutexLocker sentry(&mToolMutex);
  return mTools;
}

void IgstkToolManager::setReference()
{
  std::map<QString, IgstkToolPtr>::iterator it;
  for(it = mTools.begin(); it != mTools.end(); ++it)
  {
    if(it->second->getType() == ssc::Tool::TOOL_REFERENCE)
      mReferenceTool = it->second;
  }

  for(it = mTools.begin(); it != mTools.end(); ++it)
    it->second->setReference(mReferenceTool);
}

void IgstkToolManager::createTracker(IgstkTracker::InternalStructure trackerStructure)
{
  TrackerPtr tracker(new IgstkTracker(trackerStructure));
  if(tracker->isValid())
    mTracker = tracker;
  else
    ssc::messageManager()->sendWarning("Invalid tracker.");
}

void IgstkToolManager::createTools(std::vector<Tool::InternalStructure> toolStructures)
{
  for (unsigned i=0; i<toolStructures.size(); ++i)
  {
    IgstkToolPtr igstkTool(new IgstkTool(toolStructures[i]));
    if(igstkTool->isValid())
    {
      QMutexLocker sentry(&mToolMutex);
      mTools[igstkTool->getUid()] = igstkTool;
      connect(igstkTool.get(), SIGNAL(attachedToTracker(bool)), this, SLOT(deviceInitializedSlot(bool)));
    } else
    {
     ssc::messageManager()->sendWarning("Tool: "+toolStructures[i].mUid+" is not valid.");
    }
  }
}

void IgstkToolManager::trackerTrackingSlot(bool isTracking)
{
  if(isTracking)
    mTimer->start(33);
  else
    mTimer->stop();
}

void IgstkToolManager::initializeSlot(bool on)
{
  if(on)
  {
    mTracker->open();
    mTracker->attachTools(mTools);
  }else
  {
    mTracker->detachTools(mTools); //not sure we have to detach all tools before we close, read NDI manual
    mTracker->close();
  }
}

void IgstkToolManager::trackSlot(bool on)
{
  if(on)
    mTracker->startTracking();
  else
    mTracker->stopTracking();
}

void IgstkToolManager::checkTimeoutsAndRequestTransformSlot()
{
  mPulseGenerator->CheckTimeouts();

  if (!mReferenceTool) // no need to request extra transforms from tools to the tracker, its already done
    return;

  std::map<QString, IgstkToolPtr>::iterator it = mTools.begin();
  for(;it != mTools.end();++it)
  {
    if(!mReferenceTool || !it->second)
      continue;
    it->second->getPointer()->RequestComputeTransformTo(mReferenceTool->getPointer());
  }
}

void IgstkToolManager::deviceInitializedSlot(bool value)
{
  mInitAnsweres ++;

  int numberOfDevices = mTools.size() + 1; //+1 is the tracker

  if(mInitAnsweres == numberOfDevices)
    emit initialized(true);
}
}
