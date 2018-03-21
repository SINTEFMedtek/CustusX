/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/



#include "cxPointMetricRep.h"
#include "cxView.h"
#include "boost/bind.hpp"
#include "cxLogger.h"

namespace cx
{

PointMetricRepPtr PointMetricRep::New(const QString& uid)
{
	return wrap_new(new PointMetricRep(), uid);
}

PointMetricRep::PointMetricRep()
{
	mViewportListener.reset(new ViewportListener);
	mViewportListener->setCallback(boost::bind(&PointMetricRep::rescale, this));
}

void PointMetricRep::clear()
{
    DataMetricRep::clear();
    mGraphicalPoint.reset();
}

void PointMetricRep::addRepActorsToViewRenderer(ViewPtr view)
{
    mViewportListener->startListen(view->getRenderer());
    DataMetricRep::addRepActorsToViewRenderer(view);
}

void PointMetricRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
    DataMetricRep::removeRepActorsFromViewRenderer(view);
    mViewportListener->stopListen();
}

void PointMetricRep::onModifiedStartRender()
{
    if (!mMetric)
		return;

	if (!mGraphicalPoint && this->getView() && mMetric)
		mGraphicalPoint.reset(new GraphicalPoint3D(this->getRenderer()));

	if (!mGraphicalPoint)
		return;

    Vector3D p0_r = mMetric->getRefCoord();

	mGraphicalPoint->setValue(p0_r);
	mGraphicalPoint->setRadius(mGraphicsSize);
	mGraphicalPoint->setColor(mMetric->getColor());

    this->drawText();

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

	double size = mViewportListener->getVpnZoom(mMetric->getRefCoord());
	double sphereSize = mGraphicsSize / 100 / size;
	mGraphicalPoint->setRadius(sphereSize);
}

}
