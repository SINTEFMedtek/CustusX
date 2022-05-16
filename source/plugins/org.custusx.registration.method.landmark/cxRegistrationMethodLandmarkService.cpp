/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationMethodLandmarkService.h"
#include <QLabel>
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

	QLabel* heading = new QLabel(tabWidget);
	heading->setText("<font color=red>Intraoperative registration</font>");
	QFont font = heading->font();
	font.setBold(true);
	font.setPointSize(font.pointSize() + 2);
	heading->setFont(font);
	tabWidget->insertWidgetAtTop(heading);
	tabWidget->addTab(orientationWidget, "Orientation");
	tabWidget->addTab(imageLandmarkWidget, "Image landmark(s)");
	tabWidget->addTab(patientLandmarkWidget, "Patient landmark(s)");
	tabWidget->addTab(registrationWidget, "Register");

	return tabWidget;
}


} /* namespace cx */
