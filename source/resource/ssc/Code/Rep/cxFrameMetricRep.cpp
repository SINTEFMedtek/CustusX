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
                DataMetricRep(uid, name)
{
	mViewportListener.reset(new ssc::ViewportListener);
	mViewportListener->setCallback(boost::bind(&FrameMetricRep::rescale, this));
}

void FrameMetricRep::clear()
{
    DataMetricRep::clear();
    mGraphicalPoint.reset();
}

void FrameMetricRep::addRepActorsToViewRenderer(ssc::View *view)
{
    mViewportListener->startListen(view->getRenderer());
    DataMetricRep::addRepActorsToViewRenderer(view);
}

void FrameMetricRep::removeRepActorsFromViewRenderer(ssc::View *view)
{
    DataMetricRep::removeRepActorsFromViewRenderer(view);
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

    ssc::Vector3D p0_r = mMetric->getRefCoord();

	mGraphicalPoint->setValue(p0_r);
	mGraphicalPoint->setRadius(mGraphicsSize);
	mGraphicalPoint->setColor(mColor);

    this->drawText();

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
