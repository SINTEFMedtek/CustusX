// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.


#include <sscPointMetricRep.h>
#include "sscView.h"
#include "boost/bind.hpp"

namespace ssc
{

PointMetricRepPtr PointMetricRep::New(const QString& uid, const QString& name)
{
	PointMetricRepPtr retval(new PointMetricRep(uid, name));
	return retval;
}

PointMetricRep::PointMetricRep(const QString& uid, const QString& name) :
				DataMetricRep(uid, name), mView(NULL)
//		mSphereRadius(1),
//		mShowLabel(false),
//		mColor(ssc::Vector3D(1,0,0))
{
	mViewportListener.reset(new ssc::ViewportListener);
	mViewportListener->setCallback(boost::bind(&PointMetricRep::rescale, this));
}

//void PointMetricRep::setShowLabel(bool on)
//{
//  mShowLabel = on;
//  this->changedSlot();
//}

void PointMetricRep::setPointMetric(PointMetricPtr point)
{
	if (mMetric)
		disconnect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	mMetric = point;

	if (mMetric)
		connect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	mGraphicalPoint.reset();
	this->changedSlot();
}

PointMetricPtr PointMetricRep::getPointMetric()
{
	return mMetric;
}

void PointMetricRep::addRepActorsToViewRenderer(ssc::View *view)
{
	mView = view;
	mGraphicalPoint.reset();
	mText.reset();
	mViewportListener->startListen(mView->getRenderer());
	this->changedSlot();
}

void PointMetricRep::removeRepActorsFromViewRenderer(ssc::View *view)
{
	mView = NULL;
	mGraphicalPoint.reset();
	mText.reset();
	mViewportListener->stopListen();
}

//void PointMetricRep::setSphereRadius(double radius)
//{
//  mSphereRadius = radius;
//  this->changedSlot();
//}

void PointMetricRep::changedSlot()
{
	if (!mMetric)
		return;

	if (!mGraphicalPoint && mView && mMetric)
		mGraphicalPoint.reset(new ssc::GraphicalPoint3D(mView->getRenderer()));

	if (!mGraphicalPoint)
		return;

	ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mMetric->getSpace(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p0_r = rM0.coord(mMetric->getCoordinate());

	mGraphicalPoint->setValue(p0_r);
	mGraphicalPoint->setRadius(mGraphicsSize);
	mGraphicalPoint->setColor(mColor);

	if (!mShowLabel)
		mText.reset();
	if (!mText && mShowLabel)
		mText.reset(new ssc::CaptionText3D(mView->getRenderer()));
	if (mText)
	{
		mText->setColor(mColor);
		mText->setText(mMetric->getName());
		mText->setPosition(p0_r);
		mText->setSize(mLabelSize / 100);
	}

	this->rescale();
}

/**Note: Internal method!
 *
 * Scale the text to be a constant fraction of the viewport height
 * Called from a vtk camera observer
 *
 */
void PointMetricRep::rescale()
{
	if (!mGraphicalPoint)
		return;

	double size = mViewportListener->getVpnZoom();
	double sphereSize = mGraphicsSize / 100 / size;
	mGraphicalPoint->setRadius(sphereSize);
}

}
