/*
 * cxRegistrationMethodsWidget.cpp
 *
 *  \date May 2, 2011
 *      \author christiana
 */

#include <cxRegistrationMethodsWidget.h>

#include "cxTabbedWidget.h"

#include <iostream>
#include <QTabWidget>
#include <QVBoxLayout>

////segmentation
//#include "cxConnectedThresholdImageFilterWidget.h"
//#include "cxBinaryThresholdImageFilterWidget.h"

//registration
#include "cxLandmarkImageRegistrationWidget.h"
#include "cxLandmarkImage2ImageRegistrationWidget.h"
#include "cxLandmarkPatientRegistrationWidget.h"
#include "cxFastImageRegistrationWidget.h"
#include "cxFastPatientRegistrationWidget.h"
#include "cxFastOrientationRegistrationWidget.h"
#include "cxImageSegmentationAndCenterlineWidget.h"
#include "cxPlateRegistrationWidget.h"
//#include "cxManualRegistrationOffsetWidget.h"
#include "cxRegisterI2IWidget.h"
#include "cxElastixWidget.h"
#include "cxManualRegistrationWidget.h"
#include "cxSettings.h"
#include "cxPatientOrientationWidget.h"

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

RegistrationMethodsWidget::RegistrationMethodsWidget(RegistrationManagerPtr regManager, QWidget* parent, QString objectName, QString windowTitle) :
  TabbedWidget(parent, objectName, windowTitle)
{


  //landmark
  LandmarkRegistrationsWidget* landmarkRegistrationsWidget = new LandmarkRegistrationsWidget(this, "LandmarkRegistrationWidget", "Landmark Registrations");
  LandmarkImageRegistrationWidget* imageRegistrationWidget = new LandmarkImageRegistrationWidget(regManager, landmarkRegistrationsWidget, "LMImageRegistrationWidget", "Image Registration");
  LandmarkPatientRegistrationWidget* patientRegistrationWidget = new LandmarkPatientRegistrationWidget(regManager, landmarkRegistrationsWidget, "LMPatientRegistrationWidget", "Patient Registration");
  LandmarkImage2ImageRegistrationWidget* image2imageRegistrationWidget = new LandmarkImage2ImageRegistrationWidget(regManager, landmarkRegistrationsWidget, "LMImage2ImageRegistrationWidget", "Image2Image Registration");
  landmarkRegistrationsWidget->addTab(imageRegistrationWidget, "Image");
  landmarkRegistrationsWidget->addTab(patientRegistrationWidget, "Image2Patient");
  landmarkRegistrationsWidget->addTab(image2imageRegistrationWidget, "Image2Image");

  //fast
  FastRegistrationsWidget* fastRegistrationsWidget = new FastRegistrationsWidget(this, "FastRegistrationWidget", "Fast Registrations");
  FastOrientationRegistrationWidget* fastOrientationRegistrationWidget = new FastOrientationRegistrationWidget(regManager, fastRegistrationsWidget);
  FastImageRegistrationWidget* fastImageRegistrationWidget = new FastImageRegistrationWidget(regManager, fastRegistrationsWidget, "FastImageRegistrationWidget", "Fast Image Registration");
  FastPatientRegistrationWidget* fastPatientRegistrationWidget = new FastPatientRegistrationWidget(regManager, fastRegistrationsWidget);
  fastRegistrationsWidget->addTab(fastOrientationRegistrationWidget, "Orientation");
  fastRegistrationsWidget->addTab(fastImageRegistrationWidget, "Image");
  fastRegistrationsWidget->addTab(fastPatientRegistrationWidget, "Image2Patient");

  //fast
  ManualRegistrationsWidget* manRegWidget = new ManualRegistrationsWidget(this, "ManualRegistrationWidget", "Manual Registrations");
  ManualImageRegistrationWidget* manImageRegWidget = new ManualImageRegistrationWidget(regManager, manRegWidget);
  ManualPatientRegistrationWidget* manPatientRegWidget = new ManualPatientRegistrationWidget(regManager, manRegWidget);
  manRegWidget->addTab(manImageRegWidget, "Image");
  manRegWidget->addTab(manPatientRegWidget, "Patient");


  //vessel based image to image
  Image2ImageRegistrationWidget* image2imageWidget = new Image2ImageRegistrationWidget(this, "Image2ImageRegistrationWidget", "Image 2 Image Registration");
//  FixedImage2ImageWidget* fixedRegistrationWidget = new FixedImage2ImageWidget(image2imageWidget);
//  MovingImage2ImageWidget* movingRegistrationWidget = new MovingImage2ImageWidget(image2imageWidget);

  ImageSegmentationAndCenterlineWidget* prepareRegistrationWidget = new ImageSegmentationAndCenterlineWidget(regManager, image2imageWidget);


//  image2imageWidget->addTab(movingRegistrationWidget, "Moving"); //should be application specific
  image2imageWidget->addTab(prepareRegistrationWidget, "Prepare"); //should be application specific
  image2imageWidget->addTab(new RegisterI2IWidget(regManager, image2imageWidget),"Register");

  //manual offset
//  ManualRegistrationOffsetWidget* landmarkManualRegistrationOffsetWidget = new ManualRegistrationOffsetWidget(regManager, this);

  //patient orientation
  PatientOrientationWidget* patientOrientationWidget = new PatientOrientationWidget(regManager, this);

  //plate
  Image2PlateRegistrationWidget* imageAndPlateRegistrationWidget = new Image2PlateRegistrationWidget(this, "PlateRegistrationWidget", "Plate");
  PlateImageRegistrationWidget* platesImageRegistrationWidget = new PlateImageRegistrationWidget(regManager, imageAndPlateRegistrationWidget);
  PlateRegistrationWidget* plateRegistrationWidget = new PlateRegistrationWidget(regManager, imageAndPlateRegistrationWidget);
  imageAndPlateRegistrationWidget->addTab(plateRegistrationWidget, "Plate");
  imageAndPlateRegistrationWidget->addTab(platesImageRegistrationWidget, "Image");

  this->addTab(landmarkRegistrationsWidget, "Landmark");
  this->addTab(fastRegistrationsWidget, "Fast");
  this->addTab(manRegWidget, "Manual");
  this->addTab(new ElastixWidget(regManager), "ElastiX");
  this->addTab(image2imageWidget, "Vessel");
  this->addTab(patientOrientationWidget, "Patient Orientation");
  this->addTab(imageAndPlateRegistrationWidget, "Plate");

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
