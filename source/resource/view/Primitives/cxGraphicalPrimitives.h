/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
private:
	vtkCaptionActor2DPtr mText;
	//    vtkFollowerPtr mFollower;
	vtkRendererPtr mRenderer;

	//    ViewportListenerPtr mViewportListener;
};
typedef boost::shared_ptr<CaptionText3D> CaptionText3DPtr;

/**
 * @}
 */

}

#endif /*CXGRAPHICALPRIMITIVES_H_*/
