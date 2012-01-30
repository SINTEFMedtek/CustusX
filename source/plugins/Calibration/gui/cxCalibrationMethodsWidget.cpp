/*
 * cxCalibrationMethodsWidget.cpp
 *
 *  \date May 4, 2011
 *      \author christiana
 */

#include <cxCalibrationMethodsWidget.h>

#include <cxToolTipSampleWidget.h>
#include "cxToolTipCalibrationWidget.h"
#include "cxToolManualCalibrationWidget.h"
#include "cxTemporalCalibrationWidget.h"

namespace cx
{

CalibrationMethodsWidget::CalibrationMethodsWidget(AcquisitionDataPtr acquisitionData, QWidget* parent, QString objectName, QString windowTitle) :
  TabbedWidget(parent, objectName, windowTitle)
{
  this->addTab(new ToolTipCalibrateWidget(this), "Tool Tip");
  this->addTab(new ToolTipSampleWidget(this), "Sample");
  this->addTab(new TemporalCalibrationWidget(acquisitionData, this), "Temporal");
  this->addTab(new ToolManualCalibrationWidget(this), "Tool Manual");
}

QString CalibrationMethodsWidget::defaultWhatsThis() const
{
  return"<html>"
      "<h3>Calibration methods.</h3>"
      "<p>These methods creates data structures that can be use in visualization.</p>"
      "<p><i>Choose a method.</i></p>"
      "</html>";
}

}
