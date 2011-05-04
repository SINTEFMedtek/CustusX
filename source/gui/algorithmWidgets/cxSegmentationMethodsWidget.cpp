/*
 * cxSegmentationMethodsWidget.cpp
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#include <cxSegmentationMethodsWidget.h>

#include "cxConnectedThresholdImageFilterWidget.h"
#include "cxBinaryThresholdImageFilterWidget.h"

namespace cx
{


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


}
