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

#include <sscPlaneMetricRep.h>

#include "sscView.h"
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

//void PlaneMetricRep::setMetric(PlaneMetricPtr point)
//{
//	if (mMetric)
//		disconnect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

//	mMetric = point;

//	if (mMetric)
//		connect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

//	mGraphicalPoint.reset();
//	mNormal.reset();
//	this->changedSlot();
//}

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
//	mView = NULL;
//	mGraphicalPoint.reset();
//	mNormal.reset();
	mViewportListener->stopListen();
    DataMetricRep::removeRepActorsFromViewRenderer(view);
}

void PlaneMetricRep::changedSlot()
{
	if (!mMetric)
		return;

	if (!mGraphicalPoint && mView && mMetric)
	{
		mGraphicalPoint.reset(new GraphicalPoint3D(mView->getRenderer()));
		mNormal.reset(new GraphicalArrow3D(mView->getRenderer()));
		mRect.reset(new Rect3D(mView->getRenderer(), this->getColorAsVector3D()));
		mRect->setLine(true, 1);
	}

	if (!mGraphicalPoint)
		return;

	mGraphicalPoint->setColor(this->getColorAsVector3D());
	mNormal->setColor(this->getColorAsVector3D());

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

	Transform3D rM0 = SpaceHelpers::get_toMfrom(mMetric->getSpace(), CoordinateSystem(csREF));
	Vector3D p0_r = rM0.coord(mMetric->getCoordinate());
	Vector3D n_r = rM0.vector(mMetric->getNormal());

	double size = mViewportListener->getVpnZoom();
	double sphereSize = mGraphicsSize / 100 / size;
//  double mSize = mSphereRadius/100/size*10;
//	double mSize = 0.07; // ratio of vp height
//  double scale = mSize/size;
//  std::cout << "s= " << size << "  ,scale= " << scale << std::endl;

	mGraphicalPoint->setValue(p0_r);
	mNormal->setValue(p0_r, n_r, sphereSize * 8);

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

//  double sphereSize = 0.007/size;
	mGraphicalPoint->setRadius(sphereSize);

    this->drawText();
//	if (!mShowLabel)
//		mText.reset();
//	if (!mText && mShowLabel)
//		mText.reset(new CaptionText3D(mView->getRenderer()));
//	if (mText)
//	{
//		mText->setColor(mColor);
//		mText->setText(mMetric->getName());
//		mText->setPosition(p0_r);
//		mText->setSize(mLabelSize / 100);
//	}
}

}
