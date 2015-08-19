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

#include "cxGraphicalDisk.h"

#include <QColor>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSectorSource.h>
#include "cxVtkHelperClasses.h"
#include "vtkMatrix4x4.h"
#include "vtkLinearExtrusionFilter.h"
#include "vtkPolyDataNormals.h"

namespace cx
{

GraphicalDisk::GraphicalDisk() :
	mOutlineWidth(0.1)
{
	mDirection = Vector3D::UnitZ();
//	std::cout << "GraphicalDisk::create " << mDirection << std::endl;

	mRadius = 1;
	mColor = QColor(Qt::blue);
	mFillVisible = true;
	mOutlineColor = QColor(Qt::magenta);
	mUseLighting = false;
	mHeight = 0;
}

GraphicalDisk::~GraphicalDisk()
{
	this->removeActors();
}

void GraphicalDisk::setRadius(double radius)
{
	mRadius = radius;
}
void GraphicalDisk::setOutlineWidth(double width)
{
	mOutlineWidth = width;
}
void GraphicalDisk::setFillVisible(bool val)
{
	mFillVisible = val;
}
void GraphicalDisk::setOutlineColor(QColor color)
{
	mOutlineColor = color;
}
void GraphicalDisk::setColor(QColor color)
{
	mColor = color;
}
void GraphicalDisk::setPosition(Vector3D pos)
{
	mPosition = pos;
}
void GraphicalDisk::setDirection(Vector3D direction)
{
//	std::cout << "GraphicalDisk::setDirection " << direction << std::endl;

	mDirection = direction;
}
void GraphicalDisk::setLighting(bool on)
{
	mUseLighting = on;
}
void GraphicalDisk::setHeight(double height)
{
	mHeight = height;
}

void GraphicalDisk::setRenderer(vtkRendererPtr renderer)
{
	this->removeActors();
	mRenderer = renderer;
	this->addActors();
}

void GraphicalDisk::addActors()
{
	if (mRenderer && mCircleActor && mOutlineActor)
	{
		mRenderer->AddActor(mCircleActor);
		mRenderer->AddActor(mOutlineActor);
	}
}

void GraphicalDisk::removeActors()
{
	if (mRenderer && mCircleActor && mOutlineActor)
	{
		mRenderer->RemoveActor(mCircleActor);
		mRenderer->RemoveActor(mOutlineActor);
	}
}

void GraphicalDisk::update()
{
	if (!mRenderer)
		return;

	if (!mCircleActor)
	{
		this->createActors();
		this->addActors();
	}

	if (!mCircleActor)
		return;

	mCircleActor->GetProperty()->SetColor(getColorAsVector3D(mColor).begin());
	mOutlineActor->GetProperty()->SetColor(getColorAsVector3D(mOutlineColor).begin());

	double innerRadius = std::max(0.0, mRadius*(1.0 - mOutlineWidth));
	mCircleSource->SetOuterRadius(innerRadius);
	mOutlineSource->SetInnerRadius(innerRadius);
	mOutlineSource->SetOuterRadius(mRadius);

	mCircleExtruder->SetScaleFactor(mHeight);
	mOutlineExtruder->SetScaleFactor(mHeight);

	mCircleActor->SetVisibility(mFillVisible);

	this->updateOrientation();
}

void GraphicalDisk::createActors()
{
	if (mCircleActor)
		return;

	int resolution = 40;

	mCircleSource = vtkSectorSourcePtr::New();
	mCircleSource->SetOuterRadius(mRadius);
	mCircleSource->SetInnerRadius(0);
	mCircleSource->SetStartAngle(0);
	mCircleSource->SetEndAngle(360);
	mCircleSource->SetCircumferentialResolution(resolution);

	mCircleExtruder = vtkLinearExtrusionFilterPtr::New();
	mCircleExtruder->SetInputConnection(mCircleSource->GetOutputPort());
	mCircleExtruder->SetScaleFactor(mHeight);
	mCircleExtruder->SetExtrusionTypeToVectorExtrusion();
	mCircleExtruder->SetVector(0,0,1);

	vtkPolyDataMapperPtr mapper = vtkPolyDataMapperPtr::New();
	mapper->SetInputConnection(mCircleExtruder->GetOutputPort());
	mapper->ScalarVisibilityOff();
	mCircleActor = vtkActorPtr::New();
	mCircleActor->SetMapper(mapper);
	mCircleActor->GetProperty()->SetLighting(mUseLighting);

	mOutlineSource = vtkSectorSourcePtr::New();
	mOutlineSource->SetOuterRadius(mRadius);
	mOutlineSource->SetInnerRadius(0);
	mOutlineSource->SetStartAngle(0);
	mOutlineSource->SetEndAngle(360);
	mOutlineSource->SetCircumferentialResolution(resolution);

	mOutlineExtruder = vtkLinearExtrusionFilterPtr::New();
	mOutlineExtruder->SetInputConnection(mOutlineSource->GetOutputPort());
	mOutlineExtruder->SetScaleFactor(mHeight);
	mOutlineExtruder->SetExtrusionTypeToVectorExtrusion();
	mOutlineExtruder->SetVector(0,0,1);

	vtkPolyDataNormalsPtr normals = vtkPolyDataNormalsPtr::New();
	normals->SetInputConnection(mOutlineExtruder->GetOutputPort());

	vtkPolyDataMapperPtr outlineMapper = vtkPolyDataMapperPtr::New();
	outlineMapper->SetInputConnection(normals->GetOutputPort());
	outlineMapper->ScalarVisibilityOff();
	mOutlineActor = vtkActorPtr::New();
	mOutlineActor->SetMapper(outlineMapper);
	mOutlineActor->GetProperty()->SetLighting(mUseLighting);
}


void GraphicalDisk::updateOrientation()
{
//	Transform3D M = createTransformRotationBetweenVectors(Vector3D::UnitX(), mDirection.normal());

	Vector3D from = Vector3D::UnitZ();
	Transform3D M;
	bool directionAlongYAxis = similar(dot(from, mDirection.normal()), 1.0);

	if (directionAlongYAxis)
	{
		M = Transform3D::Identity();
	}
	else
	{
		Vector3D newXAxis = cross(from, mDirection).normal();
		Vector3D newZAxis = mDirection;
		Vector3D ivec = newXAxis;
		Vector3D jvec = cross(newZAxis, newXAxis);
		Vector3D center = Vector3D::Zero();
		M = createTransformIJC(ivec, jvec, center);

	}

	Transform3D T = createTransformTranslate(mPosition);
	M = T*M;

	mCircleActor->SetUserMatrix(M.getVtkMatrix());
	mOutlineActor->SetUserMatrix(M.getVtkMatrix());

//		mCircleActor->SetPosition(mPosition[0], mPosition[1], mPosition[2]);
//		mOutlineActor->SetPosition(mPosition[0], mPosition[1], mPosition[2]);

}

void GraphicalDisk::setRadiusBySlicingSphere(double sphereRadius, double sliceHeight)
{
	double r = this->getRadiusOfCircleSlicedFromSphere(sphereRadius, sliceHeight);
	this->setRadius(r);
}

double GraphicalDisk::getRadiusOfCircleSlicedFromSphere(double sphereRadius, double sliceHeight) const
{
	double retval = 0;

    if (std::abs(sliceHeight) > sphereRadius)
	{
		retval = 0;
	}
	else
	{
		retval = sphereRadius*cos(asin(sliceHeight/sphereRadius));
	}

	return retval;
}


} // namespace cx


