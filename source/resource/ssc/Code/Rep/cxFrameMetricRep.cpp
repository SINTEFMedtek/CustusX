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

#include "cxFrameMetricRep.h"

#include "sscView.h"
#include "boost/bind.hpp"
#include "cxGraphicalAxes3D.h"

namespace cx
{

FrameMetricRepPtr FrameMetricRep::New(const QString& uid, const QString& name)
{
	FrameMetricRepPtr retval(new FrameMetricRep(uid, name));
	return retval;
}

FrameMetricRep::FrameMetricRep(const QString& uid, const QString& name) :
                DataMetricRep(uid, name)
{
}

void FrameMetricRep::clear()
{
    mAxes.reset();
    DataMetricRep::clear();
}

//void FrameMetricRep::addRepActorsToViewRenderer(ssc::View *view)
//{
//    mAxes->setRenderer(view->getRenderer());
//    DataMetricRep::addRepActorsToViewRenderer(view);
//}

//void FrameMetricRep::removeRepActorsFromViewRenderer(ssc::View *view)
//{
//    mAxes->setRenderer(NULL);
//    DataMetricRep::removeRepActorsFromViewRenderer(view);
//}

FrameMetricPtr FrameMetricRep::getFrameMetric()
{
    return boost::dynamic_pointer_cast<FrameMetric>(mMetric);
}

void FrameMetricRep::changedSlot()
{
    FrameMetricPtr metric = this->getFrameMetric();

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
