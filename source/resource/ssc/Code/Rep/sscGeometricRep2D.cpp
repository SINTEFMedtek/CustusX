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

#include "sscGeometricRep2D.h"

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

#include "sscMesh.h"
#include "sscView.h"
#include "sscMessageManager.h"
#include "sscSliceProxy.h"
#include "sscTypeConversions.h"

namespace ssc
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

	mMapper->SetInput(mMesh->getVtkPolyData()); // original - show-all method
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
} // namespace ssc
//---------------------------------------------------------

