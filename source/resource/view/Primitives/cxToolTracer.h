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


#ifndef CXTOOLTRACER_H_
#define CXTOOLTRACER_H_

#include "cxResourceVisualizationExport.h"

#include <QObject>
#include <QSize>
#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;
#include "cxTransform3D.h"

#include <boost/shared_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxTool.h"

class QColor;

namespace cx
{

typedef boost::shared_ptr<class ToolTracer> ToolTracerPtr;
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;

/** \brief 3D Graphics class for displaying the trace path traversed by a tool.
 *
 * ToolTracer is used internally by ToolRep3D as an option.
 *
 * Used by CustusX.
 *
 * \ingroup cx_resource_view
 */
class cxResourceVisualization_EXPORT ToolTracer : QObject
{
	Q_OBJECT
public:
	static ToolTracerPtr create(SpaceProviderPtr spaceProvider);
	void setTool(ToolPtr tool);
	vtkPolyDataPtr getPolyData();
	vtkActorPtr getActor();

	void setColor(QColor color);

	void start(); // start path tracking
	void stop(); // stop tracking
	void clear(); // erase stored tracking data.
	bool isRunning() const; // true if started and not stopped.
	void setMinDistance(double distance) { mMinDistance = distance; }
	int getSkippedPoints() { return mSkippedPoints; }
	void addManyPositions(TimedTransformMap trackerRecordedData_prMt);

private slots:
	void receiveTransforms(Transform3D prMt, double timestamp);
private:
	ToolTracer();
	void connectTool();
	void disconnectTool();

	bool mRunning;
	vtkPolyDataPtr mPolyData; ///< polydata representation of the probe, in space u
	vtkActorPtr mActor;
	ToolPtr mTool;
	vtkPolyDataMapperPtr mPolyDataMapper;
	vtkPropertyPtr mProperty;

	vtkPointsPtr mPoints;
	vtkCellArrayPtr mLines;

	bool mFirstPoint;
	int mSkippedPoints;
	Vector3D mPreviousPoint;
	double mMinDistance;

	SpaceProviderPtr mSpaceProvider;
};

} // namespace cx

#endif /*CXTOOLTRACER_H_*/
