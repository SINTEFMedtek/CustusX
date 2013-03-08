#include "cxPresetWidget.h"

namespace cx {

PresetWidget::PresetWidget(QWidget* parent, QString objectName, QString windowTitle) :
	BaseWidget(parent, objectName, windowTitle)
{
	// TODO Auto-generated constructor stub

}

PresetWidget::~PresetWidget() {
	// TODO Auto-generated destructor stub
}

QString PresetWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Presets</h3>"
    "<p>Lets you select a predefined set of options.</p>"
    "<p><i></i></p>"
    "</html>";
}

} /* namespace cx */
