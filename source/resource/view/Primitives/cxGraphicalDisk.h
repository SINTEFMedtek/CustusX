/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXGRAPHICALDISK_H
#define CXGRAPHICALDISK_H

#include "cxResourceVisualizationExport.h"

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
 * \ingroup cx_resource_view
 * \date 2014-02-25
 * \author christiana
 */
class cxResourceVisualization_EXPORT GraphicalDisk
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
