/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
