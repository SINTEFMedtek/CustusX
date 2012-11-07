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
#include "sscTypeConversions.h"

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

RGBColor& RGBColor::operator=(const QColor& t)
{
	elems[0] = t.redF();
	elems[1] = t.greenF();
	elems[2] = t.blueF();
	return *this;
}

RGBColor::RGBColor(QColor c)
{
	elems[0] = c.redF();
	elems[1] = c.greenF();
	elems[2] = c.blueF();
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

void OffsetPoint::setValue(Vector3D point, RGBColor color)
{
	actor->GetProperty()->SetColor(color.begin());
	actor->SetPosition(point.begin());
}

void OffsetPoint::setValue(Vector3D point, Vector3D color)
{
	actor->GetProperty()->SetColor(color.begin());
	actor->SetPosition(point.begin());
}

void OffsetPoint::update( const Vector3D& position )
{
	Vector3D p = position;
	actor->SetPosition ( p.begin() );
}

vtkActor2DPtr OffsetPoint::getActor()
{
	return actor;
}


//---------------------------------------------------------//
// LINESEGMENT CLASS
//---------------------------------------------------------//


LineSegment:: LineSegment(vtkRendererPtr renderer)
{
	mRenderer = renderer;
	source = vtkLineSourcePtr::New();
	mapper2d = vtkPolyDataMapper2DPtr::New();
	actor2d = vtkActor2DPtr::New();
}

LineSegment::~LineSegment()
{
	mRenderer->RemoveActor(actor2d);
}

void LineSegment::setPoints(Vector3D point1, Vector3D point2, RGBColor color, int stipplePattern)
{
	source->SetPoint1(point1.begin());
	source->SetPoint2(point2.begin());
	mapper2d->SetInputConnection(source->GetOutputPort());
	actor2d->SetMapper(mapper2d);
	actor2d->GetProperty()->SetColor(color.begin());
	actor2d->GetProperty()->SetLineStipplePattern(stipplePattern);
	mRenderer->AddActor(actor2d);
}

void LineSegment::setResolution(int res)
{
	source->SetResolution(res);
}

void LineSegment::setWidth(float width)
{
	actor2d->GetProperty()->SetLineWidth(width);
}

void LineSegment::updatePosition(Vector3D point1, Vector3D point2)
{
	source->SetPoint1(point1.begin());
	source->SetPoint2(point2.begin());
}

void LineSegment::setColor(RGBColor color)
{
	actor2d->GetProperty()->SetColor(color.begin());
}

void LineSegment::setPattern(int stipplePattern)
{
	actor2d->GetProperty()->SetLineStipplePattern(stipplePattern);
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

	DoubleBoundingBox3D bb = vp;
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
TextDisplay::TextDisplay( const QString& text, const Vector3D& color, int fontsize )
{
	this->text = text;
	Vector3D c = color;
	mapper = vtkTextMapperPtr::New();
	mapper->SetInput( cstring_cast(text) );
	mapper->GetTextProperty()->SetColor( c.begin() );
	mapper->GetTextProperty()->SetFontSize( fontsize );

	actor= vtkActor2DPtr::New();
	actor->SetMapper( mapper );
	maxWidth = 0;
}

TextDisplay::~TextDisplay()
{
}

void TextDisplay::setMaxWidth( int width, vtkViewport *vp)
{
	maxWidth = width;
	QStringList components = text.split("\n");
	for (QStringList::iterator it = components.begin(); it != components.end(); ++it)
	{
		vtkTextMapperPtr line = vtkTextMapperPtr::New();
		bool changed = false;
		line->SetInput( it->toLatin1().constData() );
		line->SetTextProperty(mapper->GetTextProperty());
		while((*it).length() > 0 && line->GetWidth(vp) > maxWidth)
		{
			(*it).chop(1);
			line->SetInput( QString("%1...").arg(*it).toLatin1().constData() );
			changed = true;
		}
		if (changed)
		{
			(*it).append("...");
		}
	}
	QString newString = components.join("\n");
	mapper->SetInput( newString.toLatin1().constData() );
}

int TextDisplay::getMaxWidth()
{
	return maxWidth;
}

int TextDisplay::getWidth( vtkViewport *vp)
{
	return mapper->GetWidth( vp );
}

void TextDisplay::setPosition( const Vector3D& pos )
{
	actor->SetPosition( pos[0], pos[1] );
}

void TextDisplay::setPosition( float x, float y  )
{
	actor->SetPosition( x, y );
}

void TextDisplay::updateText( const QString& text)
{
	mapper->SetInput( cstring_cast(text) );
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

 	setPosition(Transform3D::Identity());

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

 	mAxes->SetUserMatrix((S*pos).getVtkMatrix());
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

void Axes3D::setCaption(vtkCaptionActor2D* captionActor, const QString& caption, RGBColor color)
{
	captionActor->SetCaption(cstring_cast(caption));
	captionActor->GetCaptionTextProperty()->SetColor(color.begin());

	double fontsize = 0.02;
	captionActor->SetWidth(fontsize);
	captionActor->SetHeight(fontsize);
}

}//	namespace vm
// --------------------------------------------------------
