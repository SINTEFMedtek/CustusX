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

#include "cxLegacySingletons.h"

namespace cx
{
FastOrientationRegistrationWidget::FastOrientationRegistrationWidget(RegServices services, QWidget* parent) :
	RegistrationBaseWidget(services, parent, "FastOrientationRegistrationWidget", "Fast Orientation Registration"),
	mSetOrientationButton(new QPushButton("Define Orientation")),
    mInvertButton(new QCheckBox("Back face"))
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mInvertButton);
  layout->addWidget(mSetOrientationButton);
  layout->addStretch();

  mSetOrientationButton->setToolTip(this->defaultWhatsThis());

  connect(services.patientModelService.get(), &PatientModelService::debugModeChanged,
		  this, &FastOrientationRegistrationWidget::enableToolSampleButtonSlot);

  mActiveToolProxy =  ActiveToolProxy::New(trackingService());
  connect(mActiveToolProxy.get(), SIGNAL(toolVisible(bool)), this, SLOT(enableToolSampleButtonSlot()));
  connect(mActiveToolProxy.get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(enableToolSampleButtonSlot()));
  this->enableToolSampleButtonSlot();

}

FastOrientationRegistrationWidget::~FastOrientationRegistrationWidget()
{}

QString FastOrientationRegistrationWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3>Fast orientation registration.</h3>"
			"<p><b>Prerequisite:</b> Correctly oriented DICOM axes.</p>"
			"<p>Fast and approximate method for orienting the data to the patient.</p>"
			"<p><i>"
			"Align the Polaris tool so that the tools tip points towards the patients feet and the "
			"markers face the same way as the patients nose. Click the Get Orientation button."
			"</i></p>"
			"<p>"
			"<b>Tip:</b> If the patient is orientated with the nose down towards the table, try using <i>back face</i>."
			"</p>"
			"</html>";
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
	Transform3D prMt = mServices.trackingService->getActiveTool()->get_prMt();
	mServices.registrationService->doFastRegistration_Orientation(this->get_tMtm(), prMt);
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
  ToolPtr tool = mServices.trackingService->getActiveTool();
  bool enabled = false;
  enabled = tool &&
	  tool->getVisible() &&
	  (!tool->hasType(Tool::TOOL_MANUAL) || mServices.patientModelService->getDebugMode()); // enable only for non-manual tools. ignore this in debug mode.

  mSetOrientationButton->setEnabled(enabled);
}

}//namespace cx
