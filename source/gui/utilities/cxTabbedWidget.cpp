#include "cxTabbedWidget.h"

#include <iostream>
#include <QTabWidget>
#include <QVBoxLayout>

//segmentation
#include "cxConnectedThresholdImageFilterWidget.h"
#include "cxBinaryThresholdImageFilterWidget.h"

//registration
#include "cxImageRegistrationWidget.h"
#include "cxPatientRegistrationWidget.h"
#include "cxFastImageRegistrationWidget.h"
#include "cxFastPatientRegistrationWidget.h"
#include "cxFastOrientationRegistrationWidget.h"
#include "cxImageSegmentationAndCenterlineWidget.h"
#include "cxPlateRegistrationWidget.h"
#include "cxManualRegistrationOffsetWidget.h"
#include "cxRegisterI2IWidget.h"

//calibration
#include "cxToolTipCalibrationWidget.h"

namespace cx
{
//------------------------------------------------------------------------------
TabbedWidget::TabbedWidget(QWidget* parent, QString objectName, QString windowTitle) :
    BaseWidget(parent, objectName, windowTitle),
    mTabWidget(new QTabWidget(this))
{

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mTabWidget);
  layout->setMargin(2); // lots of tabbed widgets in layers use up the desktop. Must reduce.
}

TabbedWidget::~TabbedWidget()
{}

void TabbedWidget::addTab(BaseWidget* newTab, QString newTabName)
{
  int index = mTabWidget->addTab(newTab, newTabName);
  mTabWidget->setTabWhatsThis(index, newTab->defaultWhatsThis());
  newTab->setWhatsThis(newTab->defaultWhatsThis());
}

QString TabbedWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Tabbed widget.</h3>"
      "<p>This is a tabbed widget, used for creating a hierarchy of tabbed widgets.</p>"
      "<p><i>If you see this whats this message, something's wrong!</i></p>"
      "</html>";
}
//------------------------------------------------------------------------------

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

RegistrationMethodsWidget::RegistrationMethodsWidget(QWidget* parent, QString objectName, QString windowTitle) :
  TabbedWidget(parent, objectName, windowTitle)
{
  //landmark
  LandmarkRegistrationsWidget* landmarkRegistrationsWidget = new LandmarkRegistrationsWidget(this, "LandmarkRegistrationWidget", "Landmark Registrations");
  ImageRegistrationWidget* imageRegistrationWidget = new ImageRegistrationWidget(landmarkRegistrationsWidget, "ImageRegistrationWidget", "Image Registration");
  PatientRegistrationWidget* patientRegistrationWidget = new PatientRegistrationWidget(landmarkRegistrationsWidget, "PatientRegistrationWidget", "Patient Registration");
  landmarkRegistrationsWidget->addTab(imageRegistrationWidget, "Image");
  landmarkRegistrationsWidget->addTab(patientRegistrationWidget, "Patient");

  //fast
  FastRegistrationsWidget* fastRegistrationsWidget = new FastRegistrationsWidget(this, "FastRegistrationWidget", "Fast Registrations");
  FastOrientationRegistrationWidget* fastOrientationRegistrationWidget = new FastOrientationRegistrationWidget(fastRegistrationsWidget);
  FastImageRegistrationWidget* fastImageRegistrationWidget = new FastImageRegistrationWidget(fastRegistrationsWidget, "FastImageRegistrationWidget", "Fast Image Registration");
  FastPatientRegistrationWidget* fastPatientRegistrationWidget = new FastPatientRegistrationWidget(fastRegistrationsWidget);
  fastRegistrationsWidget->addTab(fastOrientationRegistrationWidget, "Orientation");
  fastRegistrationsWidget->addTab(fastImageRegistrationWidget, "Image");
  fastRegistrationsWidget->addTab(fastPatientRegistrationWidget, "Patient");

  //vessel based image to image
  Image2ImageRegistrationWidget* image2imageWidget = new Image2ImageRegistrationWidget(this, "Image2ImageRegistrationWidget", "Image 2 Image Registration");
//  FixedImage2ImageWidget* fixedRegistrationWidget = new FixedImage2ImageWidget(image2imageWidget);
//  MovingImage2ImageWidget* movingRegistrationWidget = new MovingImage2ImageWidget(image2imageWidget);

  ImageSegmentationAndCenterlineWidget* prepareRegistrationWidget = new ImageSegmentationAndCenterlineWidget(image2imageWidget);


//  image2imageWidget->addTab(movingRegistrationWidget, "Moving"); //should be application specific
  image2imageWidget->addTab(prepareRegistrationWidget, "Prepare"); //should be application specific
  image2imageWidget->addTab(new RegisterI2IWidget(image2imageWidget),"Register");

  //manual offset
  ManualRegistrationOffsetWidget* landmarkManualRegistrationOffsetWidget = new ManualRegistrationOffsetWidget(this);

  //plate
  Image2PlateRegistrationWidget* imageAndPlateRegistrationWidget = new Image2PlateRegistrationWidget(this, "PlateRegistrationWidget", "Plate");
  PlateImageRegistrationWidget* platesImageRegistrationWidget = new PlateImageRegistrationWidget(imageAndPlateRegistrationWidget);
  PlateRegistrationWidget* plateRegistrationWidget = new PlateRegistrationWidget(imageAndPlateRegistrationWidget);
  imageAndPlateRegistrationWidget->addTab(plateRegistrationWidget, "Plate");
  imageAndPlateRegistrationWidget->addTab(platesImageRegistrationWidget, "Image");

  this->addTab(landmarkRegistrationsWidget, "Landmark");
  this->addTab(fastRegistrationsWidget, "Fast");
  this->addTab(landmarkManualRegistrationOffsetWidget, "Manual");
  this->addTab(image2imageWidget, "Image2Image");
  this->addTab(imageAndPlateRegistrationWidget, "Plate");
}

QString RegistrationMethodsWidget::defaultWhatsThis() const
{
  return"<html>"
      "<h3>Registration methods.</h3>"
      "<p>This is a way to tell the system how to display data and tools in the same world.</p>"
      "<p><i>Choose a method.</i></p>"
      "</html>";
}

//------------------------------------------------------------------------------
SegmentationMethodsWidget::SegmentationMethodsWidget(QWidget* parent, QString objectName, QString windowTitle) :
  TabbedWidget(parent, objectName, windowTitle)
{
  this->addTab(new BinaryThresholdImageFilterWidget(this), "Binary Threshold");

  //TODO finish widget before using it
//  this->addTab(new ConnectedThresholdImageFilterWidget(this), "Connected Threshold");
}

QString SegmentationMethodsWidget::defaultWhatsThis() const
{
  return"<html>"
      "<h3>Segmentation methods.</h3>"
      "<p>Segmentation methods are used to extract parts of a volume.</p>"
      "<p><i>Choose a method.</i></p>"
      "</html>";
}
//------------------------------------------------------------------------------
VisualizationMethodsWidget::VisualizationMethodsWidget(QWidget* parent, QString objectName, QString windowTitle) :
  TabbedWidget(parent, objectName, windowTitle)
{
  this->addTab(new SurfaceWidget(this), "Surface");
}

QString VisualizationMethodsWidget::defaultWhatsThis() const
{
  return"<html>"
      "<h3>Visualization methods.</h3>"
      "<p>These methods creates data structures that can be use in visualization.</p>"
      "<p><i>Choose a method.</i></p>"
      "</html>";
}
//------------------------------------------------------------------------------
CalibrationMethodsWidget::CalibrationMethodsWidget(QWidget* parent, QString objectName, QString windowTitle) :
  TabbedWidget(parent, objectName, windowTitle)
{
  this->addTab(new ToolTipCalibrationWidget(this), "Tool Tip");
}

QString CalibrationMethodsWidget::defaultWhatsThis() const
{
  return"<html>"
      "<h3>Calibration methods.</h3>"
      "<p>These methods creates data structures that can be use in visualization.</p>"
      "<p><i>Choose a method.</i></p>"
      "</html>";
}

//------------------------------------------------------------------------------
} //namespace cx
