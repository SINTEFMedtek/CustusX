#ifndef SSCGRAPHICALPRIMITIVES_H_
#define SSCGRAPHICALPRIMITIVES_H_

#include <boost/shared_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include "sscTransform3D.h"

namespace ssc
{

/** Helper for rendering a point in 3D
 */
class GraphicalPoint3D
{
	public:
		GraphicalPoint3D(vtkRendererPtr renderer = vtkRendererPtr());
		~GraphicalPoint3D();
		void setRadius(int radius);
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

}

#endif /*SSCGRAPHICALPRIMITIVES_H_*/
