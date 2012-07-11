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

#include "sscSurfaceRep.h"

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkContourFilter.h>
#include <vtkImageData.h>
#include "sscView.h"
#include "sscMesh.h"
#include "sscImage.h"

namespace ssc
{

SurfaceRep::SurfaceRep(const QString& uid) :
	RepImpl(uid)
{

	mMapper = vtkPolyDataMapperPtr::New();
	mProperty = vtkPropertyPtr::New();
	mActor = vtkActorPtr::New();
	mActor->SetMapper( mMapper );
	mActor->SetProperty( mProperty );

	mContourFilter = vtkContourFilterPtr::New();
	mContourFilter->SetValue( 0 ,50 );
	mContourFilter->ComputeNormalsOn();
	mContourFilter->ComputeGradientsOn();
	mContourFilter->ComputeScalarsOn();
	mContourFilter->UseScalarTreeOn();

	/* Filter that computes point normals for a polygonal mesh */
	//vtkPolyDataNormals* normals = vtkPolyDataNormals::New();
	//normals->SetInputConnection( iso->GetOutputPort() );
	//normals->SetFeatureAngle( 60.0 );

	mMapper->SetInputConnection( mContourFilter->GetOutputPort() );
	mMapper->ScalarVisibilityOff();
	mActor->SetMapper( mMapper );


}

SurfaceRep::~SurfaceRep()
{
}

SurfaceRepPtr SurfaceRep::New(const QString& uid)
{
	SurfaceRepPtr retval(new SurfaceRep(uid));
	retval->mSelf = retval;
	return retval;
}

void SurfaceRep::addRepActorsToViewRenderer(View *view)
{
	view->getRenderer()->AddActor(mActor);
}

void SurfaceRep::removeRepActorsFromViewRenderer(View *view)
{
	view->getRenderer()->RemoveActor(mActor);
}

void SurfaceRep::setMesh(MeshPtr mesh)
{
	mMesh = mesh;
	mMesh->connectToRep(mSelf);

	mMapper->SetInput( mesh->getVtkPolyData() );
}

bool SurfaceRep::hasMesh(MeshPtr mesh) const
{
	return (mMesh != NULL);
}

void SurfaceRep::setImage(ImagePtr image)
{
	mImage = image;
	std::cout<<"is this illigal"<<std::endl;
	//mContourFilter->SetInput( (vtkDataSet*) &mImage->getRefVtkImageData() );
	vtkImageData *imag = mImage->getRefVtkImageData();
	mContourFilter->SetInput( imag );

	std::cout<<"mybe not"<<std::endl;
}

//---------------------------------------------------------
} // namespace ssc
//---------------------------------------------------------

