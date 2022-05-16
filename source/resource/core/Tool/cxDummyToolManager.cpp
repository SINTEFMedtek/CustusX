/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxDummyToolManager.h"

namespace cx
{

DummyToolManager::DummyToolManagerPtr DummyToolManager::create()
{
	DummyToolManagerPtr retval;
	retval.reset(new DummyToolManager());
	return retval;
}

DummyToolManager::DummyToolManager() :
	m_rMpr(Transform3D::Identity()),
//	mToolTipOffset(0),
//	mConfigured(false),
//	mInitialized(false),
//	mIsTracking(false),
	mState(Tool::tsNONE)
{
	DummyToolPtr tool1(new DummyTool());

	mActiveTool = tool1;
	mReferenceTool = tool1;

	mDummyTools.insert(std::pair<QString, DummyToolPtr>(tool1->getUid(), tool1));
}
DummyToolManager::~ DummyToolManager()
{}

Tool::State DummyToolManager::getState() const
{
	return mState;
}

void DummyToolManager::setState(const Tool::State val)
{
	if (val==mState)
		return;

	if (val==Tool::tsTRACKING)
		this->startTracking();
	else if (mState==Tool::tsTRACKING)
		this->stopTracking();

	mState = val;
	emit stateChanged();
}

void DummyToolManager::startTracking()
{
	DummyToolMapConstIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		((*it).second)->startTracking();
		++it;
	}

}

void DummyToolManager::stopTracking()
{
	DummyToolMapConstIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		((*it).second)->stopTracking();
		++it;
	}
}

TrackingService::ToolMap DummyToolManager::getTools()
{
	return TrackingService::ToolMap(mDummyTools.begin(), mDummyTools.end());
}

ToolPtr DummyToolManager::getTool(const QString& uid)
{
	DummyToolMapConstIter it = mDummyTools.find(uid);
	return (*it).second;
}

ToolPtr DummyToolManager::getActiveTool()
{
	return mActiveTool;
}
void DummyToolManager::setActiveTool(const QString& uid)
{
	DummyToolMapConstIter it = mDummyTools.find(uid);
	mActiveTool = (*it).second;
	emit activeToolChanged(uid);
}

void DummyToolManager::clearActiveTool()
{
	mActiveTool = DummyToolPtr();
}

Transform3D DummyToolManager::get_rMpr() const
{
	return m_rMpr;
}

void DummyToolManager::set_rMpr(const Transform3D& val)
{
	m_rMpr = val;
}

ToolPtr DummyToolManager::getReferenceTool() const
{
	return mReferenceTool;
}

void DummyToolManager::addTool(DummyToolPtr tool)
{
	mDummyTools.insert(std::make_pair(tool->getUid(), tool));
}

//void DummyToolManager::setTooltipOffset(double offset)
//{
//	if (similar(offset, mToolTipOffset))
//		return;
//	mToolTipOffset = offset;
//	emit tooltipOffset(mToolTipOffset);
//}
//double DummyToolManager::getTooltipOffset() const
//{
//	return mToolTipOffset;
//}

void DummyToolManager::installTrackingSystem(TrackingSystemServicePtr system)
{

}

void DummyToolManager::unInstallTrackingSystem(TrackingSystemServicePtr system)
{

}

std::vector<TrackingSystemServicePtr> DummyToolManager::getTrackingSystems()
{
	return std::vector<TrackingSystemServicePtr> ();
}

void DummyToolManager::resetTimeSynchronization()
{
}

std::vector<TrackerConfigurationPtr> DummyToolManager::getConfigurations()
{
	std::vector<TrackerConfigurationPtr> retval;
	retval.push_back(TrackerConfigurationPtr());
	return retval;
}

/*TrackerConfigurationPtr DummyToolManager::getConfiguration(QString trackingSystemImplementation)
{
	return TrackerConfigurationPtr();
}*/

TrackerConfigurationPtr DummyToolManager::getConfiguration()
{
	return TrackerConfigurationPtr();
}

void DummyToolManager::setCurrentTrackingSystemImplementation(QString trackingSystemImplementation)
{
}

QString DummyToolManager::getCurrentTrackingSystemImplementation()
{
	return QString();
}

} //namespace cx
