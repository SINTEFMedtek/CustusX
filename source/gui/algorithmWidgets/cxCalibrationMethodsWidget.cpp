/*
 * cxCalibrationMethodsWidget.cpp
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#include <cxCalibrationMethodsWidget.h>

#include <cxToolTipSampleWidget.h>
#include "cxToolTipCalibrationWidget.h"
#include "cxToolManualCalibrationWidget.h"
#include "cxTemporalCalibrationWidget.h"

namespace cx
{

CalibrationMethodsWidget::CalibrationMethodsWidget(QWidget* parent, QString objectName, QString windowTitle) :
  TabbedWidget(parent, objectName, windowTitle)
{
  this->addTab(new ToolTipCalibrateWidget(this), "Tool Tip");
  this->addTab(new ToolTipSampleWidget(this), "Sample");
  this->addTab(new TemporalCalibrationWidget(this), "Temporal");
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
