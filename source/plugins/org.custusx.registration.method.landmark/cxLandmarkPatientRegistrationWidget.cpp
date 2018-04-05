/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxReporter.h"
#include "cxLandmarkListener.h"
#include "cxActiveData.h"

namespace cx
{

LandmarkPatientRegistrationWidget::LandmarkPatientRegistrationWidget(RegServicesPtr services, QWidget* parent, QString objectName, QString windowTitle) :
	LandmarkRegistrationWidget(services, parent, objectName, windowTitle)
{
	mLandmarkTableWidget->hide();

	mLandmarkListener->useI2IRegistration(false);

	mFixedProperty.reset(new StringPropertyRegistrationFixedImage(services->registration(), services->patient()));
	connect(services->patient().get(), &PatientModelService::rMprChanged, this, &LandmarkPatientRegistrationWidget::setModified);


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
	mServices->view()->setRegistrationMode(rsPATIENT_REGISTRATED);
	LandmarkRegistrationWidget::showEvent(event);
}

void LandmarkPatientRegistrationWidget::hideEvent(QHideEvent* event)
{
	mServices->view()->setRegistrationMode(rsNOT_REGISTRATED);
	LandmarkRegistrationWidget::hideEvent(event);
}

//The following functions look (almost) exactly like the same functions in PatientLandMarksWidget
void LandmarkPatientRegistrationWidget::performRegistration()
{
	if (!mServices->registration()->getFixedData())
	{
		ActiveDataPtr activeData = mServices->patient()->getActiveData();
		mServices->registration()->setFixedData(activeData->getActive<Image>());
	}

	if (mServices->patient()->getPatientLandmarks()->getLandmarks().size() < 3)
		return;

	mServices->registration()->doPatientRegistration();

	this->updateAverageAccuracyLabel();
}

LandmarkMap LandmarkPatientRegistrationWidget::getTargetLandmarks() const
{
	return mServices->patient()->getPatientLandmarks()->getLandmarks();
}

Transform3D LandmarkPatientRegistrationWidget::getTargetTransform() const
{
	Transform3D rMpr = mServices->patient()->get_rMpr();
	return rMpr;
}

void LandmarkPatientRegistrationWidget::setTargetLandmark(QString uid, Vector3D p_target)
{
	mServices->patient()->getPatientLandmarks()->setLandmark(Landmark(uid, p_target));
	reporter()->playSampleSound();
}

QString LandmarkPatientRegistrationWidget::getTargetName() const
{
	return "Patient";
}



}//namespace cx
