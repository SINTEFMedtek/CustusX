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

#include "cxLandmarkPatientRegistrationWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QSlider>
#include <QGridLayout>
#include <QSpinBox>
#include <vtkDoubleArray.h>
#include "cxVector3D.h"
#include "cxLogger.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxLandmarkRep.h"
#include "cxView.h"
#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxPatientModelService.h"
#include "cxViewGroupData.h"
#include "cxReporter.h"
#include "cxLandmarkListener.h"

namespace cx
{

LandmarkPatientRegistrationWidget::LandmarkPatientRegistrationWidget(RegServices services, QWidget* parent, QString objectName, QString windowTitle) :
	LandmarkRegistrationWidget(services, parent, objectName, windowTitle)
{
	mLandmarkTableWidget->hide();

	mLandmarkListener->useI2IRegistration(false);

	mFixedProperty.reset(new StringPropertyRegistrationFixedImage(services.registrationService, services.patientModelService));
	connect(services.patientModelService.get(), &PatientModelService::rMprChanged, this, &LandmarkPatientRegistrationWidget::setModified);


	mRegisterButton = new QPushButton("Register", this);
	mRegisterButton->setToolTip("Perform registration");
	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));

	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mFixedProperty));

	QHBoxLayout* regLayout = new QHBoxLayout;
	regLayout->addWidget(mRegisterButton);
	mVerticalLayout->addWidget(mAvarageAccuracyLabel);
	mVerticalLayout->addLayout(regLayout);
	mVerticalLayout->addStretch();

}

void LandmarkPatientRegistrationWidget::registerSlot()
{
	this->performRegistration();
}

void LandmarkPatientRegistrationWidget::showEvent(QShowEvent* event)
{
//	std::cout << "LandmarkPatientRegistrationWidget::showEvent" << std::endl;
	mServices.visualizationService->getGroup(0)->setRegistrationMode(rsPATIENT_REGISTRATED);
	LandmarkRegistrationWidget::showEvent(event);
}

void LandmarkPatientRegistrationWidget::hideEvent(QHideEvent* event)
{
	mServices.visualizationService->getGroup(0)->setRegistrationMode(rsNOT_REGISTRATED);
	LandmarkRegistrationWidget::hideEvent(event);
}

//The following functions look (almost) exactly like the same functions in PatientLandMarksWidget
void LandmarkPatientRegistrationWidget::performRegistration()
{
	if (!mServices.registrationService->getFixedData())
		mServices.registrationService->setFixedData(mServices.patientModelService->getActiveData<Image>());

	if (mServices.patientModelService->getPatientLandmarks()->getLandmarks().size() < 3)
		return;

	mServices.registrationService->doPatientRegistration();

	this->updateAverageAccuracyLabel();
}

LandmarkMap LandmarkPatientRegistrationWidget::getTargetLandmarks() const
{
	return mServices.patientModelService->getPatientLandmarks()->getLandmarks();
}

Transform3D LandmarkPatientRegistrationWidget::getTargetTransform() const
{
	Transform3D rMpr = mServices.patientModelService->get_rMpr();
	return rMpr;
}

void LandmarkPatientRegistrationWidget::setTargetLandmark(QString uid, Vector3D p_target)
{
	mServices.patientModelService->getPatientLandmarks()->setLandmark(Landmark(uid, p_target));
	reporter()->playSampleSound();
}

QString LandmarkPatientRegistrationWidget::getTargetName() const
{
	return "Patient";
}



}//namespace cx
