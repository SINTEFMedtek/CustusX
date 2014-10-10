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

#include <cxPatientOrientationWidget.h>

#include <cmath>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include "cxReporter.h"
#include "cxToolManager.h"

#include "cxLogicManager.h"

namespace cx
{
PatientOrientationWidget::PatientOrientationWidget(RegistrationServicePtr registrationService, PatientModelServicePtr patientModelService, QWidget* parent, QString objectName, QString windowTitle) :
	RegistrationBaseWidget(registrationService, parent, objectName, windowTitle),
	mPatientOrientationButton(new QPushButton("Patient Orientation")),
	mInvertButton(new QCheckBox("Back face")),
	mPatientModelService(patientModelService)
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mInvertButton);
  layout->addWidget(mPatientOrientationButton);
  layout->addStretch();

  mPatientOrientationButton->setToolTip(defaultWhatsThis());
  connect(mPatientOrientationButton, SIGNAL(clicked()), this, SLOT(setPatientOrientationSlot()));

  connect(patientModelService.get(), SIGNAL(debugModeChanged(bool)), this, SLOT(enableToolSampleButtonSlot()));

  mDominantToolProxy =  DominantToolProxy::New(trackingService());
  connect(mDominantToolProxy.get(), SIGNAL(toolVisible(bool)), this, SLOT(enableToolSampleButtonSlot()));
  connect(mDominantToolProxy.get(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(enableToolSampleButtonSlot()));
  this->enableToolSampleButtonSlot();

}

PatientOrientationWidget::~PatientOrientationWidget()
{}

QString PatientOrientationWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3>Set patient orientation using the navigation tool.</h3>"
			"<p><b>Prerequisite:</b> Image registration.</p>"
			"<p>Only orientation of the reference space is changed - data is not moved.</p>"
			"Align the Polaris tool so that the tools tip points towards the patients feet and the "
			"markers face the same way as the patients nose. Click the button. Verify that the "
			"figure in the upper left corner of the 3D view is correcly aligned.</p>"
			"<p><b>Tip:</b> If the patient is orientated with the nose down towards the table, try using <i>back face</i>.</p>"
			"</html>";
}

Transform3D PatientOrientationWidget::get_tMtm() const
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

void PatientOrientationWidget::setPatientOrientationSlot()
{
	Transform3D prMt = toolManager()->getDominantTool()->get_prMt();
	mRegistrationService->applyPatientOrientation(this->get_tMtm(), prMt);
}

void PatientOrientationWidget::enableToolSampleButtonSlot()
{
  ToolPtr tool = toolManager()->getDominantTool();
  bool enabled = false;
  enabled = tool &&
	  tool->getVisible() &&
	  (!tool->hasType(Tool::TOOL_MANUAL) || mPatientModelService->getDebugMode()); // enable only for non-manual tools. ignore this in debug mode.

  mPatientOrientationButton->setEnabled(enabled);
}

}//namespace cx
