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


#include "cxGeometricRep2D.h"

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>

#include <vtkPolyDataNormals.h>
#include <vtkPlane.h>
#include <vtkStripper.h>
#include <vtkCellArray.h>

#include "cxMesh.h"
#include "cxView.h"
#include "cxReporter.h"
#include "cxSliceProxy.h"
#include "cxTypeConversions.h"

namespace cx
{

GeometricRep2D::GeometricRep2D(const QString& uid, const QString& name) :
	RepImpl(uid, name)
{
	mMapper = vtkPolyDataMapperPtr::New();
	mProperty = vtkPropertyPtr::New();
	mActor = vtkActorPtr::New();
	mActor->SetMapper(mMapper);
	mActor->SetProperty(mProperty);
}

GeometricRep2D::~GeometricRep2D()
{
}

GeometricRep2DPtr GeometricRep2D::New(const QString& uid, const QString& name)
{
	GeometricRep2DPtr retval(new GeometricRep2D(uid, name));
	retval->mSelf = retval;
	return retval;
}

void GeometricRep2D::addRepActorsToViewRenderer(View *view)
{
	view->getRenderer()->AddActor(mActor);
}

void GeometricRep2D::removeRepActorsFromViewRenderer(View *view)
{
	view->getRenderer()->RemoveActor(mActor);
}

void GeometricRep2D::setMesh(MeshPtr mesh)
{
	if (mesh == mMesh)
		return;
	if (mMesh)
	{
		disconnect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));
		disconnect(mMesh.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
	}
	mMesh = mesh;
	if (mMesh)
	{
		connect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));
		connect(mMesh.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		this->meshChangedSlot();
		this->transformChangedSlot();
	}
}

MeshPtr GeometricRep2D::getMesh()
{
	return mMesh;
}
bool GeometricRep2D::hasMesh(MeshPtr mesh) const
{
	return (mMesh == mesh);
}

void GeometricRep2D::setSliceProxy(SliceProxyPtr slicer)
{
	if (mSlicer)
	{
		disconnect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(transformChangedSlot()));
	}
	mSlicer = slicer;
	if (mSlicer)
	{
		connect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(transformChangedSlot()));
		this->transformChangedSlot();
	}
}

void GeometricRep2D::meshChangedSlot()
{
	mMesh->connectToRep(mSelf);

	mMapper->SetInputData(mMesh->getVtkPolyData()); // original - show-all method
	mMapper->ScalarVisibilityOff();//Don't use the LUT from the VtkPolyData
	//mNormals->SetInputConnection(mMesh->getVtkPolyData()->Get);

	//Set mesh color
	mActor->GetProperty()->SetColor(mMesh->getColor().redF(), mMesh->getColor().greenF(), mMesh->getColor().blueF());
	//Set mesh opacity
	mActor->GetProperty()->SetOpacity(mMesh->getColor().alphaF());

	// Turn lightning off - we dont want 3D effects but a clear view of the slice
	mActor->GetProperty()->LightingOff();
}

/**called when transform is changed
 * reset it in the prop.*/
void GeometricRep2D::transformChangedSlot()
{
	if (!mSlicer || !mMesh)
		return;

	Transform3D rMs = mSlicer->get_sMr().inv();
	Transform3D dMr = mMesh->get_rMd().inv();
	Transform3D dMs = dMr * rMs;

	mActor->SetUserMatrix(dMs.inv().getVtkMatrix());
}

//---------------------------------------------------------
} // namespace cx
//---------------------------------------------------------

