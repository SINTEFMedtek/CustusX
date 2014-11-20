/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/



#include "cxGuideRep2D.h"
#include "boost/bind.hpp"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSectorSource.h>
#include "cxTool.h"

#include "cxSliceProxy.h"
#include "cxView.h"
#include "cxPatientModelService.h"

namespace cx
{

GuideRep2DPtr GuideRep2D::New(PatientModelServicePtr dataManager, const QString& uid)
{
	return wrap_new(new GuideRep2D(dataManager), uid);
}

GuideRep2D::GuideRep2D(PatientModelServicePtr dataManager) :
	mDataManager(dataManager),
	mOutlineWidth(1),
	mRequestedAccuracy(1)
{
}


void GuideRep2D::clear()
{
	if (this->getView())
		this->getView()->getRenderer()->RemoveActor(mCircleActor);
}

void GuideRep2D::onModifiedStartRender()
{
	if (!mMetric)
		return;

	if (!mCircleActor && this->getView() && mMetric && mSliceProxy)
	{
		mCircleSource = vtkSectorSource::New();
		mCircleSource->SetOuterRadius(mGraphicsSize);
		mCircleSource->SetInnerRadius(0);
		mCircleSource->SetStartAngle(0);
		mCircleSource->SetEndAngle(360);
		mCircleSource->SetCircumferentialResolution(60);
		vtkPolyDataMapperPtr mapper = vtkPolyDataMapper::New();
		mapper->SetInputConnection(mCircleSource->GetOutputPort());
		mapper->ScalarVisibilityOff();
		mCircleActor = vtkActor::New();
		mCircleActor->SetMapper(mapper);
		mCircleActor->GetProperty()->LightingOff();
		this->getRenderer()->AddActor(mCircleActor);

	}

	if (!mCircleActor)
		return;
	if (!mSliceProxy->getTool())
	{
		return;
	}

	double toolOffset = mSliceProxy->getTool()->getTooltipOffset();
	Transform3D rMt = mDataManager->get_rMpr()*mSliceProxy->getTool()->get_prMt();
	Vector3D toolOffsetPosRef = rMt.coord(Vector3D(0,0,toolOffset));
	Vector3D toolPosRef = rMt.coord(Vector3D(0,0,0));

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
