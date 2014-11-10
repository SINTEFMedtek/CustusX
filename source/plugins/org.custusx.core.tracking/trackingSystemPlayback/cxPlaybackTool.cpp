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
