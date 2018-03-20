/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
typedef boost::shared_ptr<class SpaceListener> SpaceListenerPtr;

/** \brief 3D Graphics class for displaying the trace path traversed by a tool.
 *
 * ToolTracer is used internally by ToolRep3D as an option.
 *
 * Used by CustusX.
 *
 * \ingroup cx_resource_view
 */
class cxResourceVisualization_EXPORT ToolTracer : public QObject
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
	ToolTracer(SpaceProviderPtr spaceProvider);
	void connectTool();
	void disconnectTool();
	void onSpaceChanged();

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
	SpaceListenerPtr mSpaceListener;
};

} // namespace cx

#endif /*CXTOOLTRACER_H_*/
