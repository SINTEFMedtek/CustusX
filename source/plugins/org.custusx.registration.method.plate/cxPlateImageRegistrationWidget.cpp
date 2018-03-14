/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPlateImageRegistrationWidget.h"
#include "cxPatientModelService.h"
#include "cxRegistrationService.h"

namespace cx {

PlateImageRegistrationWidget::PlateImageRegistrationWidget(RegServicesPtr services, QWidget* parent) :
	FastImageRegistrationWidget(services, parent, "org_custusx_registration_method_plate_image_landmarks", "Plate Registration Image landmarks")
{
	this->setToolTip("Registration using a custom plate object");
}

PlateImageRegistrationWidget::~PlateImageRegistrationWidget()
{}

void PlateImageRegistrationWidget::editLandmarkButtonClickedSlot()
{
	mServices->patient()->setLandmarkActive(mActiveLandmark, true);
	ImageLandmarksWidget::editLandmarkButtonClickedSlot();
}

void PlateImageRegistrationWidget::performRegistration()
{
	FastImageRegistrationWidget::performRegistration();
	mServices->registration()->doFastRegistration_Translation();
}

} //cx
