#include "sscSurfaceRep.h"

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkContourFilter.h>
#include <vtkImageData.h>
#include "sscView.h"


namespace ssc
{

SurfaceRep::SurfaceRep(const std::string& uid) :
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
	
SurfaceRepPtr SurfaceRep::New(const std::string& uid)
{
	SurfaceRepPtr retval(new SurfaceRep(uid));
	retval->mSelf = retval;
	return retval;
}

void SurfaceRep::addRepActorsToViewRenderer(View* view)
{
	view->getRenderer()->AddActor(mActor);
}

void SurfaceRep::removeRepActorsFromViewRenderer(View* view)
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

