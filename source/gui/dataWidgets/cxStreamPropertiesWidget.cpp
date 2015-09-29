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

StreamPropertiesWidget::StreamPropertiesWidget(PatientModelServicePtr patientModelService, VisualizationServicePtr visualizationService, QWidget *parent) :
	TabbedWidget(parent, "StreamPropertiesWidget", "Stream Properties"),
	mSelectStream(StringPropertySelectTrackedStream::New(patientModelService))
{
	this->setToolTip("Set properties on a tracked 2D/3D stream");
	bool connectToActiveImage = false;
	ActiveDataPtr activeData = patientModelService->getActiveData();
	mTransferFunctionWidget = TransferFunction3DWidgetPtr(new TransferFunction3DWidget(activeData, this, connectToActiveImage));
	mShadingWidget = ShadingWidgetPtr(new ShadingWidget(activeData, this, connectToActiveImage));

	this->insertWidgetAtTop(new DataSelectWidget(visualizationService, patientModelService, this, mSelectStream));
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
