/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxIgstkToolManager.h"

#include "cxLogger.h"
#include "cxTypeConversions.h"

namespace cx
{

IgstkToolManager::IgstkToolManager(ToolFileParser::TrackerInternalStructure trackerStructure,
                std::vector<ToolFileParser::ToolInternalStructure> toolStructures,
                ToolFileParser::ToolInternalStructure referenceToolStructure) :
				mInitAnsweres(0), mInternalInitialized(false)
{
	mTimer = 0;

	this->createTracker(trackerStructure);
	this->createTools(toolStructures, referenceToolStructure);
	this->setReferenceAndTrackerOnTools();

	connect(mTracker.get(), SIGNAL(tracking(bool)), this, SIGNAL(tracking(bool)));
	connect(mTracker.get(), SIGNAL(error()), this, SIGNAL(error()));

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
	if (!mReferenceTool)
	{
		reportWarning("Tracking is configured without a reference tool.");
	}

	std::map<QString, IgstkToolPtr>::iterator it;
	for (it = mTools.begin(); it != mTools.end(); ++it)
	{
		if (mReferenceTool)
			it->second->setReference(mReferenceTool);
		if (mTracker)
			it->second->setTracker(mTracker);
	}
}

void IgstkToolManager::createTracker(ToolFileParser::TrackerInternalStructure trackerStructure)
{
	TrackerPtr tracker(new IgstkTracker(trackerStructure));
	if (tracker->isValid())
		mTracker = tracker;
	else
		reportWarning("Invalid tracker.");
}

void IgstkToolManager::createTools(std::vector<ToolFileParser::ToolInternalStructure> toolStructures,
                ToolFileParser::ToolInternalStructure referenceToolStructure)
{
	for (unsigned i = 0; i < toolStructures.size(); ++i)
	{
		this->addIgstkTools(toolStructures[i]);
	}
	if (!referenceToolStructure.mUid.isEmpty())
	{
		IgstkToolPtr refTool = this->addIgstkTools(referenceToolStructure);
		if (refTool->isValid())
			mReferenceTool = refTool;
	}
}

IgstkToolPtr IgstkToolManager::addIgstkTools(ToolFileParser::ToolInternalStructure& toolStructure)
{
	IgstkToolPtr igstkTool(new IgstkTool(toolStructure));
	if (igstkTool->isValid())
	{
		QMutexLocker sentry(&mToolMutex);
		mTools[igstkTool->getUid()] = igstkTool;
		connect(igstkTool.get(), SIGNAL(attachedToTracker(bool)), this, SLOT(deviceInitializedSlot(bool)));
	}
	else
	{
		reportWarning(toolStructure.mUid + " is not valid.");
	}
	return igstkTool;
}

void IgstkToolManager::trackerTrackingSlot(bool isTracking)
{
	if (isTracking)
		mTimer->start(33);
	else
		mTimer->stop();
}

void IgstkToolManager::initializeSlot(bool on)
{
	if (on)
	{
		connect(mTracker.get(), SIGNAL(initialized(bool)), this, SLOT(attachToolsWhenTrackerIsInitializedSlot(bool)));
		if (!mTracker->isOpen())
			mTracker->open();
	}
	else
	{
		mTracker->detachTools(mTools); //not sure we have to detach all tools before we close, read NDI manual
		if (mTracker->isOpen())
			mTracker->close();
	}
}

void IgstkToolManager::trackSlot(bool on)
{
	if (on && !mTracker->isTracking())
		mTracker->startTracking();
	else if (!on && mTracker->isTracking())
		mTracker->stopTracking();
}

void IgstkToolManager::checkTimeoutsAndRequestTransformSlot()
{
	mPulseGenerator->CheckTimeouts();

	std::map<QString, IgstkToolPtr>::iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
	{
		if (!it->second)
			continue;
		if (mReferenceTool)
			it->second->getPointer()->RequestComputeTransformTo(mReferenceTool->getPointer());
		else
			it->second->getPointer()->RequestComputeTransformTo(mTracker->getPointer());
	}
}

void IgstkToolManager::deviceInitializedSlot(bool deviceInit)
{
	int numberOfDevices = mTools.size() + 1; //+1 is the tracker

	if (deviceInit)
	{
		mInitAnsweres++;

		if (mInitAnsweres == numberOfDevices)
		{
			mInternalInitialized = true;
			emit initialized(true);
		}
	}
	else
	{
		mInitAnsweres--;

		if (mInitAnsweres < numberOfDevices)
		{
			if (mInternalInitialized)
			{
				mInitAnsweres = 0;
				mInternalInitialized = false;
				emit initialized(false);
			}
		}
	}
}

void IgstkToolManager::attachToolsWhenTrackerIsInitializedSlot(bool open)
{
	if (!open)
		return;

	disconnect(mTracker.get(), SIGNAL(initialized(bool)), this, SLOT(attachToolsWhenTrackerIsInitializedSlot(bool)));
	mTracker->attachTools(mTools);
}

void IgstkToolManager::printStatus()
{
	std::cout << "mInternalInitialized " << mInternalInitialized << std::endl;
	std::cout << "mInitAnsweres " << mInitAnsweres << std::endl;
	std::cout << "mTracker->isValid() " << mTracker->isValid() << std::endl;
	std::cout << "mTracker->isOpen() " << mTracker->isOpen() << std::endl;
	std::cout << "mTracker->isInitialized() " << mTracker->isInitialized() << std::endl;
	std::cout << "mTracker->isTracking() " << mTracker->isTracking() << std::endl;
	std::cout << "mTools.size() " << string_cast(mTools.size()) << std::endl;
}
}
