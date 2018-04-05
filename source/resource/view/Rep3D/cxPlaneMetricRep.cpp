/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxPlaneMetricRep.h"

#include "cxView.h"
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include "boost/bind.hpp"
#include "cxBoundingBox3D.h"

namespace cx
{

PlaneMetricRepPtr PlaneMetricRep::New(const QString& uid)
{
	return wrap_new(new PlaneMetricRep(), uid);
}

PlaneMetricRep::PlaneMetricRep()
{
	mViewportListener.reset(new ViewportListener);
	mViewportListener->setCallback(boost::bind(&PlaneMetricRep::rescale, this));
}

void PlaneMetricRep::clear()
{
    mGraphicalPoint.reset();
    mNormal.reset();
}

void PlaneMetricRep::addRepActorsToViewRenderer(ViewPtr view)
{
    mViewportListener->startListen(view->getRenderer());
    DataMetricRep::addRepActorsToViewRenderer(view);
}

void PlaneMetricRep::removeRepActorsFromViewRenderer(ViewPtr view)
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

	if (!mGraphicalPoint && this->getView() && mMetric)
	{
		mGraphicalPoint.reset(new GraphicalPoint3D(this->getRenderer()));
		mNormal.reset(new GraphicalArrow3D(this->getRenderer()));
		mRect.reset(new Rect3D(this->getRenderer(), mMetric->getColor()));
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

	double size = mViewportListener->getVpnZoom(p0_r);
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
