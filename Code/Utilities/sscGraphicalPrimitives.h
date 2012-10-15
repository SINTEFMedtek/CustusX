#ifndef SSCGRAPHICALPRIMITIVES_H_
#define SSCGRAPHICALPRIMITIVES_H_

#include <boost/shared_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include "sscTransform3D.h"
#include "sscViewportListener.h"
#include <vtkPolyDataAlgorithm.h>

typedef vtkSmartPointer<class vtkPolyDataAlgorithm> vtkPolyDataAlgorithmPtr;
typedef vtkSmartPointer<class vtkArcSource> vtkArcSourcePtr;
typedef vtkSmartPointer<class vtkArrowSource> vtkArrowSourcePtr;

namespace ssc
{

/**
 * \addtogroup sscUtility
 * @{
 */

/** \brief Helper for rendering a a polydata in 3D
 */
class GraphicalPolyData3D
{
public:
	GraphicalPolyData3D(
			vtkPolyDataAlgorithmPtr source = vtkPolyDataAlgorithmPtr(),
			vtkRendererPtr renderer = vtkRendererPtr());
	void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());
	void setSource(vtkPolyDataAlgorithmPtr source);
	void setData(vtkPolyDataPtr data);
	~GraphicalPolyData3D();
	void setColor(Vector3D color);
	void setPosition(Vector3D point);
	Vector3D getPosition() const;

	vtkActorPtr getActor();
	vtkPolyDataPtr getPolyData();
	vtkPolyDataAlgorithmPtr getSource();

private:
	vtkPolyDataAlgorithmPtr mSource;
	vtkPolyDataPtr mData;
	vtkPolyDataMapperPtr mMapper;
	vtkActorPtr mActor;
	vtkRendererPtr mRenderer;
};
typedef boost::shared_ptr<GraphicalPolyData3D> GraphicalPolyData3DPtr;


/** \brief Helper for rendering a point in 3D
 */
class GraphicalPoint3D
{
	public:
		GraphicalPoint3D(vtkRendererPtr renderer = vtkRendererPtr());
    void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());
		~GraphicalPoint3D();
		void setRadius(double radius);
		void setColor(Vector3D color);
		void setValue(Vector3D point);
		Vector3D getValue() const;
		vtkActorPtr getActor();
		vtkPolyDataPtr getPolyData();

	private:
		vtkSphereSourcePtr source;
		vtkPolyDataMapperPtr mapper;
		vtkActorPtr actor;
		vtkRendererPtr mRenderer;
};
typedef boost::shared_ptr<GraphicalPoint3D> GraphicalPoint3DPtr;

/** \brief Helper for rendering a line in 3D
 */
class GraphicalLine3D
{
	public:
		GraphicalLine3D(vtkRendererPtr renderer = vtkRendererPtr());
    void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());
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

/** \brief Helper for rendering a line in 3D
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

/** \brief Helper for rendering an arrow in 3D
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

/**\brief Helper for drawing a rectangle in 3D.
 */
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

/** \brief Helper for rendering 3D text that faces the camera and
 *  has a constant viewed size.
 */
class FollowerText3D
{
  public:
	FollowerText3D(vtkRendererPtr renderer = vtkRendererPtr());
  void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());

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

/** \brief Helper for rendering 3D text that faces the camera and
 *  has a constant viewed size, always on top.
 */
class CaptionText3D
{
public:
	CaptionText3D(vtkRendererPtr renderer = vtkRendererPtr());
	void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());

	~CaptionText3D();
	void setColor(Vector3D color);
	void setText(QString text);
	void setPosition(ssc::Vector3D pos);

	void setSize(double val);
	//    void setSizeInNormalizedViewport(bool on, double size);
	vtkCaptionActor2DPtr getActor();

	//    void scaleText(); ///< internal method

private:
	vtkCaptionActor2DPtr mText;
	//    vtkFollowerPtr mFollower;
	vtkRendererPtr mRenderer;

	//    ssc::ViewportListenerPtr mViewportListener;
};
typedef boost::shared_ptr<CaptionText3D> CaptionText3DPtr;

/**
 * @}
 */

}

#endif /*SSCGRAPHICALPRIMITIVES_H_*/
