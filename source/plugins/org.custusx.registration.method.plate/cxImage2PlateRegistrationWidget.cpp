#include "cxImage2PlateRegistrationWidget.h"

namespace cx {

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
} //cx
