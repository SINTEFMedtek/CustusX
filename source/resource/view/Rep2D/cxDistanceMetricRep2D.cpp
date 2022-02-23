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
	std::vector<Vector3D> points_r = distanceMetric->getEndpoints();

	Transform3D vpMs = getView()->get_vpMs();
	Transform3D sMr = mSliceProxy->get_sMr();
	Vector3D p1_vp = vpMs * sMr * points_r[0];
	Vector3D p2_vp = vpMs * sMr * points_r[1];


	mLine->updatePosition(p1_vp, p2_vp);
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
