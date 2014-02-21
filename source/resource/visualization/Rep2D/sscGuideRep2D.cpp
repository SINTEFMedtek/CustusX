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


#include <sscGuideRep2D.h>
#include "boost/bind.hpp"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSectorSource.h>

#include "sscSliceProxy.h"
#include "sscToolManager.h"
#include "sscView.h"
namespace cx
{

GuideRep2DPtr GuideRep2D::New(const QString& uid, const QString& name)
{
	GuideRep2DPtr retval(new GuideRep2D(uid, name));
	return retval;
}

GuideRep2D::GuideRep2D(const QString& uid, const QString& name) :
    DataMetricRep(uid, name),
	mOutlineWidth(1),
	mRequestedAccuracy(1)
{
}


void GuideRep2D::clear()
{
    if (mView)
        mView->getRenderer()->RemoveActor(mCircleActor);
}

void GuideRep2D::onModifiedStartRender()
{
	if (!mMetric)
		return;

	if (!mCircleActor && mView && mMetric && mSliceProxy)
	{
		mCircleSource = vtkSectorSource::New();
		mCircleSource->SetOuterRadius(mGraphicsSize);
		mCircleSource->SetInnerRadius(0);
		mCircleSource->SetStartAngle(0);
		mCircleSource->SetEndAngle(360);
		mCircleSource->SetCircumferentialResolution(60);
		vtkPolyDataMapperPtr mapper = vtkPolyDataMapper::New();
		mapper->SetInput(mCircleSource->GetOutput());
		mapper->ScalarVisibilityOff();
		mCircleActor = vtkActor::New();
		mCircleActor->SetMapper(mapper);
		mCircleActor->GetProperty()->LightingOff();
		mView->getRenderer()->AddActor(mCircleActor);

	}

	if (!mCircleActor)
		return;
	if (!mSliceProxy->getTool())
	{
		return;
	}

	double toolOffset = mSliceProxy->getTool()->getTooltipOffset();
	Vector3D toolOffsetPosRef = (dataManager()->get_rMpr()*mSliceProxy->getTool()->get_prMt()).coord(Vector3D(0,0,toolOffset));
	Vector3D toolPosRef = (dataManager()->get_rMpr()*mSliceProxy->getTool()->get_prMt()).coord(Vector3D(0,0,0));

	Vector3D centerRef = mMetric->getRefCoord() + 0.5*(toolOffsetPosRef - mMetric->getRefCoord());
	Vector3D position = mSliceProxy->get_sMr() * centerRef;
	mCircleActor->SetPosition(position[0], position[1], 0);

	const double margin = 10;
	double offsetDistance = (mMetric->getRefCoord() - toolOffsetPosRef).length();
	double distance = (mMetric->getRefCoord() - toolPosRef).length();
	double radius = 0.5 * offsetDistance + margin;
	mCircleSource->SetOuterRadius(radius);
	mCircleSource->SetInnerRadius(radius - mOutlineWidth);

	if (distance < mRequestedAccuracy)
	{
		mCircleActor->GetProperty()->SetColor(0, 1, 1);
	}
	else if (offsetDistance < mRequestedAccuracy && toolOffset >= 0 && distance < 4*mRequestedAccuracy)
	{
		mCircleActor->GetProperty()->SetColor(0, 1, 1.0 - (distance-mRequestedAccuracy)/(3*mRequestedAccuracy));
	}
	else if (offsetDistance < mRequestedAccuracy && toolOffset >= 0)
	{
		mCircleActor->GetProperty()->SetColor(0, 1, 0);
	}
	else
	{
		mCircleActor->GetProperty()->SetColor(1, 0, 0);
	}

}

void GuideRep2D::setSliceProxy(SliceProxyPtr sliceProxy)
{
	if (mSliceProxy)
		disconnect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(setModified()));
	mSliceProxy = sliceProxy;
	if (mSliceProxy)
		connect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(setModified()));
	this->setModified();
}

void GuideRep2D::setOutlineWidth(double width)
{
	mOutlineWidth = width;
	this->setModified();
}

void GuideRep2D::setRequestedAccuracy(double accuracy)
{
	mRequestedAccuracy = accuracy;
	this->setModified();
}

}
