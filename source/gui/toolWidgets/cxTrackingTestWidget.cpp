/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTrackingTestWidget.h"

#include "cxToolManagerWidget.h"
#include "cxToolPositionRecorderWidget.h"
#include "cxTrackingComparisonWidget.h"

namespace cx
{

TrackingTestWidget::TrackingTestWidget(TrackingServicePtr trackingService, VideoServicePtr videoService, QWidget* parent) :
	TabbedWidget(parent, "tracking_test_widget", "Tracking Test")
{
	this->setToolTip("Test tracking: tracking state, record tool positions, compare tracking systems");
	this->addTab(new ToolManagerWidget(trackingService, this), "Tracking state");
	this->addTab(new ToolPositionRecorderWidget(trackingService, this), "Record positions");
	this->addTab(new TrackingComparisonWidget(trackingService, videoService, this), "Compare systems");
}

TrackingTestWidget::~TrackingTestWidget()
{
}

} // namespace cx
