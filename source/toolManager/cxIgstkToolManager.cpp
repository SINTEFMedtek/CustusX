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

  //WARNING will this work when newing several pulsegenerators in different threads????
  mPulseGenerator = igstk::PulseGenerator::New();
  mPulseGenerator->RequestSetFrequency(30.0);
  mPulseGenerator->RequestStart();
}

IgstkToolManager::~IgstkToolManager()
{
  this->trackSlot(false);
  this->initializeSlot(false);

  mPulseGenerator->RequestStop();
}

std::map<QString, IgstkToolPtr> IgstkToolManager::getTools()
{
  QMutexLocker sentry(&mToolMutex);
  return mTools;
}

IgstkToolPtr IgstkToolManager::getRefereceTool()
{
  QMutexLocker sentry(&mReferenceMutex);
  return mReferenceTool;
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
     ssc::messageManager()->sendWarning(toolStructures[i].mUid+" is not valid.");
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
  if(on && !mTracker->isInitialized())
  {
    mTracker->open();
    mTracker->attachTools(mTools);
  }else if(!on && mTracker->isInitialized())
  {
    mTracker->detachTools(mTools); //not sure we have to detach all tools before we close, read NDI manual
    mTracker->close();
  }
}

void IgstkToolManager::trackSlot(bool on)
{
  if(on && !mTracker->isTracking())
    mTracker->startTracking();
  else if(!on && mTracker->isTracking())
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
  if(value)
  {
    mInitAnsweres ++;

    int numberOfDevices = mTools.size() + 1; //+1 is the tracker

    if(mInitAnsweres == numberOfDevices)
      emit initialized(true);
  }else
  {
    mInitAnsweres--;

    if(mInitAnsweres == 0)
      emit initialized(false);
  }
}
}
