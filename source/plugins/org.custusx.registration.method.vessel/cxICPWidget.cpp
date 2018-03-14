/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
