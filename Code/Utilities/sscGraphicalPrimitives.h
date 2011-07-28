#ifndef SSCGRAPHICALPRIMITIVES_H_
#define SSCGRAPHICALPRIMITIVES_H_

#include <boost/shared_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include "sscTransform3D.h"
#include "sscViewportListener.h"

typedef vtkSmartPointer<class vtkArcSource> vtkArcSourcePtr;
typedef vtkSmartPointer<class vtkArrowSource> vtkArrowSourcePtr;

namespace ssc
{

/** Helper for rendering a point in 3D
 */
class GraphicalPoint3D
{
	public:
		GraphicalPoint3D(vtkRendererPtr renderer = vtkRendererPtr());
		~GraphicalPoint3D();
		void setRadius(double radius);
		void setColor(Vector3D color);
		void setValue(Vector3D point);
		Vector3D getValue() const;
		vtkActorPtr getActor();
	private:
		vtkSphereSourcePtr source;
		vtkPolyDataMapperPtr mapper;
		vtkActorPtr actor;
		vtkRendererPtr mRenderer;
};
typedef boost::shared_ptr<GraphicalPoint3D> GraphicalPoint3DPtr;

/** Helper for rendering a line in 3D
 */
class GraphicalLine3D
{
	public:
		GraphicalLine3D(vtkRendererPtr renderer = vtkRendererPtr());
		~GraphicalLine3D();
		void setColor(Vector3D color);
		void setValue(Vector3D point1, Vector3D point2);
		void setStipple(int stipple);
		vtkActorPtr getActor();

	private:
		vtkPolyDataMapperPtr mapper;
		vtkActorPtr actor;
		vtkRendererPtr mRenderer;
		vtkLineSourcePtr source;
};
typedef boost::shared_ptr<GraphicalLine3D> GraphicalLine3DPtr;

/** Helper for rendering a line in 3D
 */
class GraphicalArc3D
{
  public:
  GraphicalArc3D(vtkRendererPtr renderer = vtkRendererPtr());
    ~GraphicalArc3D();
    void setColor(Vector3D color);
    void setValue(Vector3D point1, Vector3D point2, Vector3D center);
    void setStipple(int stipple);
    vtkActorPtr getActor();

  private:
    vtkPolyDataMapperPtr mapper;
    vtkActorPtr actor;
    vtkRendererPtr mRenderer;
    vtkArcSourcePtr source;
};
typedef boost::shared_ptr<GraphicalArc3D> GraphicalArc3DPtr;

/** Helper for rendering an arrow in 3D
 */
class GraphicalArrow3D
{
  public:
	GraphicalArrow3D(vtkRendererPtr renderer = vtkRendererPtr());
    ~GraphicalArrow3D();
    void setColor(Vector3D color);
    void setValue(Vector3D base, Vector3D normal, double length);

  private:
    vtkPolyDataMapperPtr mapper;
    vtkActorPtr actor;
    vtkRendererPtr mRenderer;
    vtkArrowSourcePtr source;
};
typedef boost::shared_ptr<GraphicalArrow3D> GraphicalArrow3DPtr;

class Rect3D
{
public:
  Rect3D(vtkRendererPtr renderer, Vector3D color);
  ~Rect3D();
  void updatePosition(const DoubleBoundingBox3D bb, const Transform3D& M);
  void setLine(bool on, int width);
  void setSurface(bool on);

private:
  vtkPolyDataMapperPtr mapper;
  vtkActorPtr actor;
  vtkRendererPtr mRenderer;
  vtkPolyDataPtr mPolyData;
  vtkPointsPtr mPoints;
  vtkCellArrayPtr mSide;
};
typedef boost::shared_ptr<class Rect3D> Rect3DPtr;

/** Helper for rendering 3D text that faces the camera and
 *  has a constant viewed size.
 */
class FollowerText3D
{
  public:
	FollowerText3D(vtkRendererPtr renderer = vtkRendererPtr());
    ~FollowerText3D();
    void setColor(Vector3D color);
    void setText(QString text);
    void setPosition(ssc::Vector3D pos);

    void setSize(double val);
    void setSizeInNormalizedViewport(bool on, double size);
    vtkFollowerPtr getActor();

    void scaleText(); ///< internal method

  private:
    vtkVectorTextPtr mText;
    vtkFollowerPtr mFollower;
    vtkRendererPtr mRenderer;
    double mSize;

    ssc::ViewportListenerPtr mViewportListener;
};
typedef boost::shared_ptr<FollowerText3D> FollowerText3DPtr;


}

#endif /*SSCGRAPHICALPRIMITIVES_H_*/
