/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxStreamPropertiesWidget.h"

#include "cxSelectDataStringProperty.h"
#include "cxDataSelectWidget.h"
#include "cxTrackedStream.h"
#include "cxTransferFunctionWidget.h"
#include "cxShadingWidget.h"
#include "cxActiveData.h"
#include "cxPatientModelService.h"

namespace cx
{

StreamPropertiesWidget::StreamPropertiesWidget(PatientModelServicePtr patientModelService, ViewServicePtr viewService, QWidget *parent) :
	TabbedWidget(parent, "StreamPropertiesWidget", "Stream Properties"),
	mSelectStream(StringPropertySelectTrackedStream::New(patientModelService))
{
	this->setToolTip("Set properties on a tracked 2D/3D stream");
	bool connectToActiveImage = false;
	ActiveDataPtr activeData = patientModelService->getActiveData();
	mTransferFunctionWidget = TransferFunction3DWidgetPtr(new TransferFunction3DWidget(activeData, this, connectToActiveImage));
	mShadingWidget = ShadingWidgetPtr(new ShadingWidget(activeData, this, connectToActiveImage));

	this->insertWidgetAtTop(new DataSelectWidget(viewService, patientModelService, this, mSelectStream));
	this->addTab(mTransferFunctionWidget.get(), QString("Transfer Functions"));
	this->addTab(mShadingWidget.get(), "Shading");

	connect(mSelectStream.get(), &Property::changed, this, &StreamPropertiesWidget::streamSelectedSlot);
}

void StreamPropertiesWidget::streamSelectedSlot()
{
	if(mTrackedStream)
		disconnect(mTrackedStream.get(), &TrackedStream::streaming, this, &StreamPropertiesWidget::streamingSlot);

	mTrackedStream = mSelectStream->getTrackedStream();

	if(mTrackedStream)
	{
		connect(mTrackedStream.get(), &TrackedStream::streaming, this, &StreamPropertiesWidget::streamingSlot);
		streamingSlot(true);
	}
	else
		streamingSlot(false);
}

void StreamPropertiesWidget::streamingSlot(bool isStreaming)
{
	ImagePtr image = ImagePtr();
	mTransferFunctionWidget->imageChangedSlot(image);
	mShadingWidget->imageChangedSlot(image);

	if(isStreaming && mTrackedStream)
		connect(mTrackedStream.get(), &TrackedStream::newFrame, this, &StreamPropertiesWidget::firstFrame);
}

void StreamPropertiesWidget::firstFrame()
{
	ImagePtr image = mTrackedStream->getChangingImage();
	if(image)
	{
		disconnect(mTrackedStream.get(), &TrackedStream::newFrame, this, &StreamPropertiesWidget::firstFrame);
		mTransferFunctionWidget->imageChangedSlot(image);
		mShadingWidget->imageChangedSlot(image);
	}
}

} //cx
