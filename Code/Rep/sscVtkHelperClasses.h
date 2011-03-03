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
/*Wrapper class for adding orientation text
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
	private:


		vtkTextMapperPtr mapper;
		vtkActor2DPtr actor;
		vtkRendererPtr mRenderer;

};
typedef boost::shared_ptr<class TextDisplay> TextDisplayPtr;


/**
 * Wrapper class for the croos hair in ACS
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

/**Wrapper class for a vtkAxes. Sets up some nice defaults and draws the
 * axes in the world coordinate system.
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

