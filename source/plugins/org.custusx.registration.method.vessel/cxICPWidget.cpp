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
#include "cxICPWidget.h"

#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <vtkCellArray.h>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxTimedAlgorithm.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxRegistrationProperties.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxMesh.h"
#include "cxView.h"
#include "cxGeometricRep.h"
#include "cxGraphicalPrimitives.h"
#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxPatientModelService.h"
#include "cxHelperWidgets.h"

namespace cx
{

ICPWidget::ICPWidget(QWidget* parent) :
	BaseWidget(parent, "ICPWidget", "ICPWidget")
{
	QHBoxLayout * buttonLayout = new QHBoxLayout;

	mRegisterButton = new QPushButton("Register");
	mRegisterButton->setEnabled(false);
	connect(mRegisterButton, &QPushButton::clicked, this, &ICPWidget::requestRegister);
	buttonLayout->addWidget(mRegisterButton, 1, 0);

	mMetricValue = new QLineEdit(this);
	mMetricValue->setReadOnly(true);
	mMetricValue->setToolTip("Current RMS deviation between data sets");
	buttonLayout->addWidget(mMetricValue);

	mVesselRegOptionsButton = this->createAction2(this,
		QIcon(":/icons/open_icon_library/system-run-5.png"),
		"Options",
		"Options for controlling ICP algorithm",
		buttonLayout);
	mVesselRegOptionsButton->setCheckable(true);


	mOptionsWidget = new QWidget(this);
	mVesselRegOptionsWidget = this->wrapInGroupBox(mOptionsWidget, "ICP options");
	connect(mVesselRegOptionsButton, &QAction::toggled, mVesselRegOptionsWidget, &QWidget::setVisible);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addLayout(buttonLayout);
	layout->addWidget(mVesselRegOptionsWidget);

	this->enableRegistration(false);
}

ICPWidget::~ICPWidget()
{
}

void ICPWidget::setSettings(std::vector<PropertyPtr> properties)
{
	QGridLayout* layout = new QGridLayout(mOptionsWidget);

	int row=0;
	for (unsigned i=0; i< properties.size(); ++i)
		sscCreateDataWidget(this, properties[i], layout, i);
}

void ICPWidget::enableRegistration(bool on)
{
	mRegisterButton->setEnabled(on);
	mVesselRegOptionsButton->setEnabled(true);
	mVesselRegOptionsWidget->setVisible(mVesselRegOptionsButton->isChecked());
//	mVesselRegOptionsButton->setEnabled(on);
//	mVesselRegOptionsWidget->setVisible(mVesselRegOptionsButton->isChecked() && on);
}

void ICPWidget::setRMS(double val)
{
	mMetricValue->setText(QString("%1mm").arg(val, 0, 'f', 3));
}

}//namespace cx
