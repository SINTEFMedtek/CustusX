/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFrameMetricBase.h"
#include "cxBoundingBox3D.h"
#include "cxTypeConversions.h"
#include "cxSpaceProvider.h"
#include "cxSpaceListener.h"

namespace cx {

FrameMetricBase::FrameMetricBase(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
		DataMetric(uid, name, dataManager, spaceProvider),
		mSpace(CoordinateSystem::reference()),
		mFrame(Transform3D::Identity())
{
	mSpaceListener = mSpaceProvider->createListener();
	mSpaceListener->setSpace(mSpace);
//	mSpaceListener.reset(new SpaceListener(mSpace));
	connect(mSpaceListener.get(), SIGNAL(changed()), this, SIGNAL(transformChanged()));
}

FrameMetricBase::~FrameMetricBase()
{
}

QString FrameMetricBase::getParentSpace()
{
	return mSpaceProvider->convertToSpecific(mSpace).mRefObject;
}

void FrameMetricBase::setFrame(const Transform3D& rMt)
{
	mFrame = rMt;
	emit transformChanged();
}

Transform3D FrameMetricBase::getFrame()
{
	return mFrame;
}

Vector3D FrameMetricBase::getCoordinate() const
{
	Vector3D point_t = Vector3D(0,0,0);
	return mFrame.coord(point_t);
}

/** return frame described in ref space F * sMr
  */
Transform3D FrameMetricBase::getRefFrame() const
{
	Transform3D rMq = mSpaceProvider->get_toMfrom(this->getSpace(), CoordinateSystem(csREF));
	return rMq * mFrame;
}

/** return frame described in ref space F * sMr
  */
Vector3D FrameMetricBase::getRefCoord() const
{
	Transform3D rMq = this->getRefFrame();
	Vector3D p_r = rMq.coord(Vector3D(0,0,0));
	return p_r;
}

void FrameMetricBase::setSpace(CoordinateSystem space)
{
	if (space == mSpace)
		return;

	// keep the absolute position (in ref) constant when changing space.
	Transform3D new_M_old = mSpaceProvider->get_toMfrom(this->getSpace(), space);
	mFrame = new_M_old*mFrame;

	mSpace = space;
	mSpaceListener->setSpace(space);
}

CoordinateSystem FrameMetricBase::getSpace() const
{
	return mSpace;
}

DoubleBoundingBox3D FrameMetricBase::boundingBox() const
{
	// convert both inputs to r space
	Transform3D rM0 = mSpaceProvider->get_toMfrom(this->getSpace(), CoordinateSystem(csREF));
	Vector3D p0_r = rM0.coord(this->getCoordinate());

	return DoubleBoundingBox3D(p0_r, p0_r);
}

QString FrameMetricBase::matrixAsSingleLineString() const
{
	std::stringstream stream;
	mFrame.put(stream, 0, ' ');
	return qstring_cast(stream.str());
}

} //namespace cx
