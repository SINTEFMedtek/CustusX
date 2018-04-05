/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxToolMetricRep.h"

#include "cxView.h"
#include "boost/bind.hpp"
#include "cxGraphicalAxes3D.h"

namespace cx
{

ToolMetricRepPtr ToolMetricRep::New(const QString& uid)
{
	return wrap_new(new ToolMetricRep(), uid);
}

ToolMetricRep::ToolMetricRep()
{
	mViewportListener.reset(new ViewportListener);
	mViewportListener->setCallback(boost::bind(&ToolMetricRep::rescale, this));
}

void ToolMetricRep::clear()
{
	mToolTip.reset();
	mToolOffset.reset();

	mAxes.reset();
	DataMetricRep::clear();
}

void ToolMetricRep::addRepActorsToViewRenderer(ViewPtr view)
{
	mViewportListener->startListen(view->getRenderer());
	DataMetricRep::addRepActorsToViewRenderer(view);
}

void ToolMetricRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
	DataMetricRep::removeRepActorsFromViewRenderer(view);
	mViewportListener->stopListen();
}

ToolMetricPtr ToolMetricRep::getToolMetric()
{
	return boost::dynamic_pointer_cast<ToolMetric>(mMetric);
}

void ToolMetricRep::onModifiedStartRender()
{
	ToolMetricPtr metric = this->getToolMetric();

	if (!metric || !metric->isValid() || !this->getView())
		return;

	if (!mAxes || !mToolTip || !mToolOffset)
	{
		mAxes.reset(new GraphicalAxes3D());
		mToolTip.reset(new GraphicalPoint3D(this->getRenderer()));
		mToolOffset.reset(new GraphicalLine3D(this->getRenderer()));
	}

	mAxes->setFontSize(0.04);
	mAxes->setAxisLength(0.05);
//	mAxes->setAxisLength(0.2);
	mAxes->setShowAxesLabels(false);
	mAxes->setRenderer(this->getRenderer());

	Vector3D p0_r = mMetric->getRefCoord();
	Transform3D rMt = metric->getRefFrame();
	Vector3D toolTip_r = rMt.coord(Vector3D(0,0,-metric->getToolOffset()));

	mToolTip->setValue(toolTip_r);
	mToolTip->setColor(mMetric->getColor());

	mToolOffset->setValue(p0_r, toolTip_r);
	mToolOffset->setColor(mMetric->getColor());

	mAxes->setTransform(metric->getRefFrame());
	this->drawText();
	this->rescale();
}

void ToolMetricRep::rescale()
{
	if (!mToolTip)
		return;

	double size = mViewportListener->getVpnZoom(this->getToolMetric()->getCoordinate());
	double sphereSize = mGraphicsSize / 100 / size;
	mToolTip->setRadius(sphereSize);
}


} // namespace cx
