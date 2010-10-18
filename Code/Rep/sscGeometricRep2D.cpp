#include "sscGeometricRep2D.h"

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>

#include <vtkPolyDataNormals.h>
#include <vtkCutter.h>
#include <vtkPlane.h>
#include <vtkStripper.h>
#include <vtkCellArray.h>

#include "sscMesh.h"
#include "sscView.h"
#include "sscMessageManager.h"
#include "sscSliceProxy.h"

namespace ssc
{

GeometricRep2D::GeometricRep2D(const QString& uid, const QString& name) :
	RepImpl(uid, name)
{
  mNormals = vtkPolyDataNormalsPtr::New();
	mMapper = vtkPolyDataMapperPtr::New();
	mProperty = vtkPropertyPtr::New();
	mActor = vtkActorPtr::New();
	mCutter = vtkCutterPtr::New();
	mCutPlane = vtkPlanePtr::New();
	mStripper = vtkStripperPtr::New();
  mCutPoly = vtkPolyDataPtr::New();

  //mNormals->SetInputConnection(mMesh->getVtkPolyData()->GetOutputPort());
  // set plane def

	mCutter->SetInputConnection(mNormals->GetOutputPort());

//cutEdges = vtk.vtkCutter()
//  cutEdges.SetInputConnection(cowNormals.GetOutputPort())
	mCutter->SetCutFunction(mCutPlane);
	mCutter->GenerateCutScalarsOn();
	mCutter->SetValue(0, 0);

  mStripper->SetInputConnection(mCutter->GetOutputPort());
  //mStripper->Update(); // ????
  mCutPoly->SetPoints(mStripper->GetOutput()->GetPoints());
  mCutPoly->SetLines(mStripper->GetOutput()->GetLines());

  mMapper->SetInput(mCutPoly);
  // 72 cutMapper.SetInputConnection(cutTriangles.GetOutputPort())

	mActor->SetMapper( mMapper );
	mActor->SetProperty( mProperty );
}
GeometricRep2D::~GeometricRep2D()
{}
GeometricRep2DPtr GeometricRep2D::New(const QString& uid, const QString& name)
{
	GeometricRep2DPtr retval(new GeometricRep2D(uid, name));
	retval->mSelf = retval;
	return retval;
}
void GeometricRep2D::addRepActorsToViewRenderer(View* view)
{
	view->getRenderer()->AddActor(mActor);
}
void GeometricRep2D::removeRepActorsFromViewRenderer(View* view)
{
	view->getRenderer()->RemoveActor(mActor);
}
void GeometricRep2D::setMesh(MeshPtr mesh)
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

	mMapper->SetInput( mMesh->getVtkPolyData() ); // original - show-all method
	//mNormals->SetInputConnection(mMesh->getVtkPolyData()->Get);
	mNormals->SetInput(mMesh->getVtkPolyData());
//	mCutter->SetInput(mMesh->getVtkPolyData());
  
  //Set mesh color
  mActor->GetProperty()->SetColor(mMesh->getColor().redF(),
                                  mMesh->getColor().greenF(),
                                  mMesh->getColor().blueF());
  //Set mesh opacity
  mActor->GetProperty()->SetOpacity(mMesh->getColor().alphaF());
}

/**called when transform is changed
 * reset it in the prop.*/
void GeometricRep2D::transformChangedSlot()
{
  if (!mSlicer || !mMesh)
    return;

  Transform3D rMs = mSlicer->get_sMr().inv();
  Transform3D dMr = mMesh->get_rMd().inv();
  Transform3D dMs = dMr*rMs;

  ssc::Vector3D n = dMs.vector(ssc::Vector3D(0,0,1));
  ssc::Vector3D p = dMs.coord(ssc::Vector3D(0,0,0));
  mCutPlane->SetNormal(n.begin());
  mCutPlane->SetOrigin(p.begin());
//  mStripper->Update();
  //mStripper->Print(std::cout);
  //mStripper->GetOutput()->GetPoints()->Print(std::cout);
  //mStripper->GetOutput()->GetLines()->Print(std::cout);
  mCutPoly->SetPoints(mStripper->GetOutput()->GetPoints());
  mCutPoly->SetLines(mStripper->GetOutput()->GetLines());
  //mCutPoly->Print(std::cout);

  mActor->SetUserMatrix(dMs.inv().matrix());
}

//---------------------------------------------------------
} // namespace ssc
//---------------------------------------------------------

