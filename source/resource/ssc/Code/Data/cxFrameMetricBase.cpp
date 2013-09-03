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

#include "cxFrameMetricBase.h"
#include "sscBoundingBox3D.h"
#include "sscTypeConversions.h"

namespace cx {

FrameMetricBase::FrameMetricBase(const QString& uid, const QString& name) :
		ssc::DataMetric(uid, name),
		mSpace(ssc::SpaceHelpers::getR()),
		mFrame(ssc::Transform3D::Identity())
{
	mSpaceListener.reset(new ssc::CoordinateSystemListener(mSpace));
	connect(mSpaceListener.get(), SIGNAL(changed()), this, SIGNAL(transformChanged()));
}

FrameMetricBase::~FrameMetricBase() {
}

void FrameMetricBase::setFrame(const ssc::Transform3D& rMt)
{
	mFrame = rMt;
	emit transformChanged();
}

ssc::Transform3D FrameMetricBase::getFrame()
{
	return mFrame;
}

ssc::Vector3D FrameMetricBase::getCoordinate() const
{
	ssc::Vector3D point_t = ssc::Vector3D(0,0,0);
	return mFrame.coord(point_t);
}

/** return frame described in ref space F * sMr
  */
ssc::Transform3D FrameMetricBase::getRefFrame() const
{
	ssc::Transform3D rMq = ssc::SpaceHelpers::get_toMfrom(this->getSpace(), ssc::CoordinateSystem(ssc::csREF));
	return rMq * mFrame;
}

/** return frame described in ref space F * sMr
  */
ssc::Vector3D FrameMetricBase::getRefCoord() const
{
	ssc::Transform3D rMq = this->getRefFrame();
	ssc::Vector3D p_r = rMq.coord(ssc::Vector3D(0,0,0));
	return p_r;
}

void FrameMetricBase::setSpace(ssc::CoordinateSystem space)
{
	if (space == mSpace)
		return;

	// keep the absolute position (in ref) constant when changing space.
	ssc::Transform3D new_M_old = ssc::SpaceHelpers::get_toMfrom(this->getSpace(), space);
	mFrame = new_M_old*mFrame;

	mSpace = space;
	mSpaceListener->setSpace(space);
}

ssc::CoordinateSystem FrameMetricBase::getSpace() const
{
	return mSpace;
}

ssc::DoubleBoundingBox3D FrameMetricBase::boundingBox() const
{
	// convert both inputs to r space
	ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(this->getSpace(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p0_r = rM0.coord(this->getCoordinate());

	return ssc::DoubleBoundingBox3D(p0_r, p0_r);
}

QString FrameMetricBase::matrixAsSingleLineString() const
{
	std::stringstream stream;
	mFrame.put(stream, 0, ' ');
	return qstring_cast(stream.str());
}

} //namespace cx
