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
#include "cxRegistrationMethodsWidget.h"
#include "cxLandmarkImageRegistrationWidget.h"
//#include "cxRegisterI2IWidget.h"
#include "cxLandmarkImage2ImageRegistrationWidget.h"
#include "cxLandmarkPatientRegistrationWidget.h"

#include "cxFastOrientationRegistrationWidget.h"
#include "cxFastImageRegistrationWidget.h"
#include "cxFastPatientRegistrationWidget.h"

namespace cx
{

QWidget *RegistrationMethodLandmarkImageToImageService::createWidget()
{
	LandmarkRegistrationsWidget* landmarkRegistrationsWidget = new LandmarkRegistrationsWidget(NULL, "LandmarkRegistrationImageToImageWidget", "Image to Image Landmark Registration");
	LandmarkImageRegistrationWidget* imageRegistrationWidget = new LandmarkImageRegistrationWidget(mRegistrationService, mPatientModelService, landmarkRegistrationsWidget, "LMImageRegistrationWidget", "Image Registration");
	LandmarkImage2ImageRegistrationWidget* image2imageRegistrationWidget = new LandmarkImage2ImageRegistrationWidget(mRegistrationService, mPatientModelService, landmarkRegistrationsWidget, "LMImage2ImageRegistrationWidget", "Image2Image Registration");

	//Try to reuse existing widget
//	Image2ImageRegistrationWidget* image2imageWidget = new Image2ImageRegistrationWidget(NULL, this->getWidgetName(), "Image 2 Image Registration");
//	landmarkRegistrationsWidget->addTab(new RegisterI2IWidget(mRegistrationService, mPatientModelService, image2imageWidget),"Register");

	landmarkRegistrationsWidget->addTab(imageRegistrationWidget, "Image landmarks");
	landmarkRegistrationsWidget->addTab(image2imageRegistrationWidget, "Register");

	return landmarkRegistrationsWidget;
}

QWidget *RegistrationMethodLandmarkImageToPatientService::createWidget()
{
	LandmarkRegistrationsWidget* landmarkRegistrationsWidget = new LandmarkRegistrationsWidget(NULL, "LandmarkRegistrationImageToImageWidget", "Image to Image Landmark Registration");
	LandmarkImageRegistrationWidget* imageRegistrationWidget = new LandmarkImageRegistrationWidget(mRegistrationService, mPatientModelService, landmarkRegistrationsWidget, "LMImageRegistrationWidget", "Image Registration");
	LandmarkPatientRegistrationWidget* patientRegistrationWidget = new LandmarkPatientRegistrationWidget(mRegistrationService, mPatientModelService, landmarkRegistrationsWidget, "LMPatientRegistrationWidget", "Patient Registration");

	landmarkRegistrationsWidget->addTab(imageRegistrationWidget, "Image landmarks");
	landmarkRegistrationsWidget->addTab(patientRegistrationWidget, "Patient landmarks");

	return landmarkRegistrationsWidget;
}

QWidget *RegistrationMethodFastLandmarkImageToPatientService::createWidget()
{
	FastRegistrationsWidget* fastRegistrationsWidget = new FastRegistrationsWidget(NULL, "FastRegistrationWidget", "Fast Landmark Registration");
	FastOrientationRegistrationWidget* fastOrientationRegistrationWidget = new FastOrientationRegistrationWidget(mRegistrationService, fastRegistrationsWidget);
	FastImageRegistrationWidget* fastImageRegistrationWidget = new FastImageRegistrationWidget(mRegistrationService, mPatientModelService, fastRegistrationsWidget, "FastImageRegistrationWidget", "Fast Image Registration");
	FastPatientRegistrationWidget* fastPatientRegistrationWidget = new FastPatientRegistrationWidget(mRegistrationService, mPatientModelService, fastRegistrationsWidget);
	fastRegistrationsWidget->addTab(fastOrientationRegistrationWidget, "Orientation");
	fastRegistrationsWidget->addTab(fastImageRegistrationWidget, "Image landmark(s)");
	fastRegistrationsWidget->addTab(fastPatientRegistrationWidget, "Patient landmark(s)");

	return fastRegistrationsWidget;
}


} /* namespace cx */
