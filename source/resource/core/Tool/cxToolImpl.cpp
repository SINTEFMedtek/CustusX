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

#include "cxToolImpl.h"

namespace cx
{

ToolImpl::ToolImpl(ToolManager* manager, const QString& uid, const QString& name) :
	Tool(uid, name),
	mPositionHistory(new TimedTransformMap()),
	m_prMt(Transform3D::Identity()),
	mManager(manager)
{

}

ToolImpl::~ToolImpl()
{

}

TimedTransformMapPtr ToolImpl::getPositionHistory()
{
	return mPositionHistory;
}

TimedTransformMap ToolImpl::getSessionHistory(double startTime, double stopTime)
{
	TimedTransformMap::iterator startIt = mPositionHistory->lower_bound(startTime);
	TimedTransformMap::iterator stopIt = mPositionHistory->upper_bound(stopTime);

	TimedTransformMap retval(startIt, stopIt);
	return retval;
}

Transform3D ToolImpl::get_prMt() const
{
	return m_prMt;
}

void ToolImpl::set_prMt(const Transform3D& prMt, double timestamp)
{
	m_prMt = prMt;
	(*mPositionHistory)[timestamp] = m_prMt;
	emit toolTransformAndTimestamp(m_prMt, timestamp);
}

void ToolImpl::resetTrackingPositionFilter(TrackingPositionFilterPtr filter)
{
	mTrackingPositionFilter = filter;
}

} // namespace cx

