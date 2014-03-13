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
#include "cxToolManager.h"

namespace cx
{

ToolImpl::ToolImpl(TrackingServicePtr manager, const QString& uid, const QString& name) :
	Tool(uid, name),
	mPositionHistory(new TimedTransformMap()),
	m_prMt(Transform3D::Identity()),
	mManager(manager)
{

}

ToolImpl::~ToolImpl()
{

}

TrackingServicePtr ToolImpl::getTrackingService()
{
	return mManager.lock();
}
TrackingServicePtr ToolImpl::getTrackingService() const
{
	return mManager.lock();
}

// Just use the tool tip offset from the tool manager
double ToolImpl::getTooltipOffset() const
{
	if (this->getTrackingService())
		return this->getTrackingService()->getTooltipOffset();
	return 0;
}

// Just use the tool tip offset from the tool manager
void ToolImpl::setTooltipOffset(double val)
{
	if (this->getTrackingService())
		this->getTrackingService()->setTooltipOffset(val);
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

