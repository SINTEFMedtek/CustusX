/*
 * cxToolManualCalibrationWidget.cpp
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#include <cxToolManualCalibrationWidget.h>

namespace cx
{

ToolManualCalibrationWidget::ToolManualCalibrationWidget(QWidget* parent) :
    BaseWidget(parent, "TemporalCalibrationWidget", "Temporal Calibrate")
{
  QVBoxLayout* toplayout = new QVBoxLayout(this);
}


QString ToolManualCalibrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Tool Manual Calibration.</h3>"
      "<p><i>Manipulate the tool calibration matrix sMt directly, using the matrix manipulation interface.</i></br>"
      "</html>";
}


}
