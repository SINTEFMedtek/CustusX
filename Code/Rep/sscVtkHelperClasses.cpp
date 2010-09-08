//
// C++ Implementation: vmVtkUtilites
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "sscVtkHelperClasses.h"

#include <vtkSphereSource.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkAxesActor.h>
#include <vtkCaptionActor2D.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderer.h>
#include <vtkCursor2D.h>
#include <vtkLeaderActor2D.h>

#include "sscBoundingBox3D.h"

// --------------------------------------------------------
namespace ssc
{

RGBColor::RGBColor() {}

RGBColor::RGBColor(const RGBColor& t)
{
	for (unsigned i=0; i<size(); ++i)
		elems[i] = t.elems[i];
}

RGBColor& RGBColor::operator=(const RGBColor& t)
{
	if (this!=&t)
	{
		for (unsigned i=0; i<size(); ++i)
			elems[i] = t.elems[i];
	}
	return *this;
}


RGBColor::RGBColor(double r, double g, double b)
{
	elems[0] = r;
	elems[1] = g;
	elems[2] = b;
}

RGBColor::RGBColor(const double* rgb)
{
	elems[0] = rgb[0];
	elems[1] = rgb[1];
	elems[2] = rgb[2];
}


// --------------------------------------------------------
// OFFSEET POINT CLASS
// --------------------------------------------------------


OffsetPoint::OffsetPoint( vtkRendererPtr renderer )
{
	mRenderer = renderer;
	source = vtkSphereSourcePtr::New() ;
	source->SetRadius( 4);

	mapper = vtkPolyDataMapper2DPtr::New() ;
	mapper->SetInputConnection(source->GetOutputPort() );

	actor = vtkActor2DPtr::New();
	actor->SetMapper(mapper);
	mRenderer->AddActor(actor);
}

OffsetPoint::~OffsetPoint()
{
	mRenderer->RemoveActor(actor);

}

void OffsetPoint::setRadius(int radius)
{
	source->SetRadius(radius);
}

void OffsetPoint::setValue(const Vector3D& point, const Vector3D& color)
{
	Vector3D p = point;
	Vector3D c = color;
	actor->GetProperty()->SetColor(c.begin());
	actor->SetPosition( p.begin());
}

//void OffsetPoint::setValue(double *xyz, uint8_t *rgb)
//{
//	actor->GetProperty()->SetColor(norm(rgb[0]), norm(rgb[1]), norm(rgb[2]));
//	actor->SetPosition(xyz[0], xyz[1]);
//}
//
//double OffsetPoint::norm(uint8_t rgb)
//{
//	return (double) rgb *(1.0/255.0);
//}

void OffsetPoint::update( const Vector3D& position )
{
	Vector3D p = position;
	actor->SetPosition ( p.begin() );
	//mapper->Update();
}

vtkActor2DPtr OffsetPoint::getActor()
{
	return actor;
}


// --------------------------------------------------------
// PICKER POINT CLASS
// --------------------------------------------------------




//---------------------------------------------------------//
// LINESEGMENT CLASS
//---------------------------------------------------------//

LineSegment:: LineSegment( vtkRendererPtr renderer )
{
	mRenderer = renderer;
	source = vtkLineSourcePtr::New();
	mapper2d = vtkPolyDataMapper2DPtr::New() ;
	actor2d = vtkActor2DPtr::New() ;
}

LineSegment::~LineSegment()
{
	mRenderer->RemoveActor(actor2d);
}
void LineSegment::setPoints( const Vector3D& point1, const Vector3D& point2 , const RGBColor& color, int stipplePattern )
{
	Vector3D p1 = point1;
	Vector3D p2 = point2;
	RGBColor c = color;
	source->SetPoint1 ( p1.begin() );
	source->SetPoint2 ( p2.begin() );

	mapper2d->SetInputConnection( source->GetOutputPort() );
	actor2d->SetMapper (mapper2d );
	actor2d->GetProperty()->SetColor( c.begin() );
	actor2d->GetProperty()->SetLineStipplePattern (stipplePattern);
	mRenderer->AddActor(actor2d);
}

void LineSegment::setResolution( int res )
{
	source->SetResolution( res );
}

void LineSegment::updatePosition( const Vector3D& point1, const Vector3D& point2 )
{
	//SW_LOG(" ");
	Vector3D c = point1;
	Vector3D p = point2;
	source->SetPoint1 ( c.begin() );
	source->SetPoint2 ( p.begin() );
	//mapper2d->Update();

}
vtkActor2DPtr LineSegment::getActor()
{
	return actor2d;
}

// --------------------------------------------------------
// CROSS HAIR CLASS
// --------------------------------------------------------

CrossHair2D::CrossHair2D( vtkRendererPtr renderer )
{
	mRenderer = renderer;
	mCursor2D = vtkCursor2DPtr::New();
	mapper = vtkPolyDataMapper2DPtr::New();
	mapper->SetInputConnection ( mCursor2D->GetOutputPort() );

	actor = vtkActor2DPtr::New();
	actor->SetMapper ( mapper );
	mRenderer->AddActor ( actor );
}

CrossHair2D::~CrossHair2D( )
{
	mRenderer->RemoveActor(actor);
}

void CrossHair2D::setValue(const Vector3D& focalPoint, int width, int height, double bordarOffset, const RGBColor& color )
{
	RGBColor c = color;
	Vector3D p = focalPoint;
	bordarOffset = 0.0;
	mCursor2D->SetModelBounds ( bordarOffset, width - bordarOffset, bordarOffset, height - bordarOffset, 0.0, 0.0 );
	//SW_LOG(" seting focal point",p[0]/2, p[1]/2 );
	mCursor2D->SetFocalPoint ( p[0]/2, p[1]/2, 0.0 );
	mCursor2D->SetRadius ( 5 );
	mCursor2D->AllOff();
	mCursor2D->AxesOn();
	mCursor2D->PointOn();
	mCursor2D->OutlineOff();
	actor->GetProperty()-> SetColor ( c.begin() );
}
/**special made update for crosshair in registration
 */
void CrossHair2D::updateRegCross( const Vector3D& crossPos )
{
	Vector3D cp = crossPos;
	if ( mCursor2D )
	{
		mCursor2D->SetModelBounds( cp[0]-50, cp[0]+50, cp[1]-50, cp[1]+50, 0.0, 0.0 );
		mCursor2D->SetFocalPoint ( cp.begin() );
		mCursor2D->Update();
	}
}
void CrossHair2D::update( const Vector3D& crossPos, const DoubleBoundingBox3D& vp)
{
	Vector3D cp = crossPos;
	cp[2] = 0.0; // make it fit inside the z bounds.

//	int* sz = mRenderer->GetSize();
//	std::cout << "CrossHair2D::update sz: " << sz[0] << "," << sz[1] << " , cp: " << cp << std::endl;
	DoubleBoundingBox3D bb = vp;
	//DoubleBoundingBox3D bb(0, sz[0], 0, sz[1], -0.5, 0.5);
	if (!bb.contains(cp))
	{
		mCursor2D->AllOff();
	}
	else
	{
		mCursor2D->AllOff();
		mCursor2D->AxesOn();
		mCursor2D->PointOn();
		mCursor2D->OutlineOff();
	}

	if ( mCursor2D )
	{
		mCursor2D->SetFocalPoint ( cp.begin() );
		mCursor2D->Update();
	}
	if (mapper)
	{
		//mapper->Update();
	}
}
vtkActor2DPtr  CrossHair2D::getActor()
{
	return actor;
}

// --------------------------------------------------------
// TEXT DISPLAY CLASS
// --------------------------------------------------------
TextDisplay::TextDisplay( const std::string& text, const Vector3D& color, int fontsize )
{
	Vector3D c = color;
	mapper = vtkTextMapperPtr::New();
	mapper->SetInput( text.c_str() );
	mapper->GetTextProperty()-> SetColor( c.begin() );
	mapper->GetTextProperty()-> SetFontSize( fontsize );

	actor= vtkActor2DPtr::New();
	actor->SetMapper( mapper );
}

TextDisplay::~TextDisplay()
{
}

void TextDisplay::setPosition( const Vector3D& pos )
{
	actor->SetPosition( pos[0], pos[1] );
}
void TextDisplay::setPosition( float x, float y  )
{
	actor->SetPosition( x, y );
}

void TextDisplay::updateText( const std::string& text)
{
	mapper->SetInput( text.c_str() );
	//mapper->Update();
}
vtkTextProperty* TextDisplay::textProperty()
{
	return mapper->GetTextProperty();
}
vtkActor2DPtr TextDisplay::getActor()
{
	return actor;
}

void TextDisplay::setCentered()
{
	mapper->GetTextProperty()->SetJustificationToCentered() ;
	mapper->GetTextProperty()->SetVerticalJustificationToCentered() ;
}

// --------------------------------------------------------
// Axes3D CLASS
// --------------------------------------------------------

Axes3D::Axes3D(vtkRendererPtr renderer) : mRenderer(renderer)
{
	mAxes = vtkSmartPointer<vtkAxesActor>::New();

 	setCaption(mAxes->GetXAxisCaptionActor2D(), "x", RGBColor(1,0,0));
 	setCaption(mAxes->GetYAxisCaptionActor2D(), "y", RGBColor(0,1,0));
 	setCaption(mAxes->GetZAxisCaptionActor2D(), "z", RGBColor(0,0,1));

 	//SW_LOG("total len %s", string_cast(Vector3D(axes->GetTotalLength())).c_str());
 	//SW_LOG("scale %s", string_cast(Vector3D(mAxes->GetScale())).c_str());

 	setPosition(Transform3D());

 	Vector3D len(mAxes->GetNormalizedShaftLength());
 	//SW_LOG("len %s", string_cast(len).c_str());
 	//len = Vector3D(0.1,0.1,0.1);
 	mAxes->SetNormalizedShaftLength(len.begin());

	if (mRenderer)
		mRenderer->AddActor(mAxes);
}

/**Set the transform the axis will represent
 */
void Axes3D::setPosition(const Transform3D& pos)
{
 	double scale = 20;
 	//axes->SetScale(Vector3D(scale,scale,scale).begin());
 	Transform3D S = createTransformScale(Vector3D(scale,scale,scale));

 	mAxes->SetUserMatrix((S*pos).matrix());
}

Axes3D::~Axes3D()
{
	if (mRenderer)
		mRenderer->RemoveActor(mAxes);
}

vtkProp3DPtr Axes3D::getProp()
{
	return mAxes;
}

void Axes3D::setCaption(vtkCaptionActor2D* captionActor, const std::string& caption, RGBColor color)
{
	captionActor->SetCaption(caption.c_str());
	captionActor->GetCaptionTextProperty()->SetColor(color.begin());

	double fontsize = 0.02;
	captionActor->SetWidth(fontsize);
	captionActor->SetHeight(fontsize);
}

// --------------------------------------------------------
// USProbe2D CLASS
// --------------------------------------------------------


USProbe2D::USProbe2D(vtkRendererPtr renderer) : mRenderer(renderer)
{
//	mType = 0;
//	m_d_start = 0;
//	m_d_end = 0;
//	mWidth = 0;

	//setShape(1, 5, 50, M_PI_2); // sector probe test
	//setShape(2, 5, 50, 25); // linear probe test
	
//	mText.reset(new TextDisplay("USProbe2D REP", RGBColor(1,0,0), 12));	
//	mText->setPosition(Vector3D(0.5,0.5,0));
//	mText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToWorld();

	mLeft = createLeaderActor();
	mRight = createLeaderActor();
	mArc = createLeaderActor();
	
//	if (mRenderer)
//	{
//		mRenderer->AddActor(mText->getActor());
//	}

	setShape(0, 0, 0, 0);
//	mText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
//	textRep->setPosition(pos);	
}

USProbe2D::~USProbe2D()
{
  if (mRenderer)
  {
    mRenderer->RemoveActor(mLeft);
    mRenderer->RemoveActor(mRight);
    mRenderer->RemoveActor(mArc);
  }
}

vtkLeaderActor2DPtr USProbe2D::createLeaderActor()
{
	if (!mRenderer)
	{
		return vtkLeaderActor2DPtr(); 
	}
	
	Vector3D color(1.0, 1.0, 1.0);
	vtkLeaderActor2DPtr retval = vtkLeaderActor2DPtr::New();
	retval->GetPositionCoordinate()->SetCoordinateSystemToWorld();
	retval->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
	retval->GetProperty()->SetColor(color.begin());
	retval->SetArrowPlacementToNone();
	mRenderer->AddActor(retval);
	return retval;
}

void USProbe2D::setShape(int type, double d_start, double d_end, double width)
{
	mType = type;
	m_d_start = d_start;
	m_d_end = d_end;
	mWidth = width;
	setPosition();
}

void USProbe2D::setPosition(const Transform3D& wMt)
{
	m_wMt = wMt;
	setPosition();
}

namespace // unnamed 
{
	/**compute the planar unit vector for a given angle
	 */
	Vector3D unitVector(double angle)
	{
		return Vector3D(cos(angle), sin(angle), 0.0);
	}
} // unnamed

void USProbe2D::setPosition()
{
	if (!mRenderer)
		return;
	
	Vector3D p(0,0,0); // tool position in local space
	
	// first define the shape of the probe in a xy-plane.
	// after that, transform into yz-plane because thats the tool plane (probe point towards positive z)
	// then transform to global space.
	Transform3D tMl = createTransformIJC(Vector3D(0,1,0), Vector3D(0,0,1), Vector3D(0,0,0));
	
	Transform3D M = m_wMt*tMl;	
	Vector3D e_x = unitVector(0); 
	Vector3D e_y = unitVector(M_PI_2); 
	Vector3D e_z(0,0,1);
	
	mArc->SetVisibility(mLeft->GetVisibility());	
	
	if (mType==1) // sector
	{
		Vector3D c = - m_d_start * e_y;  // arc center point
		Vector3D pr = c + m_d_end * unitVector(M_PI_2 - mWidth/2.0); // left endpoint
		Vector3D pl = c + m_d_end * unitVector(M_PI_2 + mWidth/2.0); // right endpoint
		double r = - m_d_end / (pl-pr).length();		// normalize on distance between endpoints (vtk spec)
		
		c = M.coord(c);
		pr = M.coord(pr);
		pl = M.coord(pl);
		// change sign on r??

		mLeft->GetPositionCoordinate()->SetValue(c.begin());
		mLeft->GetPosition2Coordinate()->SetValue(pl.begin());

		mRight->GetPositionCoordinate()->SetValue(c.begin());
		mRight->GetPosition2Coordinate()->SetValue(pr.begin());

		// turn off arc for very tilted views. This because the projection is bad.
		Vector3D z = M.vector(e_z);
		double straightness = dot(M.vector(e_z), e_z);
		if (fabs(straightness) < 0.5)
		{
			mArc->SetVisibility(false);
		}
		
		mArc->GetPositionCoordinate()->SetValue(pr.begin());
		mArc->GetPosition2Coordinate()->SetValue(pl.begin());
		mArc->SetRadius(r);

	}
	if (mType==2) // linear 
	{
		Vector3D cr = m_d_start * e_y + mWidth/2 * e_x; 
		Vector3D cl = m_d_start * e_y - mWidth/2 * e_x; 
		
		Vector3D pr = m_d_end * e_y + mWidth/2 * e_x; 
		Vector3D pl = m_d_end * e_y - mWidth/2 * e_x; 
		
		cl = M.coord(cl);
		cr = M.coord(cr);
		pl = M.coord(pl);
		pr = M.coord(pr);

		mLeft->GetPositionCoordinate()->SetValue(cl.begin());
		mLeft->GetPosition2Coordinate()->SetValue(pl.begin());

		mRight->GetPositionCoordinate()->SetValue(cr.begin());
		mRight->GetPosition2Coordinate()->SetValue(pr.begin());

		mArc->GetPositionCoordinate()->SetValue(pr.begin());
		mArc->GetPosition2Coordinate()->SetValue(pl.begin());
		mArc->SetRadius(0);
	}
	
//	Vector3D left(0,100,100);
//	Vector3D right(0,-100,100);
//	
//	center = mPosition.coord(center);
//	left = mPosition.coord(left);
//	right = mPosition.coord(right);
//	
//	mText->setPosition(center);	
//	
//	mLeft->GetPositionCoordinate()->SetValue(center.begin());
//	mLeft->GetPosition2Coordinate()->SetValue(left.begin());
//
//	mRight->GetPositionCoordinate()->SetValue(center.begin());
//	mRight->GetPosition2Coordinate()->SetValue(right.begin());
//
//	mArc->GetPositionCoordinate()->SetValue(left.begin());
//	mArc->GetPosition2Coordinate()->SetValue(right.begin());
//	mArc->SetRadius( 100 );
	
//	Logger::log("vm.log", "USProbe2D::setPosition(), center=" + string_cast(center));	
//	Logger::log("vm.log", "USProbe2D::setPosition(), left=" + string_cast(left));	
//	Logger::log("vm.log", "USProbe2D::setPosition(), right=" + string_cast(right));	
//	Logger::log("vm.log", "-------------");	
//	setVisibility(true);
}

void USProbe2D::setVisibility(bool val)
{
//	val=true;
	mLeft->SetVisibility(val);
	mRight->SetVisibility(val);
	mArc->SetVisibility(val);	
}


}//	namespace vm
// --------------------------------------------------------


