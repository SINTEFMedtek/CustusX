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

namespace ssc
{

GeometricRep::GeometricRep(const std::string& uid, const std::string& name) :
	RepImpl(uid, name)
{
	mMapper = vtkPolyDataMapperPtr::New();
	mProperty = vtkPropertyPtr::New();
	mActor = vtkActorPtr::New();
	mActor->SetMapper( mMapper );
	mActor->SetProperty( mProperty );
}
GeometricRep::~GeometricRep()
{}
GeometricRepPtr GeometricRep::New(const std::string& uid, const std::string& name)
{
	GeometricRepPtr retval(new GeometricRep(uid, name));
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
  if (mesh == mMesh)
    return;
  if(mMesh)
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
	return (mMesh != NULL);
}
  
void GeometricRep::meshChangedSlot()
{
	mMesh->connectToRep(mSelf);
  
	mMapper->SetInput( mMesh->getVtkPolyData() );
  
  //Set mesh color
  mActor->GetProperty()->SetColor(mMesh->getColor().redF(),
                                  mMesh->getColor().greenF(),
                                  mMesh->getColor().blueF());
  //Set mesh opacity
  mActor->GetProperty()->SetOpacity(mMesh->getColor().alphaF());
}

/**called when transform is changed
 * reset it in the prop.*/
void GeometricRep::transformChangedSlot()
{
  if (!mMesh)
  {
    return;
  }
  mActor->SetUserMatrix(mMesh->get_rMd().matrix());
}

  
//---------------------------------------------------------
} // namespace ssc
//---------------------------------------------------------

