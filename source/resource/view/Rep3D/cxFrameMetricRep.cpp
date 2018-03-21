/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFrameMetricRep.h"

#include "cxView.h"
#include "boost/bind.hpp"
#include "cxGraphicalAxes3D.h"

namespace cx
{

FrameMetricRepPtr FrameMetricRep::New(const QString& uid)
{
	return wrap_new(new FrameMetricRep(), uid);
}

FrameMetricRep::FrameMetricRep()
{
}

void FrameMetricRep::clear()
{
    mAxes.reset();
    DataMetricRep::clear();
}

FrameMetricPtr FrameMetricRep::getFrameMetric()
{
    return boost::dynamic_pointer_cast<FrameMetric>(mMetric);
}

void FrameMetricRep::onModifiedStartRender()
{
    FrameMetricPtr metric = this->getFrameMetric();

	if (!metric || !metric->isValid() || !this->getView())
        return;

    if (!mAxes)
    {
        mAxes.reset(new GraphicalAxes3D());
        mAxes->setFontSize(0.04);
        mAxes->setAxisLength(0.05);
        mAxes->setShowAxesLabels(false);
		mAxes->setRenderer(this->getRenderer());
    }

    mAxes->setTransform(metric->getRefFrame());
    this->drawText();
}

} // namespace cx
