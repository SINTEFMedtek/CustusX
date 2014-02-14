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

#include "cxGraphicalTorus3D.h"

#include <QColor>
#include "sscVtkHelperClasses.h"
#include "vtkSuperquadricSource.h"

#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "sscTypeConversions.h"
#include "sscGraphicalPrimitives.h"
#include "vtkMatrix4x4.h"
#include <vtkActor.h>
#include <vtkProperty.h>

namespace cx
{

GraphicalTorus3D::GraphicalTorus3D(vtkRendererPtr renderer)
{
	source = vtkSuperquadricSourcePtr::New();
	source->SetToroidal(true);
	source->SetSize(10);
	source->SetThickness(0.5);
	source->SetThetaResolution(source->GetThetaResolution()*2);

	mapper = vtkPolyDataMapperPtr::New();
	mapper->SetInputConnection(source->GetOutputPort());

	actor = vtkActorPtr::New();
	actor->SetMapper(mapper);

	this->setRenderer(renderer);
}

GraphicalTorus3D::~GraphicalTorus3D()
{
	this->setRenderer(NULL);
}

void GraphicalTorus3D::setRenderer(vtkRendererPtr renderer)
{
	if (mRenderer)
		mRenderer->RemoveActor(actor);
	mRenderer = renderer;
	if (mRenderer)
		mRenderer->AddActor(actor);
}

void GraphicalTorus3D::setRadius(double value)
{
	source->SetSize(value);
}

void GraphicalTorus3D::setThickness(double value)
{
	source->SetThickness(value);
}

void GraphicalTorus3D::setColor(QColor color)
{
	setColorAndOpacity(actor->GetProperty(), color);
}

void GraphicalTorus3D::setPosition(Vector3D point)
{
	actor->SetPosition(point.begin());
}

void GraphicalTorus3D::setDirection(Vector3D direction)
{
	Transform3D M;
	bool directionAlongYAxis = similar(dot(Vector3D::UnitY(), direction.normal()), 1.0);

	if (directionAlongYAxis)
	{
		M = Transform3D::Identity();
	}
	else
	{
		Vector3D ivec = cross(Vector3D::UnitY(), direction).normal();
		Vector3D jvec = direction.normal();
		Vector3D center = Vector3D::Zero();
		M = createTransformIJC(ivec, jvec, center);
	}

	actor->SetUserMatrix(M.getVtkMatrix());
}

vtkActorPtr GraphicalTorus3D::getActor()
{
	return actor;
}

vtkPolyDataPtr GraphicalTorus3D::getPolyData()
{
	return source->GetOutput();
}


} // namespace cx

