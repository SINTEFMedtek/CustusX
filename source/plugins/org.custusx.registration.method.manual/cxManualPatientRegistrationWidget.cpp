/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxManualPatientRegistrationWidget.h"

#include <QLabel>
#include "cxPatientModelService.h"
#include "cxRegistrationService.h"
#include "cxTransform3DWidget.h"

namespace cx
{

ManualPatientRegistrationWidget::ManualPatientRegistrationWidget(RegServicesPtr services, QWidget* parent, QString objectName) :
				BaseWidget(parent, objectName, "Manual Patient Registration"),
				mVerticalLayout(new QVBoxLayout(this)),
				mServices(services)
{
	this->setToolTip("Set patient registration directly");
	mVerticalLayout->setMargin(0);
	mLabel = new QLabel("Patient Registration matrix rMpr");
	mVerticalLayout->addWidget(mLabel);
	mMatrixWidget = new Transform3DWidget(this);
	mVerticalLayout->addWidget(mMatrixWidget);
	connect(mMatrixWidget, SIGNAL(changed()), this, SLOT(matrixWidgetChanged()));
	connect(services->patient().get(), SIGNAL(rMprChanged()), this, SLOT(patientMatrixChanged()));
	mMatrixWidget->setEditable(true);

	mVerticalLayout->addStretch();

}


void ManualPatientRegistrationWidget::showEvent(QShowEvent* event)
{
	this->patientMatrixChanged();
}

///** Called when the matrix in the widget has changed.
// *  Perform registration.
// *
// */
void ManualPatientRegistrationWidget::matrixWidgetChanged()
{
	Transform3D rMpr = mMatrixWidget->getMatrix();
	mServices->registration()->addPatientRegistration(rMpr, "Manual Patient");
}

///** Called when the valid patient registration matrix in the system has changed.
// *  Perform registration.
// *
// */
void ManualPatientRegistrationWidget::patientMatrixChanged()
{
	mLabel->setText(this->getDescription());
	mMatrixWidget->blockSignals(true);
	mMatrixWidget->setMatrix(mServices->patient()->get_rMpr());
	mMatrixWidget->blockSignals(false);
}

bool ManualPatientRegistrationWidget::isValid() const
{
	return true;
}

QString ManualPatientRegistrationWidget::getDescription()
{
	if (this->isValid())
		return QString("<b>Patient Registration matrix rMpr</b>");
	else
		return "<Invalid matrix>";
}

} //cx
