/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxClippingWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include "cxStringPropertyBase.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxInteractiveClipper.h"
#include "cxSelectDataStringProperty.h"
#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxVisServices.h"
#include "cxStringProperty.h"
#include "cxLogger.h"
#include "cxHelperWidgets.h"
#include "cxStringPropertyClipPlane.h"

namespace cx
{

ClippingWidget::ClippingWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "clipping_widget", "Clip"),
	mServices(services)
{
	connect(mServices->view().get(), &ViewService::activeLayoutChanged, this, &ClippingWidget::setupUI);
	this->setupUI();
}

void ClippingWidget::setupUI()
{
	if (mInteractiveClipper)
		return;

	mInteractiveClipper.reset(new InteractiveClipper(mServices));

	if (!mInteractiveClipper)
		return;

	connect(mInteractiveClipper.get(), SIGNAL(changed()), this, SLOT(clipperChangedSlot()));

	mDataAdapter = StringPropertySelectData::New(mServices->patient());
	LabeledComboBoxWidget* imageCombo = new LabeledComboBoxWidget(this, mDataAdapter);
	connect(mDataAdapter.get(), SIGNAL(changed()), this, SLOT(imageChangedSlot()));

	this->setToolTip("Interactive volume clipping");

	QVBoxLayout* layout = new QVBoxLayout(this);

	QGroupBox* activeClipGroupBox = new QGroupBox("Interactive clipper");
	activeClipGroupBox->setToolTip(this->toolTip());
	layout->addWidget(activeClipGroupBox);
	QVBoxLayout* activeClipLayout = new QVBoxLayout(activeClipGroupBox);

	mPlaneAdapter = StringPropertyClipPlane::New(mInteractiveClipper);
	LabeledComboBoxWidget* combo = new LabeledComboBoxWidget(this, mPlaneAdapter);

	mUseClipperCheckBox = new QCheckBox("Use Clipper");
	mUseClipperCheckBox->setToolTip("Turn on interactive clipping for the selected volume.");
	connect(mUseClipperCheckBox, SIGNAL(toggled(bool)), mInteractiveClipper.get(), SLOT(useClipper(bool)));
	activeClipLayout->addWidget(mUseClipperCheckBox);
	activeClipLayout->addWidget(imageCombo);
	activeClipLayout->addWidget(combo);
	mInvertPlaneCheckBox = new QCheckBox("Invert plane");
	mInvertPlaneCheckBox->setToolTip("Use the inverse (mirror) of the selected slice plane.");
	connect(mInvertPlaneCheckBox, SIGNAL(toggled(bool)), mInteractiveClipper.get(), SLOT(invertPlane(bool)));
	activeClipLayout->addWidget(mInvertPlaneCheckBox);

	QPushButton* saveButton = new QPushButton("Save clip plane");
	saveButton->setToolTip("Save the interactive plane as a clip plane in the selected volume.");
	connect(saveButton, SIGNAL(clicked()), this, SLOT(saveButtonClickedSlot()));
	//saveButton->setEnabled(false);
	QPushButton* clearButton = new QPushButton("Clear saved planes");
	clearButton->setToolTip("Remove all saved clip planes from the selected volume");
	connect(clearButton, SIGNAL(clicked()), this, SLOT(clearButtonClickedSlot()));
	//clearButton->setEnabled(false);
	activeClipLayout->addWidget(saveButton);
	layout->addWidget(clearButton);

	layout->addStretch();

	this->clipperChangedSlot();
}

void ClippingWidget::clipperChangedSlot()
{
	mUseClipperCheckBox->setChecked(mInteractiveClipper->getUseClipper());
	mInvertPlaneCheckBox->setChecked(mInteractiveClipper->getInvertPlane());
	if (mInteractiveClipper->getData())
		mDataAdapter->setValue(mInteractiveClipper->getData()->getUid());
}

void ClippingWidget::imageChangedSlot()
{
	mInteractiveClipper->setData(mServices->patient()->getData(mDataAdapter->getValue()));
}

void ClippingWidget::clearButtonClickedSlot()
{
	mInteractiveClipper->clearClipPlanesInVolume();
}

void ClippingWidget::saveButtonClickedSlot()
{
	mInteractiveClipper->saveClipPlaneToVolume();
}

}
