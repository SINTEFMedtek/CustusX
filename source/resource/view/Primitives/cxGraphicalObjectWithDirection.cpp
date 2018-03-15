/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxGraphicalObjectWithDirection.h"
#include "vtkMatrix4x4.h"
#include "cxVtkHelperClasses.h"
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkSuperquadricSource.h>
#include <vtkRenderer.h>


namespace cx
{

GraphicalObjectWithDirection::GraphicalObjectWithDirection(vtkRendererPtr renderer)
{
    mPoint = Vector3D(0,0,0);
    mDirection = Vector3D(0,1,0);
    mVectorUp = Vector3D(1,0,0);
	mScale = Vector3D(1,1,1);
	mSource = vtkSuperquadricSourcePtr::New();

    mMapper = vtkPolyDataMapperPtr::New();
    mMapper->SetInputConnection(mSource->GetOutputPort());

    mActor = vtkActorPtr::New();
    mActor->SetMapper(mMapper);

    this->setRenderer(renderer);
}

GraphicalObjectWithDirection::~GraphicalObjectWithDirection()
{
    this->setRenderer(NULL);
}

vtkActorPtr GraphicalObjectWithDirection::getActor() const
{
    return mActor;
}

vtkPolyDataPtr GraphicalObjectWithDirection::getPolyData() const
{
    return mSource->GetOutput();
}

vtkPolyDataMapperPtr GraphicalObjectWithDirection::getMapper() const
{
    return mMapper;
}

void GraphicalObjectWithDirection::setPosition(Vector3D point)
{
    mPoint = point;
    this->updateOrientation();
}

void GraphicalObjectWithDirection::setDirection(Vector3D direction)
{
    mDirection = direction;
    this->updateOrientation();
}

void GraphicalObjectWithDirection::setVectorUp(const Vector3D &up)
{
    mVectorUp = up;
    this->updateOrientation();
}

void GraphicalObjectWithDirection::setScale(Vector3D scale)
{
	mScale = scale;
	this->updateOrientation();
}

void GraphicalObjectWithDirection::setRenderer(vtkRendererPtr renderer)
{
    if (mRenderer)
            mRenderer->RemoveActor(mActor);
    mRenderer = renderer;
    if (mRenderer)
            mRenderer->AddActor(mActor);
}

void GraphicalObjectWithDirection::updateOrientation()
{
    Transform3D R;
    bool directionAlongUp = similar(dot(mVectorUp, mDirection.normal()), 1.0);

    if (directionAlongUp)
    {
            R = Transform3D::Identity();
    }

    else
    {
            Vector3D jvec = mDirection.normal();
            Vector3D kvec = cross(mVectorUp, mDirection).normal();
            Vector3D ivec = cross(jvec, kvec).normal();
            Vector3D center = Vector3D::Zero();
            R = createTransformIJC(ivec, jvec, center);
    }

	Transform3D S = createTransformScale(mScale);
	Transform3D T = createTransformTranslate(mPoint);
	Transform3D M = T*R*S;
    mActor->SetUserMatrix(M.getVtkMatrix());
}


} // namespace cx
