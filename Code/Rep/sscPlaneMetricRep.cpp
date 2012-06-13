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

namespace ssc
{

PlaneMetricRepPtr PlaneMetricRep::New(const QString& uid, const QString& name)
{
	PlaneMetricRepPtr retval(new PlaneMetricRep(uid, name));
	return retval;
}

PlaneMetricRep::PlaneMetricRep(const QString& uid, const QString& name) :
				DataMetricRep(uid, name), mView(NULL)
{
	mViewportListener.reset(new ssc::ViewportListener);
	mViewportListener->setCallback(boost::bind(&PlaneMetricRep::rescale, this));
}

void PlaneMetricRep::setMetric(PlaneMetricPtr point)
{
	if (mMetric)
		disconnect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	mMetric = point;

	if (mMetric)
		connect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	mGraphicalPoint.reset();
	mNormal.reset();
	this->changedSlot();
}

void PlaneMetricRep::addRepActorsToViewRenderer(ssc::View *view)
{
	mView = view;
	mGraphicalPoint.reset();
	mNormal.reset();
	mViewportListener->startListen(mView->getRenderer());
	this->changedSlot();
}

void PlaneMetricRep::removeRepActorsFromViewRenderer(ssc::View *view)
{
	mView = NULL;
	mGraphicalPoint.reset();
	mNormal.reset();
	mViewportListener->stopListen();
}

void PlaneMetricRep::changedSlot()
{
	if (!mMetric)
		return;

	if (!mGraphicalPoint && mView && mMetric)
	{
		mGraphicalPoint.reset(new ssc::GraphicalPoint3D(mView->getRenderer()));
		mNormal.reset(new ssc::GraphicalArrow3D(mView->getRenderer()));
	}

	if (!mGraphicalPoint)
		return;

	mGraphicalPoint->setColor(mColor);
	mNormal->setColor(mColor);

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

	ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mMetric->getSpace(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p0_r = rM0.coord(mMetric->getCoordinate());
	ssc::Vector3D n_r = rM0.vector(mMetric->getNormal());

	double size = mViewportListener->getVpnZoom();
	double sphereSize = mGraphicsSize / 100 / size;
//  double mSize = mSphereRadius/100/size*10;
//	double mSize = 0.07; // ratio of vp height
//  double scale = mSize/size;
//  std::cout << "s= " << size << "  ,scale= " << scale << std::endl;

	mGraphicalPoint->setValue(p0_r);
	mNormal->setValue(p0_r, n_r, sphereSize * 10);

//  double sphereSize = 0.007/size;
	mGraphicalPoint->setRadius(sphereSize);

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
}

}
