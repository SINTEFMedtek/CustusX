#include "cxTabbedWidget.h"

#include <iostream>
#include <QTabWidget>
#include <QVBoxLayout>

//segmentation
#include "cxConnectedThresholdImageFilterWidget.h"
#include "cxBinaryThresholdImageFilterWidget.h"

#include "cxSurfaceWidget.h"
////registration
//#include "cxImageRegistrationWidget.h"
//#include "cxPatientRegistrationWidget.h"
//#include "cxFastImageRegistrationWidget.h"
//#include "cxFastPatientRegistrationWidget.h"
//#include "cxFastOrientationRegistrationWidget.h"
//#include "cxImageSegmentationAndCenterlineWidget.h"
//#include "cxPlateRegistrationWidget.h"
//#include "cxManualRegistrationOffsetWidget.h"
//#include "cxRegisterI2IWidget.h"

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
