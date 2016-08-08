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

#include "cxVolumeInfoWidget.h"

#include <QPushButton>
#include "cxImage.h"
#include "cxLabeledLineEditWidget.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxVolumeHelpers.h"
#include "cxActiveImageProxy.h"
#include "cxPatientModelService.h"
#include "cxActiveData.h"

namespace cx
{

VolumeInfoWidget::VolumeInfoWidget(PatientModelServicePtr patientModelService, QWidget* parent) :
  InfoWidget(parent, "volume_info_widget", "Volume Info"),
  mPatientModelService(patientModelService),
  mActiveData(patientModelService->getActiveData())
{
	this->setToolTip("Display volume info");
	mActiveImageProxy = ActiveImageProxy::New(mActiveData);
	connect(mActiveImageProxy.get(), &ActiveImageProxy::activeImageChanged, this, &VolumeInfoWidget::updateSlot);

	this->addWidgets();

	this->updateSlot();
}

VolumeInfoWidget::~VolumeInfoWidget()
{
	disconnect(mActiveImageProxy.get(), &ActiveImageProxy::activeImageChanged, this, &VolumeInfoWidget::updateSlot);
}

void VolumeInfoWidget::addWidgets()
{
	mParentFrameAdapter = StringPropertyParentFrame::New(mPatientModelService);
	mNameAdapter = StringPropertyDataNameEditable::New();
	mUidAdapter = StringPropertyDataUidEditable::New();
	mModalityAdapter = StringPropertyDataModality::New(mPatientModelService);
	mImageTypeAdapter = StringPropertyImageType::New(mPatientModelService);

	int i=0;
	new LabeledLineEditWidget(this, mUidAdapter, gridLayout, i++);
	new LabeledLineEditWidget(this, mNameAdapter, gridLayout, i++);
	new LabeledComboBoxWidget(this, mModalityAdapter, gridLayout, i++);
	new LabeledComboBoxWidget(this, mImageTypeAdapter, gridLayout, i++);
	new LabeledComboBoxWidget(this, mParentFrameAdapter, gridLayout, i++);

	gridLayout->addWidget(mTableWidget, i++, 0, 1, 2);
}

void VolumeInfoWidget::updateSlot()
{
	ImagePtr image = mActiveData->getActive<Image>();
	mParentFrameAdapter->setData(image);
	mNameAdapter->setData(image);
	mUidAdapter->setData(image);
	mModalityAdapter->setData(image);
	mImageTypeAdapter->setData(image);

	std::map<std::string, std::string> info = getDisplayFriendlyInfo(image);
	this->populateTableWidget(info);
}
}//namespace
