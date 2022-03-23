/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFastPatientRegistrationWidget.h"

#include <QTableWidgetItem>

#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxLogger.h"

namespace cx
{
FastPatientRegistrationWidget::FastPatientRegistrationWidget(RegServicesPtr services, QWidget* parent) :
		PatientLandMarksWidget(services, parent,
							   "org_custusx_registration_method_fast_landmark_image_to_patient_patient_landmarks_widget",
							   "Fast Patient Registration")
{
}

FastPatientRegistrationWidget::~FastPatientRegistrationWidget()
{}

void FastPatientRegistrationWidget::performRegistration()
{
	mServices->registration()->doFastRegistration_Translation();
	this->updateAverageAccuracyLabel();
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

}//namespace cx
