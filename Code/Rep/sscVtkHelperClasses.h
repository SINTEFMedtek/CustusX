#ifndef SSCVTKHELPERCLASSES_H
#define SSCVTKHELPERCLASSES_H

#include "sscTransform3D.h"
#include "sscGraphicalPrimitives.h"

#include <vtkSmartPointer.h>
// forward declarations for vtk
class vtkTextProperty;
typedef vtkSmartPointer<class vtkActor2D> vtkActor2DPtr;
typedef vtkSmartPointer<class vtkCursor2D> vtkCursor2DPtr;
typedef vtkSmartPointer<class vtkPolyDataMapper2D> vtkPolyDataMapper2DPtr;
typedef vtkSmartPointer<class vtkTextMapper> vtkTextMapperPtr;
typedef vtkSmartPointer<class vtkAxesActor> vtkAxesActorPtr;
typedef vtkSmartPointer<class vtkCaptionActor2D> vtkCaptionActor2DPtr;
typedef vtkSmartPointer<class vtkProp3D> vtkProp3DPtr;
typedef vtkSmartPointer<class vtkLeaderActor2D> vtkLeaderActor2DPtr;

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
	~RGBColor() {}
};


class OffsetPoint
{
	public:
		OffsetPoint( vtkRendererPtr renderer );
		~OffsetPoint();
		void setRadius ( int radius );
		void setValue ( const Vector3D& point, const Vector3D& color );
		//void setValue ( double *xyz, uint8_t *rgb );
		//double norm ( uint8_t rgb );
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
		LineSegment( vtkRendererPtr renderer );
		~LineSegment();
		void setPoints( const Vector3D& point1, const Vector3D& point2 , const RGBColor& color, int stipplePattern=0xFFFF);
		void setResolution( int res );
		void updatePosition( const Vector3D& point1, const Vector3D& point2 );
		vtkActor2DPtr getActor();

	private:
// 		vtkPolyDataMapperPtr mapper;
		vtkPolyDataMapper2DPtr mapper2d;
 		//vtkActorPtr actor;
		vtkActor2DPtr actor2d;
		vtkRendererPtr mRenderer;
		vtkLineSourcePtr source;


};

class TextDisplay
{
	public:
		TextDisplay(const std::string& text, const Vector3D& color, int fontsize ) ;
		~TextDisplay();
		void setPosition( float x, float y);
		void setPosition( const Vector3D& pos );
		void setCentered();
		void updateText(const std::string& text );
		vtkTextProperty* textProperty();
		//	mThermalTextMapper->GetTextProperty()-> SetColor( 1.0, 0.8, 0.0);

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
 	void setCaption(vtkCaptionActor2D* captionActor, const std::string& caption, RGBColor color);

 	vtkAxesActorPtr mAxes;
	vtkRendererPtr mRenderer;
};
typedef boost::shared_ptr<Axes3D> Axes3DPtr;

/**Wrapper class for a vtkAxes. Sets up some nice defaults and draws the
 * axes in the world coordinate system.
 */
class USProbe2D
{
public:
	USProbe2D(vtkRendererPtr renderer = vtkRendererPtr());
	//~USProbe2D();
	void setPosition(const Transform3D& wMt);
	void setShape(int type, double d_start, double d_end, double width); 
//	void setRadialShape(double depth, double roc, double angularWidth);
//	void setLinearShape(double depth, double width);	
	//vtkProp3DPtr getProp();
	void setVisibility(bool val);
	
private:
	vtkLeaderActor2DPtr createLeaderActor();
	void setPosition();
 	vtkLeaderActor2DPtr mLeft;
 	vtkLeaderActor2DPtr mRight;
 	vtkLeaderActor2DPtr mArc;
	vtkRendererPtr mRenderer;
	Transform3D m_wMt;
	//boost::shared_ptr<TextDisplay> mText;
	
	int mType; ///< 0: none, 1: sector, 2: linear
	double m_d_start; ///< start depth
	double m_d_end; ///< end depth
	double mWidth; ///< width in radians or millimeters, depending on type.
};




// --------------------------------------------------------
}//	namespace vm
// --------------------------------------------------------


#endif // SSCVTKHELPERCLASSES_H

