/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
		BaseWidget(parent, "cropping_widget", "Crop"),
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

  mInteractiveCropper = mViewService->getCropper();

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

  QGroupBox* autoCropGroupBox = new QGroupBox("Auto-crop");
  autoCropGroupBox->setToolTip("Check one or more, then press 'Auto-crop' to apply their combined result.");
  layout->addWidget(autoCropGroupBox);
  QVBoxLayout* autoCropLayout = new QVBoxLayout(autoCropGroupBox);

  mAutoCropBodyCheckBox = new QCheckBox("Crop away outside air");
  mAutoCropBodyCheckBox->setToolTip("Remove surrounding air/background. Works for both CT and MR.");
  mAutoCropBodyCheckBox->setChecked(true);
  autoCropLayout->addWidget(mAutoCropBodyCheckBox);

  mAutoCropTorsoLungBaseCheckBox = new QCheckBox("Crop away head/legs (lung-based, CT only)");
  mAutoCropTorsoLungBaseCheckBox->setToolTip("Trim to the torso/abdomen using the lung bases as a landmark.");
  autoCropLayout->addWidget(mAutoCropTorsoLungBaseCheckBox);

  mAutoCropTorsoCrossSectionCheckBox = new QCheckBox("Crop away head/legs (cross-section)");
  mAutoCropTorsoCrossSectionCheckBox->setToolTip("Trim to the torso's widest cross-section. Cheaper but less "
                             "precise than the lung-based option; works for both CT and MR.");
  autoCropLayout->addWidget(mAutoCropTorsoCrossSectionCheckBox);

  QPushButton* autoCropApplyButton = new QPushButton("Auto-crop");
  autoCropApplyButton->setToolTip("Set the crop box to the combined result of the checked option(s) above. Click "
                             "'Create new cropped volume' below to apply.");
  connect(autoCropApplyButton, SIGNAL(clicked()), this, SLOT(autoCropApplyButtonClickedSlot()));
  autoCropLayout->addWidget(autoCropApplyButton);

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

	ImagePtr retval = cropImage(mPatientModelService, image);
	mPatientModelService->insertData(retval);

	this->hideOldAndShowNewVolume(image, retval);

	return retval;
}

void CroppingWidget::autoCropApplyButtonClickedSlot()
{
	ActiveDataPtr activeData = mPatientModelService->getActiveData();
	ImagePtr image = activeData->getActive<Image>();
	if (!image)
		return;

	// Combine (intersect) whichever options are checked, rather than each
	// one overwriting the others' result - e.g. "away outside air" and "away
	// head/legs" narrow different axes/ranges of the same box, so applying
	// both together (not just one after the other) gives the full effect.
	DoubleBoundingBox3D box = image->boundingBox();
	if (mAutoCropBodyCheckBox->isChecked())
		box = intersection(box, computeAutoCropBox(image));
	if (mAutoCropTorsoLungBaseCheckBox->isChecked())
		box = intersection(box, computeLungBaseCropBox(image));
	if (mAutoCropTorsoCrossSectionCheckBox->isChecked())
		box = intersection(box, computeWidestCrossSectionCropBox(image));

	mInteractiveCropper->setBoundingBox(box);
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
