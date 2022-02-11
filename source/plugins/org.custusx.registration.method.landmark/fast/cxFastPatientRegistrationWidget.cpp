/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFastPatientRegistrationWidget.h"

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

}//namespace cx
