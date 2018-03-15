/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPlaybackTool.h"
#include "cxTime.h"
#include "cxTypeConversions.h"
#include "cxPlaybackTime.h"
#include "cxManualToolAdapter.h"

namespace cx
{

PlaybackTool::PlaybackTool(ToolPtr base, PlaybackTimePtr time) :
	ToolImpl(base->getUid(), "playback "+base->getName()), mBase(base),
    mTime(time),
    mVisible(false)
{
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

bool PlaybackTool::isInitialized() const
{
	return true;
}

void PlaybackTool::set_prMt(const Transform3D& prMt, double timestamp)
{

}

void PlaybackTool::setVisible(bool vis)
{

}


} /* namespace cx */
