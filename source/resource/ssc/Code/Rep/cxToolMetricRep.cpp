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
}

void ToolMetricRep::clear()
{
	mAxes.reset();
	DataMetricRep::clear();
}

//void ToolMetricRep::addRepActorsToViewRenderer(ssc::View *view)
//{
//    mAxes->setRenderer(view->getRenderer());
//    DataMetricRep::addRepActorsToViewRenderer(view);
//}

//void ToolMetricRep::removeRepActorsFromViewRenderer(ssc::View *view)
//{
//    mAxes->setRenderer(NULL);
//    DataMetricRep::removeRepActorsFromViewRenderer(view);
//}

ToolMetricPtr ToolMetricRep::getToolMetric()
{
	return boost::dynamic_pointer_cast<ToolMetric>(mMetric);
}

void ToolMetricRep::changedSlot()
{
	ToolMetricPtr metric = this->getToolMetric();

	if (!metric || !metric->isValid() || !mView)
		return;

	if (!mAxes)
	{
		mAxes.reset(new ssc::GraphicalAxes3D());
		mAxes->setFontSize(0.04);
		mAxes->setAxisLength(0.05);
		mAxes->setShowAxesLabels(false);
		mAxes->setRenderer(mView->getRenderer());
	}

	mAxes->setTransform(metric->getRefFrame());
	this->drawText();
}

} // namespace ssc
