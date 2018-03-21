/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
