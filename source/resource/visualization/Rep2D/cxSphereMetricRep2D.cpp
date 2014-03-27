// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

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
#include "cxLogger.h"
#include "cxSphereMetric.h"

namespace cx
{

SphereMetricRep2DPtr SphereMetricRep2D::New(const QString& uid, const QString& name)
{
	SphereMetricRep2DPtr retval(new SphereMetricRep2D(uid, name));
	return retval;
}

SphereMetricRep2D::SphereMetricRep2D(const QString& uid, const QString& name) :
	DataMetricRep(uid, name)
{
}

void SphereMetricRep2D::addRepActorsToViewRenderer(View* view)
{
	DataMetricRep::addRepActorsToViewRenderer(view);
}

void SphereMetricRep2D::removeRepActorsFromViewRenderer(View* view)
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

	if (!mDisk && mView && metric && mSliceProxy)
	{
		mDisk.reset(new GraphicalDisk());
		mDisk->setRenderer(this->getRenderer());
	}

	if (!mDisk)
		return;

	Vector3D position = mSliceProxy->get_sMr() * mMetric->getRefCoord();

	mDisk->setColor(mMetric->getColor());
	mDisk->setOutlineColor(mMetric->getColor());
	mDisk->setOutlineWidth(0.1);
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


