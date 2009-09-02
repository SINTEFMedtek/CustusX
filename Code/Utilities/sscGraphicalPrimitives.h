#ifndef SSCGRAPHICALPRIMITIVES_H_
#define SSCGRAPHICALPRIMITIVES_H_

#include <boost/shared_ptr.hpp>
#include <vtkSmartPointer.h>
#include "sscVector3D.h"

typedef vtkSmartPointer<class vtkActor> vtkActorPtr;
typedef vtkSmartPointer<class vtkRenderer> vtkRendererPtr;
typedef vtkSmartPointer<class vtkSphereSource> vtkSphereSourcePtr;
typedef vtkSmartPointer<class vtkPolyDataMapper> vtkPolyDataMapperPtr;
typedef vtkSmartPointer<class vtkLineSource> vtkLineSourcePtr;
typedef vtkSmartPointer<class vtkPolyDataMapper> vtkPolyDataMapperPtr;


namespace ssc
{

/** Helper for rendering a point in 3D
 */
class GraphicalPoint3D
{
	public:
		GraphicalPoint3D(vtkRendererPtr renderer = vtkRendererPtr());
		~GraphicalPoint3D();
		void setRadius(int radius );
		void setColor(const Vector3D& color );
		void setValue(const Vector3D& point);
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
		void setColor(const Vector3D& color);
		void setValue( const Vector3D& point1, const Vector3D& point2 );
		vtkActorPtr getActor();

	private:
		vtkPolyDataMapperPtr mapper;
		vtkActorPtr actor;
		vtkRendererPtr mRenderer;
		vtkLineSourcePtr source;
};
typedef boost::shared_ptr<GraphicalLine3D> GraphicalLine3DPtr;

}

#endif /*SSCGRAPHICALPRIMITIVES_H_*/
