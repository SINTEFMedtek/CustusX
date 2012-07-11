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

#ifndef SSCVTKHELPERCLASSES_H
#define SSCVTKHELPERCLASSES_H

#include "sscTransform3D.h"
#include "sscGraphicalPrimitives.h"

#include "vtkForwardDeclarations.h"

#include <QColor>

// --------------------------------------------------------
namespace ssc
{
// --------------------------------------------------------

/**\brief RGB color data.
 *
 * \ingroup sscProxy
 */
class RGBColor : public boost::array<double,3>
{
public:
	RGBColor();
	RGBColor(double r, double g, double b);
	explicit RGBColor(const double* rgb);
	RGBColor(const RGBColor& t);
	RGBColor& operator=(const RGBColor& t);
	RGBColor(QColor c);
	RGBColor& operator=(const QColor& c);
	~RGBColor() {}
};

/**\brief Helper for drawing a point in 2D.
 *
 * \ingroup sscProxy
 */
class OffsetPoint
{
	public:
		OffsetPoint( vtkRendererPtr renderer );
		~OffsetPoint();
		void setRadius ( int radius );
		void setValue(Vector3D point, RGBColor color);
		void setValue(Vector3D point, Vector3D color);
		void update( const Vector3D& point );
		vtkActor2DPtr getActor();
	private:
		vtkSphereSourcePtr source;
		vtkPolyDataMapper2DPtr mapper;
		vtkActor2DPtr actor;
		vtkRendererPtr mRenderer;
};
typedef boost::shared_ptr<OffsetPoint> OffsetPointPtr;

/**\brief Helper for drawing a line in 2D.
 *
 * \ingroup sscProxy
 */
class LineSegment
{
	public:
		LineSegment(vtkRendererPtr renderer);
		~LineSegment();
		void setPoints(Vector3D point1, Vector3D point2, RGBColor color, int stipplePattern = 0xFFFF);
		void setResolution(int res);
		void updatePosition(Vector3D point1, Vector3D point2);
		vtkActor2DPtr getActor();
		void setColor(RGBColor color);
		void setPattern(int stipplePattern);
		void setWidth(float width);

	private:
		vtkPolyDataMapper2DPtr mapper2d;
		vtkActor2DPtr actor2d;
		vtkRendererPtr mRenderer;
		vtkLineSourcePtr source;
};

/**\brief Helper for drawing text in 2D.
 *
 * \ingroup sscProxy
 */
class TextDisplay
{
	public:
		TextDisplay(const QString& text, const Vector3D& color, int fontsize);
		~TextDisplay();
		void setPosition( float x, float y);
		void setPosition( const Vector3D& pos );
		void setCentered();
		void updateText(const QString& text );
		vtkTextProperty* textProperty();
		vtkActor2DPtr getActor();
		void setMaxWidth( int width, vtkViewport *vp );
		int getMaxWidth();
		int getWidth( vtkViewport *vp );
	private:
		vtkTextMapperPtr mapper;
		vtkActor2DPtr actor;
		vtkRendererPtr mRenderer;
		int maxWidth;
		QString text;
};
typedef boost::shared_ptr<class TextDisplay> TextDisplayPtr;


/**\brief Helper for drawing a crosshair in 2D.
 *
 * \ingroup sscProxy
 */
class CrossHair2D
{
	public:
		CrossHair2D( vtkRendererPtr renderer);
		~CrossHair2D();
		void setValue(const Vector3D& focalPoint, int width, int height, double bordarOffset, const RGBColor& color);
		//void update( const Vector3D& crossPos );
		void update( const Vector3D& crossPos, const DoubleBoundingBox3D& vp);
		void updateRegCross( const Vector3D& crossPos );
		vtkActor2DPtr getActor();
	private:
		vtkCursor2DPtr mCursor2D;
		vtkPolyDataMapper2DPtr mapper;
		vtkActor2DPtr actor;
		vtkRendererPtr mRenderer;
};
typedef boost::shared_ptr<class CrossHair2D> CrossHair2DPtr;

/**\brief Helper for drawing a coordinate axis in 3D.
 *
 * Wrapper class for a vtkAxes. Sets up some nice defaults and draws the
 * axes in the world coordinate system.
 *
 * \ingroup sscProxy
 */
class Axes3D
{
public:
	Axes3D(vtkRendererPtr renderer = vtkRendererPtr());
	~Axes3D();
	void setPosition(const Transform3D& pos);
	vtkProp3DPtr getProp();
private:
 	void setCaption(vtkCaptionActor2D* captionActor, const QString& caption, RGBColor color);

 	vtkAxesActorPtr mAxes;
	vtkRendererPtr mRenderer;
};
typedef boost::shared_ptr<Axes3D> Axes3DPtr;

// --------------------------------------------------------
}//	namespace vm
// --------------------------------------------------------


#endif // SSCVTKHELPERCLASSES_H

