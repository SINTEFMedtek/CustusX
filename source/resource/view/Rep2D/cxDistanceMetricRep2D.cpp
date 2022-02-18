/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/



#include "cxDistanceMetricRep2D.h"
#include "boost/bind.hpp"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSectorSource.h>

#include "cxSliceProxy.h"
#include "cxView.h"
#include "cxVtkHelperClasses.h"
#include "cxLogger.h"


namespace cx
{

DistanceMetricRep2DPtr DistanceMetricRep2D::New(const QString& uid)
{
	return wrap_new(new DistanceMetricRep2D(), uid);
}

DistanceMetricRep2D::DistanceMetricRep2D()
{	
}

void DistanceMetricRep2D::setDynamicSize(bool on)
{
	if (on)
	{
		mViewportListener.reset(new ViewportListener);
	}
	else
	{
		mViewportListener.reset();
	}
}

void DistanceMetricRep2D::addRepActorsToViewRenderer(ViewPtr view)
{
    if (mViewportListener)
        mViewportListener->startListen(view->getRenderer());
    DataMetricRep::addRepActorsToViewRenderer(view);
}

void DistanceMetricRep2D::removeRepActorsFromViewRenderer(ViewPtr view)
{
	mLine.reset();

	if (mViewportListener)
		mViewportListener->stopListen();
    DataMetricRep::removeRepActorsFromViewRenderer(view);
}

void DistanceMetricRep2D::clear()
{
    DataMetricRep::clear();
}

void DistanceMetricRep2D::onModifiedStartRender()
{
	if (!mMetric)
		return;

	if (!mLine && this->getView() && mMetric && mSliceProxy)
	{
		mLine.reset(new LineSegment(this->getRenderer()));
		mLine->setPoints(Vector3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 0.0), mMetric->getColor());
	}

	if (!mLine)
		return;

	Vector3D position = mSliceProxy->get_sMr() * mMetric->getRefCoord();


	mLine->setWidth(2);

	mLine->setColor(mMetric->getColor());
	DistanceMetricPtr distanceMetric = boost::dynamic_pointer_cast<DistanceMetric>(mMetric);
	if(!distanceMetric)
	{
		CX_LOG_WARNING() << "DistanceMetricRep2D: Got no DistanceMetric";
		return;
	}
	std::vector<Vector3D> points = distanceMetric->getEndpoints();
	//CX_LOG_DEBUG()<< "points: " << points[0] << " " << points[1];

	//TODO: Need to convert positions to 2D coordinates
	Vector3D point1 = mSliceProxy->get_sMr() * points[0];
	Vector3D point2 = mSliceProxy->get_sMr() * points[1];
	//double offsetFromXYPlane = 0.01;
	//point1[2] = offsetFromXYPlane;
	//point2[2] = offsetFromXYPlane;

//	static bool printed = false;
//	if(!printed)
//	{
//		printed = true;
//		CX_LOG_DEBUG() << "point1 ref coord: " << points[0];
//		CX_LOG_DEBUG() << "point1: " << point1;
//		CX_LOG_DEBUG() << "point2: " << point2;
//	}



	mLine->updatePosition(point1, point2);
	//mLine->updatePosition(Vector3D(0.0, 0.0, 0.0), Vector3D(100.0, 100.0, 0.0));
}

void DistanceMetricRep2D::setSliceProxy(SliceProxyPtr sliceProxy)
{
	if (mSliceProxy)
		disconnect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(setModified()));
	mSliceProxy = sliceProxy;
	if (mSliceProxy)
		connect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(setModified()));
	this->setModified();
}

}
