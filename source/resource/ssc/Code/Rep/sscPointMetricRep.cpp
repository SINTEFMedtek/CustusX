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
#include "sscLogger.h"

namespace ssc
{

PointMetricRepPtr PointMetricRep::New(const QString& uid, const QString& name)
{
	PointMetricRepPtr retval(new PointMetricRep(uid, name));
	return retval;
}

PointMetricRep::PointMetricRep(const QString& uid, const QString& name) :
                DataMetricRep(uid, name)
{
	mViewportListener.reset(new ssc::ViewportListener);
	mViewportListener->setCallback(boost::bind(&PointMetricRep::rescale, this));
}

void PointMetricRep::clear()
{
    DataMetricRep::clear();
    mGraphicalPoint.reset();
}

void PointMetricRep::addRepActorsToViewRenderer(ssc::View *view)
{
    mViewportListener->startListen(view->getRenderer());
    DataMetricRep::addRepActorsToViewRenderer(view);
}

void PointMetricRep::removeRepActorsFromViewRenderer(ssc::View *view)
{
    DataMetricRep::removeRepActorsFromViewRenderer(view);
    mViewportListener->stopListen();
}

void PointMetricRep::changedSlot()
{
    SSC_LOG("A");
    if (!mMetric)
		return;
    SSC_LOG("B");

	if (!mGraphicalPoint && mView && mMetric)
		mGraphicalPoint.reset(new ssc::GraphicalPoint3D(mView->getRenderer()));
    SSC_LOG("C");

	if (!mGraphicalPoint)
		return;

    ssc::Vector3D p0_r = mMetric->getRefCoord();
    SSC_LOG("D");

	mGraphicalPoint->setValue(p0_r);
	mGraphicalPoint->setRadius(mGraphicsSize);
	mGraphicalPoint->setColor(mColor);

    this->drawText();

    SSC_LOG("rescale pre");
    this->rescale();
    SSC_LOG("rescale post");
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
