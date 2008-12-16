#include "sscGeometricRep.h"

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkRenderer.h>

#include "sscView.h"

namespace ssc
{

GeometricRep::GeometricRep(const std::string& uid) :
	RepImpl(uid)
{

	mMapper = vtkPolyDataMapperPtr::New();
	mProperty = vtkPropertyPtr::New();
	mActor = vtkActorPtr::New();
	mActor->SetMapper( mMapper );
	mActor->SetProperty( mProperty );

}

GeometricRep::~GeometricRep()
{
	// ??
}

GeometricRepPtr GeometricRep::New(const std::string& uid)
{
	GeometricRepPtr retval(new GeometricRep(uid));
	retval->mSelf = retval;
	return retval;
}

void GeometricRep::addRepActorsToViewRenderer(View* view)
{
	view->getRenderer()->AddActor(mActor);
}

void GeometricRep::removeRepActorsFromViewRenderer(View* view)
{
	view->getRenderer()->RemoveActor(mActor);
}

void GeometricRep::setMesh(MeshPtr mesh)
{
	mMesh = mesh;
	mMesh->connectToRep(mSelf);

	mMapper->SetInput( mesh->getVtkPolyData() );
}

bool GeometricRep::hasMesh(MeshPtr mesh) const
{
	return (mMesh != NULL);
}
} // namespace ssc
