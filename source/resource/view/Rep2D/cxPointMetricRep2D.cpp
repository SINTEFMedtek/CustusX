/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/



#include "cxPointMetricRep2D.h"
#include "boost/bind.hpp"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSectorSource.h>

#include "cxSliceProxy.h"
#include "cxView.h"
#include "cxGraphicalDisk.h"


namespace cx
{

PointMetricRep2DPtr PointMetricRep2D::New(const QString& uid)
{
	return wrap_new(new PointMetricRep2D(), uid);
}

PointMetricRep2D::PointMetricRep2D()
{	
}

void PointMetricRep2D::setDynamicSize(bool on)
{
	if (on)
	{
		mViewportListener.reset(new ViewportListener);
//		mViewportListener->setCallback(boost::bind(&PointMetricRep2D::rescale, this));
	}
	else
	{
		mViewportListener.reset();
	}
}

void PointMetricRep2D::addRepActorsToViewRenderer(ViewPtr view)
{
    if (mViewportListener)
        mViewportListener->startListen(view->getRenderer());
    DataMetricRep::addRepActorsToViewRenderer(view);
}

void PointMetricRep2D::removeRepActorsFromViewRenderer(ViewPtr view)
{
	mDisk.reset();

	if (mViewportListener)
		mViewportListener->stopListen();
    DataMetricRep::removeRepActorsFromViewRenderer(view);
}

void PointMetricRep2D::clear()
{
    DataMetricRep::clear();
}

void PointMetricRep2D::onModifiedStartRender()
{
	if (!mMetric)
		return;

	if (!mDisk && this->getView() && mMetric && mSliceProxy)
	{
		mDisk.reset(new GraphicalDisk());
		mDisk->setRenderer(this->getRenderer());
	}

	if (!mDisk)
		return;

	Vector3D position = mSliceProxy->get_sMr() * mMetric->getRefCoord();

	mDisk->setColor(mMetric->getColor());
	mDisk->setOutlineColor(mMetric->getColor());
	mDisk->setOutlineWidth(0.25);
	mDisk->setFillVisible(false);

	mDisk->setRadiusBySlicingSphere(this->findSphereRadius(), position[2]);

	Vector3D projectedPosition = position;
	double offsetFromXYPlane = 0.01;
	projectedPosition[2] = offsetFromXYPlane;
	mDisk->setPosition(projectedPosition);

	mDisk->update();
}

double PointMetricRep2D::findSphereRadius()
{
	double radius = mGraphicsSize;
	if (mViewportListener)
	{
		double size = mViewportListener->getVpnZoom();
		radius = mGraphicsSize / 100 / size * 2.5;
	}

	return radius;
}

//void PointMetricRep2D::rescale()
//{
//	this->setModified();
//}

void PointMetricRep2D::setSliceProxy(SliceProxyPtr sliceProxy)
{
	if (mSliceProxy)
		disconnect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(setModified()));
	mSliceProxy = sliceProxy;
	if (mSliceProxy)
		connect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(setModified()));
	this->setModified();
}

}
