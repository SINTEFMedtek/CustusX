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
