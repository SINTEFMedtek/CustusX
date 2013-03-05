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
	DataMetricRep(uid, name), mView(NULL),
	mOutlineWidth(0.1)
{
	mFillVisible = true;
	mOutlineColor = Vector3D(1,1,1) - mColor;
}

void PointMetricRep2D::setDynamicSize(bool on)
{
	if (on)
	{
		mViewportListener.reset(new ssc::ViewportListener);
		mViewportListener->setCallback(boost::bind(&PointMetricRep2D::rescale, this));
	}
	else
	{
		mViewportListener.reset();
	}
}

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
	if (mViewportListener)
		mViewportListener->startListen(mView->getRenderer());
	this->changedSlot();
}

void PointMetricRep2D::removeRepActorsFromViewRenderer(ssc::View* view)
{
	mView->getRenderer()->RemoveActor(mCircleActor);
	mView->getRenderer()->RemoveActor(mOutlineActor);
	mView = NULL;
	mText.reset();
	if (mViewportListener)
		mViewportListener->stopListen();
}

void PointMetricRep2D::changedSlot()
{
	if (!mMetric)
		return;

	if (!mCircleActor && mView && mMetric && mSliceProxy)
	{
		mCircleSource = vtkSectorSourcePtr::New();
		mCircleSource->SetOuterRadius(mGraphicsSize);
		mCircleSource->SetInnerRadius(0);
		mCircleSource->SetStartAngle(0);
		mCircleSource->SetEndAngle(360);
		mCircleSource->SetCircumferentialResolution(20);
		vtkPolyDataMapperPtr mapper = vtkPolyDataMapperPtr::New();
		mapper->SetInput(mCircleSource->GetOutput());
		mapper->ScalarVisibilityOff();
		mCircleActor = vtkActorPtr::New();
		mCircleActor->SetMapper(mapper);
		mCircleActor->GetProperty()->LightingOff();
		mView->getRenderer()->AddActor(mCircleActor);

		mOutlineSource = vtkSectorSourcePtr::New();
		mOutlineSource->SetOuterRadius(mGraphicsSize);
		mOutlineSource->SetInnerRadius(0);
		mOutlineSource->SetStartAngle(0);
		mOutlineSource->SetEndAngle(360);
		mOutlineSource->SetCircumferentialResolution(20);
		vtkPolyDataMapperPtr outlineMapper = vtkPolyDataMapperPtr::New();
		outlineMapper->SetInput(mOutlineSource->GetOutput());
		outlineMapper->ScalarVisibilityOff();
		mOutlineActor = vtkActorPtr::New();
		mOutlineActor->SetMapper(outlineMapper);
		mOutlineActor->GetProperty()->LightingOff();
		mView->getRenderer()->AddActor(mOutlineActor);
	}

	if (!mCircleActor)
		return;

	mCircleActor->GetProperty()->SetColor(mColor[0], mColor[1], mColor[2]);
	mOutlineActor->GetProperty()->SetColor(mOutlineColor[0], mOutlineColor[1], mOutlineColor[2]);

	Vector3D position = mSliceProxy->get_sMr() * mMetric->getRefCoord();
	mCircleActor->SetPosition(position[0], position[1], 0.0);
	mOutlineActor->SetPosition(position[0], position[1], 0.0);
	mOutlineSource->SetZCoord(0.01);
	mCircleSource->SetZCoord(0.01);

	if (abs(position[2]) > mGraphicsSize)
	{
		mCircleSource->SetOuterRadius(0);
		mOutlineSource->SetOuterRadius(0);
		mOutlineSource->SetInnerRadius(0);
	}
	else
	{
		double sphereSize = mGraphicsSize;
		if (mViewportListener)
		{
			double size = mViewportListener->getVpnZoom();
			sphereSize = mGraphicsSize / 100 / size * 2.5;
		}

		double radius = sphereSize*cos(asin(position[2]/sphereSize));

		mCircleSource->SetOuterRadius(radius*(1.0 - mOutlineWidth));
		mOutlineSource->SetInnerRadius(radius*(1.0 - mOutlineWidth));
		mOutlineSource->SetOuterRadius(radius);
	}

	if (!mShowLabel)
		mText.reset();
	if (!mText && mShowLabel && mView)
		mText.reset(new ssc::CaptionText3D(mView->getRenderer()));
	if (mText)
	{
		mText->setColor(mColor);
		mText->setText(mMetric->getName());
//		mText->setPosition(p0_r);
		mText->setSize(mLabelSize / 100);
	}

	mCircleActor->SetVisibility(mFillVisible);
}

void PointMetricRep2D::setFillVisibility(bool on)
{
	mFillVisible = on;
}

/**Note: Internal method!
 *
 * Scale the text to be a constant fraction of the viewport height
 * Called from a vtk camera observer
 *
 */
void PointMetricRep2D::rescale()
{
	this->changedSlot();
//	if (!mCircleActor)
//		return;
//
//	double size = mViewportListener->getVpnZoom();
////  double sphereSize = 0.007/size;
//	double sphereSize = mGraphicsSize / 100 / size;
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

void PointMetricRep2D::setOutlineWidth(double width)
{
	mOutlineWidth = width;
	changedSlot();
}

void PointMetricRep2D::setOutlineColor(double red, double green, double blue)
{
	mOutlineColor = Vector3D(red, green, blue);
	changedSlot();
}

}
