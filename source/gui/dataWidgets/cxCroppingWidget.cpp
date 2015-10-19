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

#include "cxCroppingWidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <vtkImageData.h>
#include "cxStringPropertyBase.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxDefinitionStrings.h"
#include "cxUtilHelpers.h"

#include "cxRegistrationTransform.h"
#include "cxImageAlgorithms.h"
#include "cxInteractiveCropper.h"
#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxInteractiveCropper.h"

#include "cxViewGroupData.h"
#include "cxReporter.h"
#include "cxActiveData.h"

namespace cx
{

CroppingWidget::CroppingWidget(PatientModelServicePtr patientModelService, ViewServicePtr viewService, QWidget* parent) :
		BaseWidget(parent, "CroppingWidget", "Crop"),
		mPatientModelService(patientModelService),
		mViewService(viewService)
{
	connect(viewService.get(), &ViewService::activeLayoutChanged, this, &CroppingWidget::setupUI);
	this->setupUI();
}

void CroppingWidget::setupUI()
{
	if (mInteractiveCropper)
		return;

  mInteractiveCropper = viewService()->getCropper();

  if (!mInteractiveCropper)
	  return;

  connect(mInteractiveCropper.get(), SIGNAL(changed()), this, SLOT(cropperChangedSlot()));

  QVBoxLayout* layout = new QVBoxLayout(this);

  this->setToolTip("Interactive volume cropping");

  QGroupBox* activeGroupBox = new QGroupBox("Interactive cropper");
  activeGroupBox->setToolTip(this->toolTip());
  layout->addWidget(activeGroupBox);
  QVBoxLayout* activeLayout = new QVBoxLayout(activeGroupBox);

  mUseCropperCheckBox = new QCheckBox("Use Cropper");
  mUseCropperCheckBox->setToolTip("Turn on cropping for the active volume.");
  connect(mUseCropperCheckBox, SIGNAL(toggled(bool)), mInteractiveCropper.get(), SLOT(useCropping(bool)));
  activeLayout->addWidget(mUseCropperCheckBox);

  mShowBoxCheckBox = new QCheckBox("Show box");
  mShowBoxCheckBox->setToolTip("Show crop box in 3D view. This also turns on cropping for convenience.");
  connect(mShowBoxCheckBox, SIGNAL(toggled(bool)), mInteractiveCropper.get(), SLOT(showBoxWidget(bool)));
  activeLayout->addWidget(mShowBoxCheckBox);

  mBoundingBoxDimensions = new QLabel("?, ?, ?");
  mBoundingBoxDimensions->setToolTip("The dimensions of the croppers boundingbox.");
  activeLayout->addWidget(mBoundingBoxDimensions);

  mBBWidget = new BoundingBoxWidget(this);
  layout->addWidget(mBBWidget);
  connect(mBBWidget, SIGNAL(changed()), this, SLOT(boxValuesChanged()));

  QPushButton* cropClipButton = new QPushButton("Create new cropped volume");
  cropClipButton->setToolTip("Create a new volume containing only the volume inside the crop box.");
  connect(cropClipButton, SIGNAL(clicked()), this, SLOT(cropClipButtonClickedSlot()));
  layout->addWidget(cropClipButton);

  layout->addStretch();

  this->cropperChangedSlot();
}

void CroppingWidget::boxValuesChanged()
{
  mInteractiveCropper->setBoundingBox(mBBWidget->getValue());
}

void CroppingWidget::cropperChangedSlot()
{
	std::vector<int> dims = mInteractiveCropper->getDimensions();
	if(dims.size() < 3)
		return;

	QString dimensionText = "Dimensions: "+qstring_cast(dims.at(0))+", "+qstring_cast(dims.at(1))+", "+qstring_cast(dims.at(2));
	mBoundingBoxDimensions->setText(dimensionText);
	mUseCropperCheckBox->setChecked(mInteractiveCropper->getUseCropping());
	mShowBoxCheckBox->setChecked(mInteractiveCropper->getShowBoxWidget());

	mBBWidget->setValue(mInteractiveCropper->getBoundingBox(), mInteractiveCropper->getMaxBoundingBox());
}

ImagePtr CroppingWidget::cropClipButtonClickedSlot()
{
	ActiveDataPtr activeData = mPatientModelService->getActiveData();
	ImagePtr image = activeData->getActive<Image>();

	ImagePtr retval = cropImage(patientService(), image);
	mPatientModelService->insertData(retval);

	this->hideOldAndShowNewVolume(image, retval);

	return retval;
}

void CroppingWidget::hideOldAndShowNewVolume(ImagePtr oldImage, ImagePtr newImage)
{
//	int groupNr = mViewService->getActiveGroup();//Gives -1 when we are inside the CroppingWidget
	int groupNr = 0;

	ViewGroupDataPtr viewGroup = mViewService->getGroup(0);
	if(!viewGroup)
	{
		reportWarning(QString("CroppingWidget: Hide old and show new volume failed. Can't get view group %1.").arg(groupNr));
		return;
	}
	if(!viewGroup->removeData(oldImage->getUid()))
		reportWarning(QString("CroppingWidget: Hide old and show new volume failed. Can't remove image %1 from view group %2").arg(oldImage->getUid()).arg(groupNr));

	viewGroup->addData(newImage->getUid());
}

}
