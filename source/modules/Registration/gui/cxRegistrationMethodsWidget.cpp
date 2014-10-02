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
#include "cxRegistrationMethodsWidget.h"

#include "cxTabbedWidget.h"

#include <iostream>
#include <QTabWidget>
#include <QVBoxLayout>

//registration
#include "cxLandmarkImageRegistrationWidget.h"
#include "cxLandmarkImage2ImageRegistrationWidget.h"
#include "cxLandmarkPatientRegistrationWidget.h"
#include "cxFastImageRegistrationWidget.h"
#include "cxFastPatientRegistrationWidget.h"
#include "cxFastOrientationRegistrationWidget.h"
#include "cxPlateRegistrationWidget.h"
#include "cxRegisterI2IWidget.h"
#include "cxElastixWidget.h"
#include "cxManualRegistrationWidget.h"
#include "cxSettings.h"
#include "cxPatientOrientationWidget.h"

#include "cxPrepareVesselsWidget.h"
#include "cxBronchoscopyRegistrationWidget.h"

namespace cx
{

LandmarkRegistrationsWidget::LandmarkRegistrationsWidget(QWidget* parent, QString objectName, QString windowTitle) :
  TabbedWidget(parent, objectName, windowTitle)
{
}

QString LandmarkRegistrationsWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Landmark based registrations.</h3>"
      "<p>This is a set of methods that use landmarks/fiducials to connect the images with the real world.</p>"
      "<p><i>Choose a step to continue..</i></p>"
      "</html>";
}

//------------------------------------------------------------------------------

FastRegistrationsWidget::FastRegistrationsWidget(QWidget* parent, QString objectName, QString windowTitle) :
  TabbedWidget(parent, objectName, windowTitle)
{
}

QString FastRegistrationsWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3>Fast and approximate registration.</h3>"
			"<p><b>Prerequisite:</b> Correctly oriented DICOM axes.</p>"
			"<p>This is a method designed to be quick and easy, it uses a tools orientation and a landmarks translation to determine where the image is relative to the patient.</p>"
			"<p><i>Choose a step to continue.</i></p>"
			"</html>";
}

//------------------------------------------------------------------------------

Image2ImageRegistrationWidget::Image2ImageRegistrationWidget(QWidget* parent, QString objectName, QString windowTitle) :
  TabbedWidget(parent, objectName, windowTitle)
{
}

QString Image2ImageRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Image to image registration.</h3>"
      "<p>This is a method used to registrate one image to another using segments of the images.</p>"
      "<p><i>Choose a step to continue.</i></p>"
      "</html>";
}

//------------------------------------------------------------------------------

Image2PlateRegistrationWidget::Image2PlateRegistrationWidget(QWidget* parent, QString objectName, QString windowTitle) :
  TabbedWidget(parent, objectName, windowTitle)
{
}

QString Image2PlateRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Image to plate registration.</h3>"
      "<p>This is a method used to registrate one image to a specific aurora tool (a plastic plate with ct markers).</p>"
      "<p><i>Choose a step to continue.</i></p>"
      "</html>";
}

//------------------------------------------------------------------------------

ManualRegistrationsWidget::ManualRegistrationsWidget(QWidget* parent, QString objectName, QString windowTitle) :
  TabbedWidget(parent, objectName, windowTitle)
{
}

QString ManualRegistrationsWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Manual registration.</h3>"
      "<p>Directly manipulate the image or patient registrations via either"
      "the defining matrix, or using sliders.</p>"
      "<p><i>Chose image or patient</i></p>"
      "</html>";
}

//------------------------------------------------------------------------------

RegistrationMethodsWidget::RegistrationMethodsWidget(RegistrationServicePtr registrationService, VisualizationServicePtr visualizationService, PatientModelServicePtr patientModelService, QWidget* parent, QString objectName, QString windowTitle) :
  TabbedWidget(parent, objectName, windowTitle)
{


  //landmark
  LandmarkRegistrationsWidget* landmarkRegistrationsWidget = new LandmarkRegistrationsWidget(this, "LandmarkRegistrationWidget", "Landmark Registrations");
	LandmarkImageRegistrationWidget* imageRegistrationWidget = new LandmarkImageRegistrationWidget(registrationService, patientModelService, landmarkRegistrationsWidget, "LMImageRegistrationWidget", "Image Registration");
	LandmarkPatientRegistrationWidget* patientRegistrationWidget = new LandmarkPatientRegistrationWidget(registrationService, patientModelService, landmarkRegistrationsWidget, "LMPatientRegistrationWidget", "Patient Registration");
	LandmarkImage2ImageRegistrationWidget* image2imageRegistrationWidget = new LandmarkImage2ImageRegistrationWidget(registrationService, patientModelService, landmarkRegistrationsWidget, "LMImage2ImageRegistrationWidget", "Image2Image Registration");
  landmarkRegistrationsWidget->addTab(imageRegistrationWidget, "Image");
  landmarkRegistrationsWidget->addTab(patientRegistrationWidget, "Image2Patient");
  landmarkRegistrationsWidget->addTab(image2imageRegistrationWidget, "Image2Image");

  //fast
  FastRegistrationsWidget* fastRegistrationsWidget = new FastRegistrationsWidget(this, "FastRegistrationWidget", "Fast Registrations");
	FastOrientationRegistrationWidget* fastOrientationRegistrationWidget = new FastOrientationRegistrationWidget(registrationService, fastRegistrationsWidget);
	FastImageRegistrationWidget* fastImageRegistrationWidget = new FastImageRegistrationWidget(registrationService, patientModelService, fastRegistrationsWidget, "FastImageRegistrationWidget", "Fast Image Registration");
	FastPatientRegistrationWidget* fastPatientRegistrationWidget = new FastPatientRegistrationWidget(registrationService, patientModelService, fastRegistrationsWidget);
  fastRegistrationsWidget->addTab(fastOrientationRegistrationWidget, "Orientation");
  fastRegistrationsWidget->addTab(fastImageRegistrationWidget, "Image");
  fastRegistrationsWidget->addTab(fastPatientRegistrationWidget, "Image2Patient");

  //fast
  ManualRegistrationsWidget* manRegWidget = new ManualRegistrationsWidget(this, "ManualRegistrationWidget", "Manual Registrations");
	ManualImage2ImageRegistrationWidget* manImage2ImageRegWidget = new ManualImage2ImageRegistrationWidget(registrationService, patientModelService, manRegWidget);
	ManualImageTransformRegistrationWidget* manImageTransformRegWidget = new ManualImageTransformRegistrationWidget(registrationService, patientModelService, manRegWidget);
	ManualPatientRegistrationWidget* manPatientRegWidget = new ManualPatientRegistrationWidget(registrationService, manRegWidget);
  manRegWidget->addTab(manImage2ImageRegWidget, "Image2Image");
  manRegWidget->addTab(manImageTransformRegWidget, "Image Transform");
  manRegWidget->addTab(manPatientRegWidget, "Patient");


  //vessel based image to image
  Image2ImageRegistrationWidget* image2imageWidget = new Image2ImageRegistrationWidget(this, "Image2ImageRegistrationWidget", "Image 2 Image Registration");

  PrepareVesselsWidget* prepareRegistrationWidget = new PrepareVesselsWidget(registrationService, visualizationService, patientModelService, image2imageWidget);

  image2imageWidget->addTab(prepareRegistrationWidget, "Prepare"); //should be application specific
	image2imageWidget->addTab(new RegisterI2IWidget(registrationService, patientModelService, image2imageWidget),"Register");

  //patient orientation
	PatientOrientationWidget* patientOrientationWidget = new PatientOrientationWidget(registrationService, patientModelService, this);

  //plate
	Image2PlateRegistrationWidget* imageAndPlateRegistrationWidget = new Image2PlateRegistrationWidget(this, "PlateRegistrationWidget", "Plate");
	PlateImageRegistrationWidget* platesImageRegistrationWidget = new PlateImageRegistrationWidget(registrationService, patientModelService, imageAndPlateRegistrationWidget);
	PlateRegistrationWidget* plateRegistrationWidget = new PlateRegistrationWidget(registrationService, patientModelService, imageAndPlateRegistrationWidget);
  imageAndPlateRegistrationWidget->addTab(plateRegistrationWidget, "Plate");
  imageAndPlateRegistrationWidget->addTab(platesImageRegistrationWidget, "Image");

  this->addTab(landmarkRegistrationsWidget, "Landmark");
  this->addTab(fastRegistrationsWidget, "Fast");
  this->addTab(manRegWidget, "Manual");
	this->addTab(new ElastixWidget(registrationService, patientModelService, this), "ElastiX");
  this->addTab(image2imageWidget, "Vessel");
  this->addTab(patientOrientationWidget, "Patient Orientation");
  this->addTab(imageAndPlateRegistrationWidget, "Plate");
  this->addTab(new BronchoscopyRegistrationWidget(registrationService, visualizationService, patientModelService, this), "Bronchoscopy");

  connect(mTabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChangedSlot(int)));
  mTabWidget->setCurrentIndex(settings()->value("registration/tabIndex").toInt());
}

void RegistrationMethodsWidget::tabChangedSlot(int value)
{
	settings()->setValue("registration/tabIndex", value);
}

QString RegistrationMethodsWidget::defaultWhatsThis() const
{
  return"<html>"
      "<h3>Registration methods.</h3>"
      "<p>This is a way to tell the system how to display data and tools in the same world.</p>"
      "<p><i>Choose a method.</i></p>"
      "</html>";
}


}
