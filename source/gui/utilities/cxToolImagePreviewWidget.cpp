/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxToolImagePreviewWidget.h"


#include "cxTrackerConfiguration.h"
#include "cxTrackingService.h"

namespace cx
{
ToolImagePreviewWidget::ToolImagePreviewWidget(TrackingServicePtr trackingService, QWidget* parent) :
	ImagePreviewWidget(parent),
	mTrackingService(trackingService)
{
	this->setToolTip("Preview of a Tool Image");
	ImagePreviewWidget::setSize(200, 200);
}

ToolImagePreviewWidget::~ToolImagePreviewWidget()
{}

void ToolImagePreviewWidget::previewFileSlot(const QString& absoluteFilePath)
{
	TrackerConfigurationPtr config = mTrackingService->getConfiguration();
	const QString imageAbsoluteFilePath = config->getTool(absoluteFilePath).mPictureFilename;
	ImagePreviewWidget::previewFileSlot(imageAbsoluteFilePath);
}
}//namespace cx
