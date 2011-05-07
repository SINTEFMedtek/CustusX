/*
 * cxVisualizationMethodsWidget.cpp
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#include <cxVisualizationMethodsWidget.h>

#include "cxSurfaceWidget.h"

namespace cx
{

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

}
