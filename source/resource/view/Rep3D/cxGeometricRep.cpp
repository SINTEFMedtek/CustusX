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


#include "cxGeometricRep.h"
#include "cxLogger.h"


#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkGlyph3D.h>
#include <vtkArrowSource.h>
#include <vtkPointData.h>



#include "cxMesh.h"
#include "cxView.h"

#include "cxTypeConversions.h"

namespace cx
{

GeometricRep::GeometricRep() :
	RepImpl()
{
    mMapper = vtkSmartPointer<vtkGlyph3DMapper>::New();
   vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();
   //arrowSource->Update();
   mMapper->SetSourceConnection(arrowSource->GetOutputPort());

	mProperty = vtkPropertyPtr::New();
	mActor = vtkActorPtr::New();
	mActor->SetMapper(mMapper);
	mActor->SetProperty(mProperty);

	mProperty->SetPointSize(2);
}
GeometricRep::~GeometricRep()
{
}
GeometricRepPtr GeometricRep::New(const QString& uid)
{
	return wrap_new(new GeometricRep(), uid);
}

void GeometricRep::addRepActorsToViewRenderer(ViewPtr view)
{
	view->getRenderer()->AddActor(mActor);
}

void GeometricRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
	view->getRenderer()->RemoveActor(mActor);
}

void GeometricRep::setMesh(MeshPtr mesh)
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

MeshPtr GeometricRep::getMesh()
{
	return mMesh;
}
bool GeometricRep::hasMesh(MeshPtr mesh) const
{
	return (mMesh == mesh);
}

void GeometricRep::meshChangedSlot()
{
//	std::cout << "GeometricRep::meshChangedSlot()" << std::endl;
//	mMesh->connectToRep(mSelf);

    //mMapper->SetInputData(mMesh->getVtkPolyData());
    //mMapper->ScalarVisibilityOff();//Don't use the LUT from the VtkPolyData

//     mMesh->getVtkPolyData()->GetPointData()->SetActiveVectors("ImageScalars");
    // Visualize
    mMapper->SetInputData(mMesh->getVtkPolyData());
    mMapper->SetOrientationArray("Flow direction");
    mMapper->Update();

	//Set mesh color
	mActor->GetProperty()->SetColor(mMesh->getColor().redF(), mMesh->getColor().greenF(), mMesh->getColor().blueF());
	//Set mesh opacity
	mActor->GetProperty()->SetOpacity(mMesh->getColor().alphaF());

	if (mMesh->getIsWireframe())
		mActor->GetProperty()->SetRepresentationToWireframe();
	else
		mActor->GetProperty()->SetRepresentationToSurface();

	//Set backface and frontface culling
	mActor->GetProperty()->SetBackfaceCulling(mMesh->getBackfaceCulling());
	mActor->GetProperty()->SetFrontfaceCulling(mMesh->getFrontfaceCulling());
}

/**called when transform is changed
 * reset it in the prop.*/
void GeometricRep::transformChangedSlot()
{
	if (!mMesh)
	{
		return;
	}

	mActor->SetUserMatrix(mMesh->get_rMd().getVtkMatrix());
}

//---------------------------------------------------------
}
// namespace cx
//---------------------------------------------------------
