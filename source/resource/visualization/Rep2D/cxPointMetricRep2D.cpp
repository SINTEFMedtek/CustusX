// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.


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
#include "cxLogger.h"

namespace cx
{

PointMetricRep2DPtr PointMetricRep2D::New(const QString& uid, const QString& name)
{
	PointMetricRep2DPtr retval(new PointMetricRep2D(uid, name));
	return retval;
}

PointMetricRep2D::PointMetricRep2D(const QString& uid, const QString& name) :
	DataMetricRep(uid, name)
{	
}

void PointMetricRep2D::setDynamicSize(bool on)
{
	if (on)
	{
		mViewportListener.reset(new ViewportListener);
		mViewportListener->setCallback(boost::bind(&PointMetricRep2D::rescale, this));
	}
	else
	{
		mViewportListener.reset();
	}
}

void PointMetricRep2D::addRepActorsToViewRenderer(View* view)
{
    if (mViewportListener)
        mViewportListener->startListen(view->getRenderer());
    DataMetricRep::addRepActorsToViewRenderer(view);
}

void PointMetricRep2D::removeRepActorsFromViewRenderer(View* view)
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

	if (!mDisk && mView && mMetric && mSliceProxy)
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

	mDisk->setRadius(this->findDiskRadius(position));

	Vector3D projectedPosition = position;
	double offsetFromXYPlane = 0.01;
	projectedPosition[2] = offsetFromXYPlane;
	mDisk->setPosition(projectedPosition);

	mDisk->update();
}

double PointMetricRep2D::findDiskRadius(Vector3D position)
{
	double radius = mGraphicsSize;
	if (mViewportListener)
	{
		double size = mViewportListener->getVpnZoom();
		radius = mGraphicsSize / 100 / size * 2.5;
	}

//	double radius = mGraphicsSize;
	if (abs(position[2]) > radius)
	{
		radius = 0;
	}
	else
	{
		radius = radius*cos(asin(position[2]/radius));
	}

	return radius;
}

void PointMetricRep2D::rescale()
{
	this->setModified();
}

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
