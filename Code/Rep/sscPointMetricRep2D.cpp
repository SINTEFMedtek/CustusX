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


#include <sscPointMetricRep2D.h>
#include "boost/bind.hpp"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSectorSource.h>

#include "sscSliceProxy.h"
#include "sscView.h"
namespace ssc
{

PointMetricRep2DPtr PointMetricRep2D::New(const QString& uid, const QString& name)
{
	PointMetricRep2DPtr retval(new PointMetricRep2D(uid, name));
	return retval;
}

PointMetricRep2D::PointMetricRep2D(const QString& uid, const QString& name) :
				DataMetricRep(uid, name), mView(NULL)
//		mSphereRadius(1),
//		mShowLabel(false),
//		mColor(ssc::Vector3D(1,0,0))
{
	mViewportListener.reset(new ssc::ViewportListener);
	mViewportListener->setCallback(boost::bind(&PointMetricRep2D::rescale, this));
	std::cout << "Creating PointMetricRep2D" << std::endl;
}

//void PointMetricRep2D::setShowLabel(bool on)
//{
//  mShowLabel = on;
//  this->changedSlot();
//}

void PointMetricRep2D::setPointMetric(PointMetricPtr point)
{
	if (mMetric)
		disconnect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	mMetric = point;

	if (mMetric)
		connect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	this->changedSlot();
}
	
PointMetricPtr PointMetricRep2D::getPointMetric()
{
	return mMetric;
}

void PointMetricRep2D::addRepActorsToViewRenderer(ssc::View* view)
{
	mView = view;
	mText.reset();
	mViewportListener->startListen(mView->getRenderer());
	this->changedSlot();
}

void PointMetricRep2D::removeRepActorsFromViewRenderer(ssc::View* view)
{
	mView->getRenderer()->RemoveActor(mActor);
	mView = NULL;
	mText.reset();
	mViewportListener->stopListen();
	
}

//void PointMetricRep2D::setSphereRadius(double radius)
//{
//  mSphereRadius = radius;
//  this->changedSlot();
//}

void PointMetricRep2D::changedSlot()
{
	if (!mMetric)
		return;

	if (!mActor && mView && mMetric && mSliceProxy)
	{
		mCircle = vtkSectorSource::New();
		mCircle->SetOuterRadius(mGraphicsSize);
		mCircle->SetInnerRadius(0);
		mCircle->SetStartAngle(0);
		mCircle->SetEndAngle(360);
		mCircle->SetCircumferentialResolution(20);
		vtkPolyDataMapperPtr mapper = vtkPolyDataMapper::New();
		mapper->SetInput(mCircle->GetOutput());
		mapper->ScalarVisibilityOff();
		mActor = vtkActor::New();
		mActor->SetMapper(mapper);
		mActor->GetProperty()->SetColor(1.0, 0, 0);
		mActor->GetProperty()->LightingOff();
		mView->getRenderer()->AddActor(mActor);
	}

	if (!mActor)
		return;

	Vector3D position = mSliceProxy->get_sMr() * mMetric->getRefCoord();
	mActor->SetPosition(position[0], position[1], 0);

	if (abs(position[2]) > mGraphicsSize)
	{
		mCircle->SetOuterRadius(0);
	}
	else
	{
		double radius = mGraphicsSize*cos(asin(position[2]/mGraphicsSize));
		mCircle->SetOuterRadius(radius);
	}

	if (!mShowLabel)
		mText.reset();
	if (!mText && mShowLabel)
		mText.reset(new ssc::CaptionText3D(mView->getRenderer()));
	if (mText)
	{
		mText->setColor(mColor);
		mText->setText(mMetric->getName());
//		mText->setPosition(p0_r);
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
void PointMetricRep2D::rescale()
{
	if (!mActor)
		return;

	double size = mViewportListener->getVpnZoom();
//  double sphereSize = 0.007/size;
	double sphereSize = mGraphicsSize / 100 / size;
//	mGraphicalPoint->setRadius(sphereSize);
}

void PointMetricRep2D::setSliceProxy(ssc::SliceProxyPtr sliceProxy)
{
	if (mSliceProxy)
		disconnect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(changedSlot()));
	mSliceProxy = sliceProxy;
	if (mSliceProxy)
		connect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(changedSlot()));
	changedSlot();
}

void PointMetricRep2D::set_vpMs(Transform3D vpMs)
{
	m_vpMs = vpMs;
	changedSlot();
}
	
}
