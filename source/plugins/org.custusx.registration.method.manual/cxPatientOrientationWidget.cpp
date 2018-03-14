/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <cxPatientOrientationWidget.h>

#include <cmath>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>

#include "cxRegistrationService.h"
#include "cxPatientModelService.h"

#include "cxTrackingService.h"
#include "cxSettings.h"

namespace cx
{
PatientOrientationWidget::PatientOrientationWidget(RegServicesPtr services, QWidget* parent, QString objectName, QString windowTitle) :
	RegistrationBaseWidget(services, parent, objectName, windowTitle),
	mPatientOrientationButton(new QPushButton("Patient Orientation")),
	mInvertButton(new QCheckBox("Back face"))
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);
  layout->addWidget(mInvertButton);
  layout->addWidget(mPatientOrientationButton);
  layout->addStretch();

  this->setToolTip("Set patient orientation using the navigation tool");
  mPatientOrientationButton->setToolTip(this->toolTip());
  connect(mPatientOrientationButton, SIGNAL(clicked()), this, SLOT(setPatientOrientationSlot()));

  connect(settings(), &Settings::valueChangedFor, this, &PatientOrientationWidget::globalConfigurationFileChangedSlot);

  mActiveToolProxy =  ActiveToolProxy::New(services->tracking());
  connect(mActiveToolProxy.get(), SIGNAL(toolVisible(bool)), this, SLOT(enableToolSampleButtonSlot()));
  connect(mActiveToolProxy.get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(enableToolSampleButtonSlot()));
  this->enableToolSampleButtonSlot();
}

PatientOrientationWidget::~PatientOrientationWidget()
{}

void PatientOrientationWidget::globalConfigurationFileChangedSlot(QString key)
{
	if (key == "giveManualToolPhysicalProperties")
		this->enableToolSampleButtonSlot();
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
	Transform3D prMt = mServices->tracking()->getActiveTool()->get_prMt();
	mServices->registration()->applyPatientOrientation(this->get_tMtm(), prMt);
}

void PatientOrientationWidget::enableToolSampleButtonSlot()
{
  ToolPtr tool = mServices->tracking()->getActiveTool();
  bool enabled = tool &&
	  tool->getVisible() &&
	  (!tool->hasType(Tool::TOOL_MANUAL) || settings()->value("giveManualToolPhysicalProperties").toBool()); // enable only for non-manual tools.

  mPatientOrientationButton->setEnabled(enabled);
}

}//namespace cx
