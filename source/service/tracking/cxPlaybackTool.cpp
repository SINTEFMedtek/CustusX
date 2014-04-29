// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxPlaybackTool.h"
#include "cxTime.h"
#include "cxTypeConversions.h"
#include "cxToolManager.h"
#include "cxPlaybackTime.h"
#include "cxManualToolAdapter.h"

namespace cx
{

PlaybackTool::PlaybackTool(TrackingServicePtr manager, ToolPtr base, PlaybackTimePtr time) :
	ToolImpl(manager, "playback_"+base->getUid(), "playback "+base->getName()), mBase(base),
    mTime(time),
    mVisible(false)
{
	std::cout << "PlaybackTool::PlaybackTool " << this->getUid() << std::endl;
	connect(mTime.get(), SIGNAL(changed()), this, SLOT(timeChangedSlot()));

	connect(mBase.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));
	connect(mBase.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(tooltipOffset(double)));
}

PlaybackTool::~PlaybackTool()
{
}

void PlaybackTool::timeChangedSlot()
{
	QDateTime time = mTime->getTime();
	qint64 time_ms = time.toMSecsSinceEpoch();

	TimedTransformMapPtr positions = mBase->getPositionHistory();
	if (positions->empty())
		return;

	// find last stored time before current time.
	TimedTransformMap::iterator lastSample = positions->lower_bound(time_ms);
	if (lastSample!=positions->begin())
		--lastSample;

	// interpret as hidden if no samples has been received the last time:
	qint64 timeout = 200;
	bool visible = (lastSample!=positions->end()) && (fabs(time_ms - lastSample->first) < timeout);

	// change visibility if applicable
	if (mVisible!=visible)
	{
		mVisible = visible;
		emit toolVisible(mVisible);
	}

	// emit new position if visible
	if (this->getVisible())
	{
		m_rMpr = lastSample->second;
		mTimestamp = lastSample->first;
		emit toolTransformAndTimestamp(m_rMpr, mTimestamp);
	}

	// Overwrite manual tool pos, set timestamp to 1ms previous.
	// This makes sure manual tool is not picked as dominant.
	if (this->getTrackingService())
	{
		this->getTrackingService()->getManualTool()->set_prMt(m_rMpr, mTimestamp-1);
		this->getTrackingService()->dominantCheckSlot();
	}
}

std::set<Tool::Type> PlaybackTool::getTypes() const
{
	return mBase->getTypes();
}

vtkPolyDataPtr PlaybackTool::getGraphicsPolyData() const
{
	return mBase->getGraphicsPolyData();
}

Transform3D PlaybackTool::get_prMt() const
{
	return m_rMpr;
}

bool PlaybackTool::getVisible() const
{
	return mVisible;
}

QString PlaybackTool::getUid() const
{
	return mUid;
}

QString PlaybackTool::getName() const
{
	return mName;
}

bool PlaybackTool::isCalibrated() const
{
	return mBase->isCalibrated();
}

ProbeDefinition PlaybackTool::getProbeSector() const
{
	return mBase->getProbeSector();
}

double PlaybackTool::getTimestamp() const
{
	return mTimestamp;
}

// Just use the tool tip offset from the tool manager
double PlaybackTool::getTooltipOffset() const
{
	return mBase->getTooltipOffset();
}

// Just use the tool tip offset from the tool manager
void PlaybackTool::setTooltipOffset(double val)
{
	mBase->setTooltipOffset(val);
}

Transform3D PlaybackTool::getCalibration_sMt() const
{
	return mBase->getCalibration_sMt();
}

std::map<int, Vector3D> PlaybackTool::getReferencePoints() const
{
	return mBase->getReferencePoints();
}


} /* namespace cx */
