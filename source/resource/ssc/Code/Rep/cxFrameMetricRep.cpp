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

#include "cxFrameMetricRep.h"

#include "sscView.h"
#include "boost/bind.hpp"

namespace cx
{

FrameMetricRepPtr FrameMetricRep::New(const QString& uid, const QString& name)
{
	FrameMetricRepPtr retval(new FrameMetricRep(uid, name));
	return retval;
}

FrameMetricRep::FrameMetricRep(const QString& uid, const QString& name) :
				DataMetricRep(uid, name), mView(NULL)
{
	mViewportListener.reset(new ssc::ViewportListener);
	mViewportListener->setCallback(boost::bind(&FrameMetricRep::rescale, this));
}

void FrameMetricRep::setFrameMetric(FrameMetricPtr point)
{
	if (mMetric)
		disconnect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	mMetric = point;

	if (mMetric)
		connect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	mGraphicalPoint.reset();
	this->changedSlot();
}

FrameMetricPtr FrameMetricRep::getFrameMetric()
{
	return mMetric;
}

void FrameMetricRep::addRepActorsToViewRenderer(ssc::View *view)
{
	mView = view;
	mGraphicalPoint.reset();
	mText.reset();
	mViewportListener->startListen(mView->getRenderer());
	this->changedSlot();
}

void FrameMetricRep::removeRepActorsFromViewRenderer(ssc::View *view)
{
	mView = NULL;
	mGraphicalPoint.reset();
	mText.reset();
	mViewportListener->stopListen();
}

void FrameMetricRep::changedSlot()
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
void FrameMetricRep::rescale()
{
	if (!mGraphicalPoint)
		return;

	double size = mViewportListener->getVpnZoom();
	double sphereSize = mGraphicsSize / 100 / size;
	mGraphicalPoint->setRadius(sphereSize);
}

} // namespace ssc
