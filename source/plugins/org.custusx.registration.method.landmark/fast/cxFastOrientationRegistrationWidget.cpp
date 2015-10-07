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
