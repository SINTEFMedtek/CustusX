// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxGraphicalDisk.h"

#include <QColor>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSectorSource.h>
#include "sscVtkHelperClasses.h"

namespace cx
{

GraphicalDisk::GraphicalDisk() :
	mOutlineWidth(0.1)
{
	mRadius = 1;
	mColor = QColor(Qt::blue);
	mFillVisible = true;
	mOutlineColor = QColor(Qt::magenta);
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

	mCircleActor->SetPosition(mPosition[0], mPosition[1], mPosition[2]);
	mOutlineActor->SetPosition(mPosition[0], mPosition[1], mPosition[2]);
//	mOutlineSource->SetZCoord(0.01);
//	mCircleSource->SetZCoord(0.01);

	double innerRadius = std::max(0.0, mRadius*(1.0 - mOutlineWidth));
	mCircleSource->SetOuterRadius(innerRadius);
	mOutlineSource->SetInnerRadius(innerRadius);
	mOutlineSource->SetOuterRadius(mRadius);

	mCircleActor->SetVisibility(mFillVisible);
}

void GraphicalDisk::createActors()
{
	if (mCircleActor)
		return;

	mCircleSource = vtkSectorSourcePtr::New();
	mCircleSource->SetOuterRadius(mRadius);
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
//	mRenderer->AddActor(mCircleActor);

	mOutlineSource = vtkSectorSourcePtr::New();
	mOutlineSource->SetOuterRadius(mRadius);
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
//	mRenderer->AddActor(mOutlineActor);
}

} // namespace cx


