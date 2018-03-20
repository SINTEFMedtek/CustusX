/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSphereMetricRep2D.h"

#include "boost/bind.hpp"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSectorSource.h>

#include "cxSliceProxy.h"
#include "cxView.h"
#include "cxGraphicalDisk.h"

#include "cxSphereMetric.h"

namespace cx
{

SphereMetricRep2DPtr SphereMetricRep2D::New(const QString& uid)
{
	return wrap_new(new SphereMetricRep2D(), uid);
}

SphereMetricRep2D::SphereMetricRep2D()
{
}

void SphereMetricRep2D::addRepActorsToViewRenderer(ViewPtr view)
{
	DataMetricRep::addRepActorsToViewRenderer(view);
}

void SphereMetricRep2D::removeRepActorsFromViewRenderer(ViewPtr view)
{
	mDisk.reset();
	DataMetricRep::removeRepActorsFromViewRenderer(view);
}

void SphereMetricRep2D::clear()
{
	DataMetricRep::clear();
}

SphereMetricPtr SphereMetricRep2D::getSphereMetric()
{
	return boost::dynamic_pointer_cast<SphereMetric>(mMetric);
}

void SphereMetricRep2D::onModifiedStartRender()
{
	SphereMetricPtr metric = this->getSphereMetric();

	if (!metric)
		return;

	if (!mDisk && this->getView() && metric && mSliceProxy)
	{
		mDisk.reset(new GraphicalDisk());
		mDisk->setRenderer(this->getRenderer());
	}

	if (!mDisk)
		return;

	Vector3D position = mSliceProxy->get_sMr() * mMetric->getRefCoord();

	mDisk->setColor(mMetric->getColor());
	mDisk->setOutlineColor(mMetric->getColor());
	mDisk->setOutlineWidth(0.31);
	mDisk->setFillVisible(false);

	mDisk->setRadiusBySlicingSphere(metric->getRadius(), position[2]);

	Vector3D projectedPosition = position;
	double offsetFromXYPlane = 0.01;
	projectedPosition[2] = offsetFromXYPlane;
	mDisk->setPosition(projectedPosition);

	mDisk->update();
}

void SphereMetricRep2D::setSliceProxy(SliceProxyPtr sliceProxy)
{
	if (mSliceProxy)
		disconnect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(setModified()));
	mSliceProxy = sliceProxy;
	if (mSliceProxy)
		connect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(setModified()));
	this->setModified();
}

} // namespace cx


