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


#include "cxLogger.h"
#include "cxTime.h"
#include "cxPlaybackTime.h"
#include "cxPlaybackTool.h"
#include <QApplication>
#include "cxUtilHelpers.h"
#include "cxManualTool.h"
#include "cxTrackerConfiguration.h"

namespace cx
{

TrackingSystemPlaybackService::TrackingSystemPlaybackService(PlaybackTimePtr controller, std::vector<TrackingSystemServicePtr> base, ManualToolPtr manual)
{
	mBases = base;
	mState = Tool::tsNONE;
	mController = controller;
	mManual = manual;
}

TrackingSystemPlaybackService::~TrackingSystemPlaybackService()
{
	this->setState(Tool::tsNONE);
}

void TrackingSystemPlaybackService::start()
{
	if (!this->forceBaseToConfiguredState())
	{
		reportWarning("ToolManager must be configured before setting playback");
		return;
	}

	std::vector<ToolPtr> original;
	for (unsigned i=0; i<mBases.size(); ++i)
	{
		std::vector<ToolPtr> original_one = mBases[i]->getTools();
		std::copy(original_one.begin(), original_one.end(), std::back_inserter(original));
	}

	std::pair<double,double> timeRange(getMilliSecondsSinceEpoch(), 0);

	for (unsigned i=0; i<original.size(); ++i)
	{
		cx::PlaybackToolPtr current(new PlaybackTool(original[i], mController));
		connect(current.get(), &Tool::toolTransformAndTimestamp, this, &TrackingSystemPlaybackService::onToolPositionChanged);
		mTools.push_back(current);

		TimedTransformMapPtr history = original[i]->getPositionHistory();
		if (!history->empty())
		{
			timeRange.first = std::min(timeRange.first, history->begin()->first);
			timeRange.second = std::max(timeRange.second, history->rbegin()->first);
		}
	}

	mController->initialize(QDateTime::fromMSecsSinceEpoch(timeRange.first), timeRange.second - timeRange.first);

	report("Opened Playback Mode");
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
	bool atLeastOneConfigured = false;

	for (unsigned i=0; i<mBases.size(); ++i)
	{
		// attempt to configure tracker if not configured,
		// or deinit/stop tracking if on.
		if (mBases[i]->getState() != Tool::tsCONFIGURED)
		{
			mBases[i]->setState(Tool::tsCONFIGURED);
			this->waitForState(mBases[i], Tool::tsCONFIGURED, 200);
		}

		bool configured = (mBases[i]->getState() >= Tool::tsCONFIGURED);
		atLeastOneConfigured = atLeastOneConfigured || configured;
	}

	return atLeastOneConfigured;
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

bool TrackingSystemPlaybackService::isRunning() const
{
	return !mTools.empty();
}

void TrackingSystemPlaybackService::setState(const Tool::State val)
{
	mState = val;

	if (mState >= Tool::tsCONFIGURED)
	{
		if (!this->isRunning())
			this->start();
	}
	else
	{
		if (this->isRunning())
			this->stop();
	}
}

void TrackingSystemPlaybackService::setLoggingFolder(QString loggingFolder)
{

}

TrackerConfigurationPtr TrackingSystemPlaybackService::getConfiguration()
{
	return TrackerConfiguration::getNullObject();
}

void TrackingSystemPlaybackService::onToolPositionChanged(Transform3D matrix, double timestamp)
{
	// Overwrite manual tool pos, set timestamp to 1ms previous.
	// This makes sure manual tool is not picked as active.
	mManual->set_prMt(matrix, timestamp-1);
//	mManual->setVisible(false);
//	mManual->setVisible(true);
}



} // namespace cx
