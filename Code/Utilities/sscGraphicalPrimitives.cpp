#include "sscGraphicalPrimitives.h"

#include <vtkSphereSource.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

#include "sscBoundingBox3D.h"

namespace ssc
{

GraphicalPoint3D::GraphicalPoint3D(vtkRendererPtr renderer)
{
	mRenderer = renderer;
	source = vtkSphereSourcePtr::New();
	source->SetRadius(4);

	mapper = vtkPolyDataMapperPtr::New();
	mapper->SetInputConnection(source->GetOutputPort());

	actor = vtkActorPtr::New();
	actor->SetMapper(mapper);
	if (mRenderer)
	{
		mRenderer->AddActor(actor);
	}
}

GraphicalPoint3D::~GraphicalPoint3D()
{
	if (mRenderer)
	{
		mRenderer->RemoveActor(actor);
	}
}

void GraphicalPoint3D::setRadius(int radius)
{
	source->SetRadius(radius);
}

void GraphicalPoint3D::setColor(Vector3D color)
{
	actor->GetProperty()->SetColor(color.begin());
}

void GraphicalPoint3D::setValue(Vector3D point)
{
	actor->SetPosition(point.begin());
}

Vector3D GraphicalPoint3D::getValue() const
{
	return Vector3D(actor->GetPosition());
}

vtkActorPtr GraphicalPoint3D::getActor()
{
	return actor;
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


GraphicalLine3D::GraphicalLine3D( vtkRendererPtr renderer)
{
	mRenderer = renderer;
	source = vtkLineSourcePtr::New();
	mapper = vtkPolyDataMapperPtr::New() ;
	actor = vtkActorPtr::New() ;
	
	mapper->SetInputConnection( source->GetOutputPort() );
	actor->SetMapper (mapper );
	if (mRenderer)
		mRenderer->AddActor(actor);	
}

GraphicalLine3D::~GraphicalLine3D()
{
	if (mRenderer)
		mRenderer->RemoveActor(actor);
}

void GraphicalLine3D::setColor(Vector3D color)
{
	actor->GetProperty()->SetColor(color.begin());
}

void GraphicalLine3D::setValue(Vector3D point1, Vector3D point2)
{
	source->SetPoint1(point1.begin());
	source->SetPoint2(point2.begin());
}

void GraphicalLine3D::setStipple(int stipple)
{
	actor->GetProperty()->SetLineStipplePattern(stipple);
}

vtkActorPtr GraphicalLine3D::getActor()
{
	return actor;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

Rect3D::Rect3D(vtkRendererPtr renderer, Vector3D color)
{
  mRenderer = renderer;
  mapper = vtkPolyDataMapperPtr::New();
  actor = vtkActorPtr::New();
  actor->GetProperty()->SetColor(color.begin());
  actor->SetMapper(mapper);
  if (mRenderer)
    mRenderer->AddActor(actor);

  mPolyData = vtkPolyDataPtr::New();
  mPoints = vtkPointsPtr::New();
  mSide = vtkCellArrayPtr::New();

  vtkIdType cells[5] = { 0,1,2,3,0 };
  mSide->InsertNextCell(5, cells);

  mPolyData->SetPoints(mPoints);
  mapper->SetInput(mPolyData);
}

void Rect3D::setLine(bool on, int width)
{
  if (on)
  {
    mPolyData->SetLines(mSide);
    actor->GetProperty()->SetLineWidth(width);
  }
  else
  {
    mPolyData->SetLines(NULL);
  }
}

void Rect3D::setSurface(bool on)
{
  if (on)
  {
    mPolyData->SetPolys(mSide);
    actor->GetProperty()->SetOpacity(1.0); // transparent planes dont work well together with texture volume. Use 1.0
  }
  else
  {
    mPolyData->SetPolys(NULL);
  }
}

Rect3D::~Rect3D()
{
  if (mRenderer)
    mRenderer->RemoveActor(actor);
}

void Rect3D::updatePosition(const DoubleBoundingBox3D bb, const Transform3D& M)
{
  mPoints = vtkPointsPtr::New();
  mPoints->InsertPoint(0, M.coord(bb.corner(0,0,0)).begin());
  mPoints->InsertPoint(1, M.coord(bb.corner(0,1,0)).begin());
  mPoints->InsertPoint(2, M.coord(bb.corner(1,1,0)).begin());
  mPoints->InsertPoint(3, M.coord(bb.corner(1,0,0)).begin());
  mPolyData->SetPoints(mPoints);
  mPolyData->Update();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

}
