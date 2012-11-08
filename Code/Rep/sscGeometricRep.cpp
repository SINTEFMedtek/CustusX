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

#include "sscGeometricRep.h"

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>

#include "sscMesh.h"
#include "sscView.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"

namespace ssc
{

GeometricRep::GeometricRep(const QString& uid, const QString& name) :
	RepImpl(uid, name)
{
	mMapper = vtkPolyDataMapperPtr::New();
	mProperty = vtkPropertyPtr::New();
	mActor = vtkActorPtr::New();
	mActor->SetMapper(mMapper);
	mActor->SetProperty(mProperty);

	mActor->GetProperty()->BackfaceCullingOn();

	mProperty->SetPointSize(2);
}
GeometricRep::~GeometricRep()
{
}
GeometricRepPtr GeometricRep::New(const QString& uid, const QString& name)
{
	GeometricRepPtr retval(new GeometricRep(uid, name));
	retval->mSelf = retval;
	return retval;
}

void GeometricRep::addRepActorsToViewRenderer(View *view)
{
	view->getRenderer()->AddActor(mActor);
}

void GeometricRep::removeRepActorsFromViewRenderer(View *view)
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
	mMesh->connectToRep(mSelf);

	mMapper->SetInput(mMesh->getVtkPolyData());
	mMapper->ScalarVisibilityOff();//Don't use the LUT from the VtkPolyData

	//Set mesh color
	mActor->GetProperty()->SetColor(mMesh->getColor().redF(), mMesh->getColor().greenF(), mMesh->getColor().blueF());
	//Set mesh opacity
	mActor->GetProperty()->SetOpacity(mMesh->getColor().alphaF());

	if (mMesh->getIsWireframe())
		mActor->GetProperty()->SetRepresentationToWireframe();
	else
		mActor->GetProperty()->SetRepresentationToSurface();
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
// namespace ssc
//---------------------------------------------------------
