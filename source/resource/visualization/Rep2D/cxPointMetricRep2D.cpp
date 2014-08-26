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
