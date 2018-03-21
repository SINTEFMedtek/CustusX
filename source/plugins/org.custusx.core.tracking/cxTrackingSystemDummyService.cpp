/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxTrackingSystemDummyService.h"

#include "cxDummyTool.h"
#include "cxLogger.h"
#include "cxTrackerConfiguration.h"

namespace cx
{

TrackingSystemDummyService::TrackingSystemDummyService(DummyToolPtr tool)
{
	mState = Tool::tsINITIALIZED;

	if (tool)
	{
		mTools.push_back(tool);
		report("Running dummy tool " + tool->getUid());
	}
}

TrackingSystemDummyService::~TrackingSystemDummyService()
{

}

std::vector<ToolPtr> TrackingSystemDummyService::getTools()
{
	std::vector<ToolPtr> retval;
	std::copy(mTools.begin(), mTools.end(), back_inserter(retval));
	return retval;
}

Tool::State TrackingSystemDummyService::getState() const
{
	return mState;
}

void TrackingSystemDummyService::setState(const Tool::State val)
{
	if (mState==val)
		return;
	mState = val;

	if (mState>=Tool::tsTRACKING)
	{
		for (unsigned i=0; i<mTools.size(); ++i)
		{
			mTools[i]->setVisible(true);
			mTools[i]->startTracking(30);
		}
	}
	else
	{
		for (unsigned i=0; i<mTools.size(); ++i)
		{
			mTools[i]->setVisible(false);
			mTools[i]->stopTracking();
		}
	}

	emit stateChanged();
}

void TrackingSystemDummyService::setLoggingFolder(QString loggingFolder)
{

}

TrackerConfigurationPtr TrackingSystemDummyService::getConfiguration()
{
	return TrackerConfiguration::getNullObject();
}

//void ToolManagerUsingIGSTK::runDummyTool(DummyToolPtr tool)
//{
//	TrackingSystemServicePtr dummySystem;
//	dummySystem.reset(new TrackingSystemDummyService(tool));
//	this->installTrackingSystem(dummySystem);
//	this->setActiveTool(tool->getUid());
//	dummySystem->setState(Tool::tsTRACKING);

//	report("Running dummy tool " + tool->getUid());

//	mTools[tool->getUid()] = tool;
//	tool->setVisible(true);
//	connect(tool.get(), SIGNAL(toolVisible(bool)), this, SLOT(dominantCheckSlot()));
//	tool->startTracking(30);
//	this->setActiveTool(tool->getUid());

//	report("Dummy: Config/Init/Track started in toolManager");
//	mState = Tool::tsCONFIGURED;
//	emit configured();
//	emit stateChanged();

//	this->initializedSlot(true);
//	this->trackerTrackingSlot(true);
//}

} // namespace cx
