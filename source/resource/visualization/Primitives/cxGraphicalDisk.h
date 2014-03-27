// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXGRAPHICALDISK_H
#define CXGRAPHICALDISK_H

#include <boost/shared_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include "cxVector3D.h"
#include <QColor>
typedef vtkSmartPointer<class vtkLinearExtrusionFilter> vtkLinearExtrusionFilterPtr;

namespace cx
{

/** Render a flat disk in 2D/3D
 *
 *
 * \ingroup cx_resource_visualization
 * \date 2014-02-25
 * \author christiana
 */
class GraphicalDisk
{
public:
	GraphicalDisk();
	void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());
	~GraphicalDisk();
	void setRadius(double radius);
	void setOutlineColor(QColor color);
	void setOutlineWidth(double width);
	void setColor(QColor color);
	void setFillVisible(bool val);
	void setLighting(bool on);
	void setHeight(double height);

	void setPosition(Vector3D pos);
	void setDirection(Vector3D direction);

	void update();

	void setRadiusBySlicingSphere(double sphereRadius, double sliceHeight);

private:
	void createActors();
	void addActors();
	void removeActors();
	void updateOrientation();
	double getRadiusOfCircleSlicedFromSphere(double sphereRadius, double sliceHeight) const;

	vtkRendererPtr mRenderer;

	double mRadius;
	double mHeight;
	Vector3D mPosition;
	Vector3D mDirection;

	vtkActorPtr mCircleActor;
	vtkLinearExtrusionFilterPtr mCircleExtruder;
	vtkSectorSourcePtr mCircleSource;
	vtkActorPtr mOutlineActor;
	vtkLinearExtrusionFilterPtr mOutlineExtruder;
	vtkSectorSourcePtr mOutlineSource;
	double mOutlineWidth;
	QColor mOutlineColor;
	QColor mColor;
	bool mFillVisible;
	bool mUseLighting;
};
typedef boost::shared_ptr<GraphicalDisk> GraphicalDiskPtr;



} // namespace cx



#endif // CXGRAPHICALDISK_H
