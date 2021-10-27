/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

#include "cxSliceProxy.h"
#include "cxTypeConversions.h"

namespace cx
{

GeometricRep2D::GeometricRep2D() :
	RepImpl()
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

GeometricRep2DPtr GeometricRep2D::New(const QString& uid)
{
	return wrap_new(new GeometricRep2D(), uid);
}

void GeometricRep2D::addRepActorsToViewRenderer(ViewPtr view)
{
	view->getRenderer()->AddActor(mActor);
}

void GeometricRep2D::removeRepActorsFromViewRenderer(ViewPtr view)
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
	mMapper->SetInputData(mMesh->getVtkPolyData()); // original - show-all method
	mMapper->ScalarVisibilityOff();//Don't use the LUT from the VtkPolyData
	//mNormals->SetInputConnection(mMesh->getVtkPolyData()->Get);

	//Set mesh color
	mActor->GetProperty()->SetColor(mMesh->getColor().redF(), mMesh->getColor().greenF(), mMesh->getColor().blueF());
	//Set mesh opacity
	mActor->GetProperty()->SetOpacity(mMesh->getColor().alphaF());

	// Turn lightning off - we dont want 3D effects but a clear view of the slice
	mActor->GetProperty()->LightingOff();
	
	//Set linewidth of mesh
	mActor->GetProperty()->SetLineWidth(mMesh->getProperties().mLineWidth->getValue());
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

