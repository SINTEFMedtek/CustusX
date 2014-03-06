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
#include "cxTrackingPositionFilter.h"

namespace cx
{

TrackingPositionFilter::TrackingPositionFilter()
{
}

void TrackingPositionFilter::addPosition(Transform3D pos, double timestamp)
{
	this->clearIfTimestampIsOlderThanHead(timestamp);

	mHistory[timestamp] = pos;
}

Transform3D TrackingPositionFilter::getFilteredPosition()
{
	if (mHistory.empty())
		return Transform3D::Identity();
	return mHistory.rbegin()->second;
}

void TrackingPositionFilter::clearIfTimestampIsOlderThanHead(double timestamp)
{
	if (mHistory.empty())
		return;
	if (timestamp < mHistory.rbegin()->first) // clear history if old timestamps appear
		mHistory.clear();
}

} // namespace cx


