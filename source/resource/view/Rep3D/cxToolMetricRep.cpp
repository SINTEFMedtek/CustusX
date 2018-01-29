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
