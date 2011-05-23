/**
 * cxIgstkToolManager.cpp
 *
 * \brief
 *
 * \date Mar 17, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

#include <cxIgstkToolManager.h>

#include "sscMessageManager.h"

namespace cx
{

IgstkToolManager::IgstkToolManager(IgstkTracker::InternalStructure trackerStructure, std::vector<IgstkTool::InternalStructure> toolStructures, IgstkTool::InternalStructure referenceToolStructure) :
    mInitAnsweres(0),
    mInternalInitialized(false)
{
  mTimer = 0;

  this->createTracker(trackerStructure);
  this->createTools(toolStructures, referenceToolStructure);
  this->setReferenceAndTrackerOnTools();

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

void IgstkToolManager::setReferenceAndTrackerOnTools()
{
  if(!mReferenceTool)
  {
    ssc::messageManager()->sendDebug("Tracking is configured without a reference tool.");
  }

  std::map<QString, IgstkToolPtr>::iterator it;
  for(it = mTools.begin(); it != mTools.end(); ++it)
  {
    if(mReferenceTool)
      it->second->setReference(mReferenceTool);
    if(mTracker)
      it->second->setTracker(mTracker);
  }
}

void IgstkToolManager::createTracker(IgstkTracker::InternalStructure trackerStructure)
{
  TrackerPtr tracker(new IgstkTracker(trackerStructure));
  if(tracker->isValid())
    mTracker = tracker;
  else
    ssc::messageManager()->sendWarning("Invalid tracker.");
}

void IgstkToolManager::createTools(std::vector<IgstkTool::InternalStructure> toolStructures, IgstkTool::InternalStructure referenceToolStructure)
{
  for (unsigned i=0; i<toolStructures.size(); ++i)
  {
    this->addIgstkTools(toolStructures[i]);
  }
  IgstkToolPtr refTool = this->addIgstkTools(referenceToolStructure);
  if(refTool->isValid())
    mReferenceTool = refTool;
}

IgstkToolPtr IgstkToolManager::addIgstkTools(IgstkTool::InternalStructure& toolStructure)
{
  IgstkToolPtr igstkTool(new IgstkTool(toolStructure));
  if(igstkTool->isValid())
  {
    QMutexLocker sentry(&mToolMutex);
    mTools[igstkTool->getUid()] = igstkTool;
    connect(igstkTool.get(), SIGNAL(attachedToTracker(bool)), this, SLOT(deviceInitializedSlot(bool)));
  } else
  {
    ssc::messageManager()->sendWarning(toolStructure.mUid+" is not valid.");
  }
  return igstkTool;
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
    if(!mTracker->isOpen())
    {
      mTracker->open();
      connect(mTracker, SIGNAL(open(bool)), this, SLOT(attachToolsWhenTrackerIsOpenSlot(bool)));
    }else
      mTracker->attachTools(mTools);
  }else
  {
    if(mTracker->isOpen())
    {
      mTracker->detachTools(mTools); //not sure we have to detach all tools before we close, read NDI manual
      mTracker->close();
    }
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

  std::map<QString, IgstkToolPtr>::iterator it = mTools.begin();
  for(;it != mTools.end();++it)
  {
    if(!it->second)
      continue;
    if(mReferenceTool)
      it->second->getPointer()->RequestComputeTransformTo(mReferenceTool->getPointer());
    else
      it->second->getPointer()->RequestComputeTransformTo(mTracker->getPointer());
  }
}

void IgstkToolManager::deviceInitializedSlot(bool deviceInit)
{
  int numberOfDevices = mTools.size() + 1; //+1 is the tracker

  if(deviceInit)
  {
    mInitAnsweres ++;

    if(mInitAnsweres == numberOfDevices)
    {
      mInternalInitialized = true;
      emit initialized(true);
    }
  }else
  {
    mInitAnsweres--;

    if(mInitAnsweres < numberOfDevices)
    {
      if(mInternalInitialized)
      //{
        //mInitAnsweres = 0;
        emit initialized(false);
      //}

      mInternalInitialized = false;
    }
  }
}

void IgstkToolManager::attachToolsWhenTrackerIsOpenSlot(bool open)
{
  if(!open)
    return;

  disconnect(mTracker, SIGNAL(open(bool)), this, SLOT(attachToolsWhenTrackerIsOpenSlot(bool)));
  mTracker->attachTools(mTools);
}

}
