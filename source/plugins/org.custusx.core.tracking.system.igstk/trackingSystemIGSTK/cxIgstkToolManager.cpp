/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxIgstkToolManager.h"

#include "cxLogger.h"
#include "cxTypeConversions.h"

namespace cx
{

void sampleInfo2xml(const igstk::NDITracker::TrackingSampleInfo& info, QDomElement& node)
{
	node.setAttribute("timestamp", QString("%1").arg(info.m_TimeStamp, 0, 'f', 0));
	node.setAttribute("error", QString("%1").arg(info.m_Error, 0, 'f', 3));
	node.setAttribute("frame", QString("%1").arg(info.m_FrameNumber));
	node.setAttribute("portstatus", QString("0b%1").arg(info.m_PortStatus, 16, 2, QChar('0')));
	node.setAttribute("toolinformation", QString("0b%1").arg(info.m_ToolInformation, 8, 2, QChar('0')));
	QString markers;
	for (unsigned i=0; i<info.m_MarkerInformation.size(); ++i)
		markers += QString::number(info.m_MarkerInformation[i]);
	node.setAttribute("markers", markers);
}


IgstkToolManager::IgstkToolManager(ToolFileParser::TrackerInternalStructure trackerStructure,
								std::vector<ToolFileParser::ToolInternalStructurePtr> toolStructures,
								ToolFileParser::ToolInternalStructurePtr referenceToolStructure) :
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
}

IgstkToolManager::~IgstkToolManager()
{
	this->trackSlot(false);
	this->initializeSlot(false);
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

void IgstkToolManager::createTools(std::vector<ToolFileParser::ToolInternalStructurePtr> toolStructures,
								ToolFileParser::ToolInternalStructurePtr referenceToolStructure)
{
	for (unsigned i = 0; i < toolStructures.size(); ++i)
	{
		this->addIgstkTools(toolStructures[i]);
	}
	if (referenceToolStructure && !referenceToolStructure->mUid.isEmpty())
	{
		IgstkToolPtr refTool = this->addIgstkTools(referenceToolStructure);
		if (refTool->isValid())
			mReferenceTool = refTool;
	}
}

IgstkToolPtr IgstkToolManager::addIgstkTools(ToolFileParser::ToolInternalStructurePtr toolStructure)
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
		reportWarning(toolStructure->mUid + " is not valid.");
	}
	return igstkTool;
}

void IgstkToolManager::trackerTrackingSlot(bool isTracking)
{
	int igstkPulsingDriveRate = 10;
	if (isTracking)
		mTimer->start(igstkPulsingDriveRate);
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
	igstk::PulseGenerator::CheckTimeouts();

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
	size_t numberOfDevices = mTools.size() + 1; //+1 is the tracker

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
