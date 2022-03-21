/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFastOrientationRegistrationWidget.h"

#include <cmath>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>

#include "cxTrackingService.h"
#include "cxPatientModelService.h"
#include "cxRegistrationService.h"
#include "cxTrackingService.h"
#include "cxSettings.h"

namespace cx
{
FastOrientationRegistrationWidget::FastOrientationRegistrationWidget(RegServicesPtr services, QWidget* parent) :
	RegistrationBaseWidget(services, parent, "org_custusx_registration_method_fast_landmark_image_to_patient_orientation_widget", "Fast Orientation Registration"),
	mSetOrientationButton(new QPushButton("Define Orientation")),
	mInvertButton(new QCheckBox("Back face"))
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(mInvertButton);
	layout->addWidget(mSetOrientationButton);
	layout->addStretch();

	mSetOrientationButton->setToolTip("Orient the data to the patient using a tracked tool.");

	connect(settings(), &Settings::valueChangedFor, this, &FastOrientationRegistrationWidget::globalConfigurationFileChangedSlot);

	mActiveToolProxy =  ActiveToolProxy::New(services->tracking());
	connect(mActiveToolProxy.get(), SIGNAL(toolVisible(bool)), this, SLOT(enableToolSampleButtonSlot()));
	connect(mActiveToolProxy.get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(enableToolSampleButtonSlot()));
	this->enableToolSampleButtonSlot();

}

FastOrientationRegistrationWidget::~FastOrientationRegistrationWidget()
{}

void FastOrientationRegistrationWidget::globalConfigurationFileChangedSlot(QString key)
{
	if (key == "giveManualToolPhysicalProperties")
		this->enableToolSampleButtonSlot();
}

void FastOrientationRegistrationWidget::showEvent(QShowEvent* event)
{
	connect(mSetOrientationButton, SIGNAL(clicked()), this, SLOT(setOrientationSlot()));
}

void FastOrientationRegistrationWidget::hideEvent(QHideEvent* event)
{
	disconnect(mSetOrientationButton, SIGNAL(clicked()), this, SLOT(setOrientationSlot()));
}

void FastOrientationRegistrationWidget::setOrientationSlot()
{
	Transform3D prMt = mServices->tracking()->getActiveTool()->get_prMt();
	mServices->registration()->doFastRegistration_Orientation(this->get_tMtm(), prMt);
}

Transform3D FastOrientationRegistrationWidget::get_tMtm() const
{
	Transform3D tMtm;

	if (mInvertButton->isChecked())
	{
		tMtm = createTransformRotateY(M_PI) * createTransformRotateZ(-M_PI / 2);
	}
	else
	{
		tMtm = createTransformRotateY(M_PI) * createTransformRotateZ(M_PI / 2);
	}

	return tMtm;
}

void FastOrientationRegistrationWidget::enableToolSampleButtonSlot()
{
	ToolPtr tool = mServices->tracking()->getActiveTool();
	bool enabled = tool &&
			tool->getVisible() &&
			(!tool->hasType(Tool::TOOL_MANUAL) || settings()->value("giveManualToolPhysicalProperties").toBool()); // enable only for non-manual tools.

	mSetOrientationButton->setEnabled(enabled);
}

}//namespace cx
