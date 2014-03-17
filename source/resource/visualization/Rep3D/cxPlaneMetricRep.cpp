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

#include "cxPlaneMetricRep.h"

#include "cxView.h"
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include "boost/bind.hpp"

namespace cx
{

PlaneMetricRepPtr PlaneMetricRep::New(const QString& uid, const QString& name)
{
	PlaneMetricRepPtr retval(new PlaneMetricRep(uid, name));
	return retval;
}

PlaneMetricRep::PlaneMetricRep(const QString& uid, const QString& name) :
                DataMetricRep(uid, name)
{
	mViewportListener.reset(new ViewportListener);
	mViewportListener->setCallback(boost::bind(&PlaneMetricRep::rescale, this));
}

void PlaneMetricRep::clear()
{
    mGraphicalPoint.reset();
    mNormal.reset();
}

void PlaneMetricRep::addRepActorsToViewRenderer(View *view)
{
    mViewportListener->startListen(view->getRenderer());
    DataMetricRep::addRepActorsToViewRenderer(view);
}

void PlaneMetricRep::removeRepActorsFromViewRenderer(View *view)
{
	mViewportListener->stopListen();
    DataMetricRep::removeRepActorsFromViewRenderer(view);
}

PlaneMetricPtr PlaneMetricRep::getPlaneMetric()
{
	return boost::dynamic_pointer_cast<PlaneMetric>(mMetric);
}

void PlaneMetricRep::onModifiedStartRender()
{
	PlaneMetricPtr planeMetric = this->getPlaneMetric();
	if (!planeMetric)
		return;

	if (!mGraphicalPoint && mView && mMetric)
	{
		mGraphicalPoint.reset(new GraphicalPoint3D(mView->getRenderer()));
		mNormal.reset(new GraphicalArrow3D(mView->getRenderer()));
		mRect.reset(new Rect3D(mView->getRenderer(), mMetric->getColor()));
		mRect->setLine(true, 1);
	}

	if (!mGraphicalPoint)
		return;

	mGraphicalPoint->setColor(mMetric->getColor());
	mNormal->setColor(mMetric->getColor());
	mRect->setColor(mMetric->getColor());

	this->rescale();
}

/**Note: Internal method!
 *
 * Scale the text to be a constant fraction of the viewport height
 * Called from a vtk camera observer
 *
 */
void PlaneMetricRep::rescale()
{
	if (!mGraphicalPoint)
		return;

	PlaneMetricPtr planeMetric = this->getPlaneMetric();

	Vector3D p0_r = planeMetric->getRefCoord();
	Vector3D n_r = planeMetric->getRefNormal();

	double size = mViewportListener->getVpnZoom();
	double sphereSize = mGraphicsSize / 100 / size;

	mGraphicalPoint->setValue(p0_r);
	mGraphicalPoint->setRadius(sphereSize);

	mNormal->setValue(p0_r, n_r, sphereSize * 8);

	this->drawRectangleForPlane(p0_r, n_r, size);

    this->drawText();
}

void PlaneMetricRep::drawRectangleForPlane(Vector3D p0_r, Vector3D n_r, double size)
{
	if (!mRect)
		return;
	// draw a rectangle showing the plane:
	Vector3D e_z = n_r;
	Vector3D k1(1,0,0);
	Vector3D k2(0,1,0);
	Vector3D e_x;
	if (cross(k2,e_z).length() > cross(k1,e_z).length())
		e_x = cross(k2,e_z)/cross(k2,e_z).length();
	else
		e_x = cross(k1,e_z)/cross(k1,e_z).length();

	Vector3D e_y = cross(e_z,e_x);
	Transform3D rMb = createTransformIJC(e_x, e_y, p0_r);
	double bb_size = 0.10/size;
	DoubleBoundingBox3D bb(-bb_size,bb_size,-bb_size,bb_size,0,0);

	mRect->updatePosition(bb, rMb);
}

}
