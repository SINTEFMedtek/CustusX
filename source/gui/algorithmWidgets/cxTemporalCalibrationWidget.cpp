/*
 * cxTemporalCalibrationWidget.cpp
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#include <cxTemporalCalibrationWidget.h>

namespace cx
{

TemporalCalibrationWidget::TemporalCalibrationWidget(QWidget* parent) :
    BaseWidget(parent, "TemporalCalibrationWidget", "Temporal Calibrate")
{
  QVBoxLayout* toplayout = new QVBoxLayout(this);
}


QString TemporalCalibrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Temporal Calibration.</h3>"
      "<p><i>Clibrate the time shift between the tracking system and the video acquisition source.</i></br>"
      "</html>";
}

}
