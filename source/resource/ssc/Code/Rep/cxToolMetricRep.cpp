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

#include "cxToolMetricRep.h"

#include "sscView.h"
#include "boost/bind.hpp"
#include "cxGraphicalAxes3D.h"

namespace cx
{

ToolMetricRepPtr ToolMetricRep::New(const QString& uid, const QString& name)
{
	ToolMetricRepPtr retval(new ToolMetricRep(uid, name));
	return retval;
}

ToolMetricRep::ToolMetricRep(const QString& uid, const QString& name) :
				DataMetricRep(uid, name)
{
	mViewportListener.reset(new ssc::ViewportListener);
	mViewportListener->setCallback(boost::bind(&ToolMetricRep::rescale, this));
}

void ToolMetricRep::clear()
{
	mToolTip.reset();
	mToolOffset.reset();

	mAxes.reset();
	DataMetricRep::clear();
}

void ToolMetricRep::addRepActorsToViewRenderer(ssc::View *view)
{
	mViewportListener->startListen(view->getRenderer());
	DataMetricRep::addRepActorsToViewRenderer(view);
}

void ToolMetricRep::removeRepActorsFromViewRenderer(ssc::View *view)
{
	DataMetricRep::removeRepActorsFromViewRenderer(view);
	mViewportListener->stopListen();
}

ToolMetricPtr ToolMetricRep::getToolMetric()
{
	return boost::dynamic_pointer_cast<ToolMetric>(mMetric);
}

void ToolMetricRep::changedSlot()
{
	ToolMetricPtr metric = this->getToolMetric();

	if (!metric || !metric->isValid() || !mView)
		return;

	if (!mAxes || !mToolTip || !mToolOffset)
	{
		mAxes.reset(new ssc::GraphicalAxes3D());
		mToolTip.reset(new ssc::GraphicalPoint3D(mView->getRenderer()));
		mToolOffset.reset(new ssc::GraphicalLine3D(mView->getRenderer()));
	}

	mAxes->setFontSize(0.04);
	mAxes->setAxisLength(0.05);
//	mAxes->setAxisLength(0.2);
	mAxes->setShowAxesLabels(false);
	mAxes->setRenderer(mView->getRenderer());

	ssc::Vector3D p0_r = mMetric->getRefCoord();
	ssc::Transform3D rMt = metric->getRefFrame();
	ssc::Vector3D toolTip_r = rMt.coord(ssc::Vector3D(0,0,-metric->getToolOffset()));

	mToolTip->setValue(toolTip_r);
	mToolTip->setRadius(mGraphicsSize);
	mToolTip->setColor(mColor);

	mToolOffset->setValue(p0_r, toolTip_r);
	mToolOffset->setColor(mColor);

	mAxes->setTransform(metric->getRefFrame());
	this->drawText();
}

void ToolMetricRep::rescale()
{
	if (!mToolTip)
		return;

	double size = mViewportListener->getVpnZoom();
	double sphereSize = mGraphicsSize / 100 / size;
	mToolTip->setRadius(sphereSize);
}


} // namespace ssc
