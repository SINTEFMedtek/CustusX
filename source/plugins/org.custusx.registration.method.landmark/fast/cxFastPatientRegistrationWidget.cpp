/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFastPatientRegistrationWidget.h"

#include <QTableWidget>
#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxLogger.h"

namespace cx
{
FastPatientRegistrationWidget::FastPatientRegistrationWidget(RegServicesPtr services, QWidget* parent) :
		PatientLandMarksWidget(services, parent,
							   "org_custusx_registration_method_fast_landmark_image_to_patient_patient_landmarks_widget",
							   "Fast Patient Registration"),
		mMouseClickSample(NULL)
{
	mMouseClickSample = new QCheckBox("Sample with mouse clicks in anyplane view.", this);
	mMouseClickSample->setToolTip("Allow mouse clicks in 2D anyplane view to sample patient landmarks.");
	connect(mMouseClickSample, &QCheckBox::stateChanged, this, &FastPatientRegistrationWidget::mouseClickSampleStateChanged);

	mVerticalLayout->addWidget(mMouseClickSample);
}

FastPatientRegistrationWidget::~FastPatientRegistrationWidget()
{}

void FastPatientRegistrationWidget::performRegistration()
{
	mServices->registration()->doFastRegistration_Translation();
	this->updateAverageAccuracyLabel();
}

void FastPatientRegistrationWidget::mouseClickSampleStateChanged()
{
	if(mMouseClickSample->isChecked())
		connect(mServices->view().get(), &ViewService::pointSampled, this, &FastPatientRegistrationWidget::pointSampled);
	else
		disconnect(mServices->view().get(), &ViewService::pointSampled, this, &FastPatientRegistrationWidget::pointSampled);
}

QTableWidgetItem * FastPatientRegistrationWidget::getLandmarkTableItem()
{
	if(!mLandmarkTableWidget)
		return NULL;

	int row = mLandmarkTableWidget->currentRow();
	int column = mLandmarkTableWidget->currentColumn();

	if((row < 0) && (mLandmarkTableWidget->rowCount() >= 0))
		row = 0;
	if((column < 0) && (mLandmarkTableWidget->columnCount() >= 0))
		column = 0;

	QTableWidgetItem* item = mLandmarkTableWidget->item(row, column);

	return item;
}

void FastPatientRegistrationWidget::pointSampled(Vector3D p_r)
{
	QTableWidgetItem* item = getLandmarkTableItem();
	if(!item)
	{
		CX_LOG_WARNING() << "FastPatientRegistrationWidget::pointSampled() Cannot get item from mLandmarkTableWidget";
		return;
	}
	QString uid = item->data(Qt::UserRole).toString();

	Transform3D rMtarget = this->getTargetTransform();
	Vector3D p_target = rMtarget.inv().coord(p_r);

	this->setTargetLandmark(uid, p_target);
	this->performRegistration();
}

void FastPatientRegistrationWidget::showEvent(QShowEvent* event)
{
	mMouseClickSample->setChecked(true);
	PatientLandMarksWidget::showEvent(event);
}

void FastPatientRegistrationWidget::hideEvent(QHideEvent* event)
{
	mMouseClickSample->setChecked(false);
	PatientLandMarksWidget::hideEvent(event);
}

}//namespace cx
