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
#include "cxTrackingSystemPlaybackService.h"


#include "cxReporter.h"
#include "cxTime.h"
#include "cxPlaybackTime.h"
#include "cxPlaybackTool.h"
#include <QApplication>
#include "cxUtilHelpers.h"

namespace cx
{

TrackingSystemPlaybackService::TrackingSystemPlaybackService(PlaybackTimePtr controller, TrackingSystemServicePtr base)
{
	mBase = base;
	mState = Tool::tsNONE;

	if (controller)
		this->start(controller);
	else
		this->stop();


//	if (!controller)
//	{
//		mTools.clear();
//		mState = Tool::tsNONE;
//		report("Closed Playback Mode");
//		emit stateChanged();
////		this->closePlayBackMode();
//		return;
//	}

//	// attempt to configure tracker if not configured,
//	// or deinit/stop tracking if on.
//	if (mBase->getState() != Tool::tsCONFIGURED)
//	{
//		this->setState(Tool::tsCONFIGURED);
//		this->waitForState(mBase, Tool::tsCONFIGURED, 200);
//	}

//	if (!this->getState() <= Tool::tsCONFIGURED)
//	{
//		reportWarning("ToolManager must be configured before setting playback");
//		return;
//	}

//	std::vector<Tool> original = mBase->getTools();
////	mBase->setEnabled(false); // do we need this?? we are only in the configured state for the base, but this can be changed...
////	ToolManager::ToolMap original = mTools; ///< all tools
////	mTools.clear();

//	std::pair<double,double> timeRange(getMilliSecondsSinceEpoch(), 0);

//	for (ToolManager::ToolMap::iterator iter = original.begin(); iter!=original.end(); ++iter)
//	{
////		if (iter->second==mManualTool)
////			continue; // dont wrap the manual tool
//		cx::PlaybackToolPtr current(new PlaybackTool(iter->second, controller));
//		mTools.push_back(current);

//		TimedTransformMapPtr history = iter->second->getPositionHistory();
//		if (!history->empty())
//		{
//			timeRange.first = std::min(timeRange.first, history->begin()->first);
//			timeRange.second = std::max(timeRange.second, history->rbegin()->first);
//		}
//	}
////	mTools[mManualTool->getUid()] = mManualTool;

//	controller->initialize(QDateTime::fromMSecsSinceEpoch(timeRange.first), timeRange.second - timeRange.first);

//	report("Opened Playback Mode");
////	mPlayBackMode = true;
////	emit initialized();
//	mState = Tool::tsTRACKING;
//	emit stateChanged();

}

TrackingSystemPlaybackService::~TrackingSystemPlaybackService()
{

}

void TrackingSystemPlaybackService::start(PlaybackTimePtr controller)
{
	if (!this->forceBaseToConfiguredState())
	{
		reportWarning("ToolManager must be configured before setting playback");
		return;
	}

	std::vector<ToolPtr> original = mBase->getTools();
//	mBase->setEnabled(false); // do we need this?? we are only in the configured state for the base, but this can be changed...
//	ToolManager::ToolMap original = mTools; ///< all tools
//	mTools.clear();

	std::pair<double,double> timeRange(getMilliSecondsSinceEpoch(), 0);

	for (unsigned i=0; i<original.size(); ++i)
	{
//		if (iter->second==mManualTool)
//			continue; // dont wrap the manual tool
		cx::PlaybackToolPtr current(new PlaybackTool(original[i], controller));
		mTools.push_back(current);

		TimedTransformMapPtr history = original[i]->getPositionHistory();
		if (!history->empty())
		{
			timeRange.first = std::min(timeRange.first, history->begin()->first);
			timeRange.second = std::max(timeRange.second, history->rbegin()->first);
		}
	}
//	mTools[mManualTool->getUid()] = mManualTool;

	controller->initialize(QDateTime::fromMSecsSinceEpoch(timeRange.first), timeRange.second - timeRange.first);

	report("Opened Playback Mode");
//	mPlayBackMode = true;
//	emit initialized();
	mState = Tool::tsTRACKING;
	emit stateChanged();
}

void TrackingSystemPlaybackService::stop()
{
	mTools.clear();
	mState = Tool::tsNONE;
	report("Closed Playback Mode");
	emit stateChanged();
}

bool TrackingSystemPlaybackService::forceBaseToConfiguredState()
{
	// attempt to configure tracker if not configured,
	// or deinit/stop tracking if on.
	if (mBase->getState() != Tool::tsCONFIGURED)
	{
		this->setState(Tool::tsCONFIGURED);
		this->waitForState(mBase, Tool::tsCONFIGURED, 200);
	}

	return (mBase->getState() >= Tool::tsCONFIGURED);
}

/**
 *  Wait until timeout ms for the system to reach state.
 */
void TrackingSystemPlaybackService::waitForState(TrackingSystemServicePtr base, Tool::State state, int timeout)
{
	int interval = 10;
	for (unsigned i=0; i<timeout/interval; ++i)
	{
		if (base->getState() == state)
			break;
		qApp->processEvents();
		sleep_ms(interval);
//		mTrackerThread->wait(interval);
	}
}

std::vector<ToolPtr> TrackingSystemPlaybackService::getTools()
{
	std::vector<ToolPtr> retval;
	std::copy(mTools.begin(), mTools.end(), back_inserter(retval));
	return retval;
}

Tool::State TrackingSystemPlaybackService::getState() const
{
	return mState;
}

void TrackingSystemPlaybackService::setState(const Tool::State val)
{
	return; // cannot modify state, always on.

//	if (mState==val)
//		return;
//	mState = val;

////	if (mState>=Tool::tsTRACKING)
////	{
////		for (unsigned i=0; i<mTools.size(); ++i)
////		{
////			mTools[i]->setVisible(true);
////			mTools[i]->startTracking(30);
////		}
////	}
////	else
////	{
////		for (unsigned i=0; i<mTools.size(); ++i)
////		{
////			mTools[i]->setVisible(false);
////			mTools[i]->stopTracking();
////		}
////	}

//	emit stateChanged();
}

void TrackingSystemPlaybackService::setLoggingFolder(QString loggingFolder)
{

}

TrackerConfigurationPtr TrackingSystemPlaybackService::getConfiguration()
{
	return TrackerConfigurationPtr();
}


} // namespace cx
