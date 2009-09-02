#include "sscGraphicalPrimitives.h"

#include <vtkSphereSource.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>


namespace ssc
{

GraphicalPoint3D::GraphicalPoint3D( vtkRendererPtr renderer )
{
	mRenderer = renderer;
	source = vtkSphereSourcePtr::New() ;
	source->SetRadius( 4);

	mapper = vtkPolyDataMapperPtr::New() ;
	mapper->SetInputConnection(source->GetOutputPort() );

	actor = vtkActorPtr::New();
	actor->SetMapper(mapper);
	if (mRenderer)
		mRenderer->AddActor(actor);
}

GraphicalPoint3D::~GraphicalPoint3D()
{
	if (mRenderer)
		mRenderer->RemoveActor(actor);
}

void GraphicalPoint3D::setRadius(int radius)
{
	source->SetRadius(radius);
}

void GraphicalPoint3D::setColor(const Vector3D& color )
{
	Vector3D c = color;
	actor->GetProperty()->SetColor(c.begin());	
}

void GraphicalPoint3D::setValue(const Vector3D& point)
{
	Vector3D p = point;
	actor->SetPosition(p.begin());
}

Vector3D GraphicalPoint3D::getValue() const
{
	Vector3D pos (actor->GetPosition()) ;
	return pos;
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

void GraphicalLine3D::setColor(const Vector3D& color)
{
	Vector3D c = color;
	actor->GetProperty()->SetColor( c.begin() );
}

void GraphicalLine3D::setValue( const Vector3D& point1, const Vector3D& point2 )
{
	Vector3D c = point1;
	Vector3D p = point2;
	source->SetPoint1 ( c.begin() );
	source->SetPoint2 ( p.begin() );
}

vtkActorPtr GraphicalLine3D::getActor()
{
	return actor;
}


}
