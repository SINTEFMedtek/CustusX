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
