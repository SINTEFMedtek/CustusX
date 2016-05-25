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
    source = vtkSuperquadricSourcePtr::New();

    mapper = vtkPolyDataMapperPtr::New();
    mapper->SetInputConnection(source->GetOutputPort());

    actor = vtkActorPtr::New();
    actor->SetMapper(mapper);

    this->setRenderer(renderer);
}

GraphicalObjectWithDirection::~GraphicalObjectWithDirection()
{
    this->setRenderer(NULL);
}

vtkActorPtr GraphicalObjectWithDirection::getActor()
{
    return actor;
}

vtkPolyDataPtr GraphicalObjectWithDirection::getPolyData()
{
    return source->GetOutput();
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

void GraphicalObjectWithDirection::setRenderer(vtkRendererPtr renderer)
{
    if (mRenderer)
            mRenderer->RemoveActor(actor);
    mRenderer = renderer;
    if (mRenderer)
            mRenderer->AddActor(actor);
}

void GraphicalObjectWithDirection::updateOrientation()
{
//	Transform3D M = createTransformRotationBetweenVectors(Vector3D::UnitY(), mDirection.normal());
    Transform3D M;
    bool directionAlongYAxis = similar(dot(Vector3D::UnitY(), mDirection.normal()), 1.0);

    if (directionAlongYAxis)
    {
            M = Transform3D::Identity();
    }
    else
    {
            Vector3D ivec = cross(Vector3D::UnitY(), mDirection).normal();
            Vector3D jvec = mDirection.normal();
            Vector3D center = Vector3D::Zero();
            M = createTransformIJC(ivec, jvec, center);
    }

    Transform3D T = createTransformTranslate(mPoint);
    M = T*M;

//	std::cout << "M end:\n" << M << std::endl;
////	actor->SetPosition(point.begin());
    actor->SetUserMatrix(M.getVtkMatrix());
}

} // namespace cx
