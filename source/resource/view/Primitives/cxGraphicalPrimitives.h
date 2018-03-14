/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXGRAPHICALPRIMITIVES_H_
#define CXGRAPHICALPRIMITIVES_H_

#include "cxResourceVisualizationExport.h"

#include <boost/shared_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include "cxTransform3D.h"
#include "cxViewportListener.h"
#include <vtkPolyDataAlgorithm.h>
#include <vtkGlyph3DMapper.h>
#include <vtkColorSeries.h>
#include <vtkLookupTable.h>
#include <vtkMapper.h>

class QColor;
typedef vtkSmartPointer<class vtkPolyDataAlgorithm> vtkPolyDataAlgorithmPtr;
typedef vtkSmartPointer<class vtkArcSource> vtkArcSourcePtr;
typedef vtkSmartPointer<class vtkArrowSource> vtkArrowSourcePtr;

namespace cx
{


class cxResourceVisualization_EXPORT GraphicalGeometricBase
{
public:
	GraphicalGeometricBase(vtkPolyDataAlgorithmPtr source = vtkPolyDataAlgorithmPtr(),
						   vtkRendererPtr renderer = vtkRendererPtr());
	~GraphicalGeometricBase();

	void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());
	void setSource(vtkPolyDataAlgorithmPtr source);
	void setBackfaceCulling(bool val);
	void setVisibility(bool visible);
	void setFrontfaceCulling(bool val);
	//    void setRepresentation();
	void setColor(double red, double green, double blue);
	void setColor(Vector3D color);
	void setPosition(Vector3D point);


	void setOpacity(double val);
	void setUserMatrix(vtkMatrix4x4 *matrix);
	void setPointSize(int pointSize);
	void setScalarVisibility(bool show);

	vtkActorPtr getActor();
	vtkPropertyPtr getProperty();
	vtkPolyDataPtr getPolyData();
	Vector3D getPosition() const;
	vtkPolyDataAlgorithmPtr getSource();

protected:
	vtkPolyDataAlgorithmPtr mSource;
	vtkPropertyPtr mProperty;
	vtkActorPtr mActor;
	vtkPolyDataPtr mData;
	vtkRendererPtr mRenderer;
	virtual vtkMapperPtr getMapper()=0;
};


/**
 * \addtogroup cx_resource_view
 * @{
 */

/** \brief Helper for rendering a a polydata in 3D
 */
class cxResourceVisualization_EXPORT GraphicalPolyData3D : public GraphicalGeometricBase
{
public:
	GraphicalPolyData3D(vtkPolyDataAlgorithmPtr source = vtkPolyDataAlgorithmPtr(),
						vtkRendererPtr renderer = vtkRendererPtr());
	void setIsWireFrame(bool val);
	//    void setRepresentation();
	void setData(vtkPolyDataPtr data);
	void setTexture(vtkTexturePtr texture);
	vtkMapperPtr getMapper();

private:
	vtkPolyDataMapperPtr mMapper;
};
typedef boost::shared_ptr<GraphicalPolyData3D> GraphicalPolyData3DPtr;


/** \brief Helper for rendering a a glyph in 3D
 */
class cxResourceVisualization_EXPORT GraphicalGlyph3DData : public GraphicalGeometricBase
{
public:
	GraphicalGlyph3DData(vtkPolyDataAlgorithmPtr source = vtkPolyDataAlgorithmPtr(),
						 vtkRendererPtr renderer = vtkRendererPtr());
	void setData(vtkPolyDataPtr data);
	void setOrientationArray(const char* orientationArray);
	void setColorArray(const char* colorArray);
	void setLUT(const char* lut);
	void setScaleFactor(double scaleFactor);
	vtkMapperPtr getMapper();

private:
	vtkGlyph3DMapperPtr mMapper;
};
typedef boost::shared_ptr<GraphicalGlyph3DData> GraphicalGlyph3DDataPtr;


/** \brief Helper for rendering a point in 3D
 */
class cxResourceVisualization_EXPORT GraphicalPoint3D
{
public:
	GraphicalPoint3D(vtkRendererPtr renderer = vtkRendererPtr());
	void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());
	~GraphicalPoint3D();
	void setRadius(double radius);
	//		void setColor(Vector3D color);
	void setColor(QColor color);
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
class cxResourceVisualization_EXPORT GraphicalLine3D
{
public:
	GraphicalLine3D(vtkRendererPtr renderer = vtkRendererPtr());
	void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());
	~GraphicalLine3D();
	void setColor(QColor color);
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
class cxResourceVisualization_EXPORT GraphicalArc3D
{
public:
	GraphicalArc3D(vtkRendererPtr renderer = vtkRendererPtr());
	~GraphicalArc3D();
	void setColor(QColor color);
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
class cxResourceVisualization_EXPORT GraphicalArrow3D
{
public:
	GraphicalArrow3D(vtkRendererPtr renderer = vtkRendererPtr());
	~GraphicalArrow3D();
	void setColor(QColor color);
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
class cxResourceVisualization_EXPORT Rect3D
{
public:
	Rect3D(vtkRendererPtr renderer, QColor color);
	~Rect3D();
	void setColor(QColor color);
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
class cxResourceVisualization_EXPORT FollowerText3D
{
public:
	FollowerText3D(vtkRendererPtr renderer = vtkRendererPtr());
	void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());

	~FollowerText3D();
	void setColor(QColor color);
	void setText(QString text);
	void setPosition(Vector3D pos);

	void setSize(double val);
	void setSizeInNormalizedViewport(bool on, double size);
	vtkFollowerPtr getActor();

	void scaleText(); ///< internal method

private:
	vtkVectorTextPtr mText;
	vtkFollowerPtr mFollower;
	vtkRendererPtr mRenderer;
	double mSize;

	ViewportListenerPtr mViewportListener;
};
typedef boost::shared_ptr<FollowerText3D> FollowerText3DPtr;

/** \brief Helper for rendering 3D text that faces the camera and
 *  has a constant viewed size, always on top.
 */
class cxResourceVisualization_EXPORT CaptionText3D
{
public:
	CaptionText3D(vtkRendererPtr renderer = vtkRendererPtr());
	void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());

	~CaptionText3D();
	void setColor(QColor color);
	void setText(QString text);
	void setPosition(Vector3D pos);

	void setSize(double val);
	//    void setSizeInNormalizedViewport(bool on, double size);
	vtkCaptionActor2DPtr getActor();

	//    void scaleText(); ///< internal method

	void placeBelowCenter();
	void placeAboveCenter();
	void setVisibility(bool visible);
	Vector3D getPosition() const;
private:
	vtkCaptionActor2DPtr mText;
	//    vtkFollowerPtr mFollower;
	vtkRendererPtr mRenderer;
	Vector3D mPos;

	//    ViewportListenerPtr mViewportListener;
};
typedef boost::shared_ptr<CaptionText3D> CaptionText3DPtr;

/**
 * @}
 */

}

#endif /*CXGRAPHICALPRIMITIVES_H_*/
