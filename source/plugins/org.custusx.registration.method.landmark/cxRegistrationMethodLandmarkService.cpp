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

#include "cxRegistrationMethodLandmarkService.h"
#include "cxImageLandmarksWidget.h"
#include "cxLandmarkImage2ImageRegistrationWidget.h"
#include "cxLandmarkPatientRegistrationWidget.h"
#include "cxPatientLandMarksWidget.h"

#include "cxFastOrientationRegistrationWidget.h"
#include "cxFastImageRegistrationWidget.h"
#include "cxFastPatientRegistrationWidget.h"
#include "cxFastLandmarkPatientRegistrationWidget.h"

#include "cxTabbedWidget.h"

namespace cx
{

QWidget *RegistrationMethodLandmarkImageToImageService::createWidget()
{
	TabbedWidget* tabWidget = new TabbedWidget(NULL, this->getWidgetName(), "Image to Image Landmark Registration");
	ImageLandmarksWidget* imageLandmarksWidget = new ImageLandmarksWidget(mServices, tabWidget, "org_custusx_registration_method_landmark_image_to_image_image_landmarks_widget", "Image Registration");
	LandmarkImage2ImageRegistrationWidget* image2imageRegistrationWidget = new LandmarkImage2ImageRegistrationWidget(mServices, tabWidget, "org_custusx_registration_method_landmark_image_to_image_register_widget", "Image2Image Registration");

	tabWidget->addTab(imageLandmarksWidget, "Image landmarks");
	tabWidget->addTab(image2imageRegistrationWidget, "Register");

	return tabWidget;
}

QWidget *RegistrationMethodLandmarkImageToPatientService::createWidget()
{
	TabbedWidget* tabWidget = new TabbedWidget(NULL, this->getWidgetName(), "Image to Patient Landmark Registration");
	ImageLandmarksWidget* imageLandmarksWidget = new ImageLandmarksWidget(mServices, tabWidget, "org_custusx_registration_method_landmark_image_to_patient_image_landmarks_widget", "Image Landmarks", true);
	PatientLandMarksWidget* patientLandmarksWidget = new PatientLandMarksWidget(mServices, tabWidget, "org_custusx_registration_method_landmark_image_to_patient_patient_landmarks_widget", "Patient Landmarks");

	LandmarkPatientRegistrationWidget* registrationWidget = new LandmarkPatientRegistrationWidget(mServices, tabWidget, "org_custusx_registration_method_landmark_image_to_patient_registration_widget", "Image To Patient Registration");

	tabWidget->addTab(imageLandmarksWidget, "Image landmarks");
	tabWidget->addTab(patientLandmarksWidget, "Patient landmarks");
	tabWidget->addTab(registrationWidget, "Register");

	return tabWidget;
}

QWidget *RegistrationMethodFastLandmarkImageToPatientService::createWidget()
{
	TabbedWidget* tabWidget = new TabbedWidget(NULL, this->getWidgetName(), "Fast Landmark Registration");
	FastOrientationRegistrationWidget* orientationWidget = new FastOrientationRegistrationWidget(mServices, tabWidget);
	FastImageRegistrationWidget* imageLandmarkWidget = new FastImageRegistrationWidget(mServices, tabWidget, "org_custusx_registration_method_fast_landmark_image_to_patient_image_landmarks_widget", "Fast Image Registration - Landmarks", true);
	FastPatientRegistrationWidget* patientLandmarkWidget = new FastPatientRegistrationWidget(mServices, tabWidget);

	LandmarkPatientRegistrationWidget* registrationWidget = new FastLandmarkPatientRegistrationWidget(mServices, tabWidget, "org_custusx_registration_method_fast_landmark_image_to_patient_registration_widget", "Fast Image Registration");

	tabWidget->addTab(orientationWidget, "Orientation");
	tabWidget->addTab(imageLandmarkWidget, "Image landmark(s)");
	tabWidget->addTab(patientLandmarkWidget, "Patient landmark(s)");
	tabWidget->addTab(registrationWidget, "Register");

	return tabWidget;
}


} /* namespace cx */
