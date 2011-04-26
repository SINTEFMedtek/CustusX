#include <cxConnectedThresholdImageFilterWidget.h>

namespace cx
{

ConnectedThresholdImageFilterWidget::ConnectedThresholdImageFilterWidget(QWidget* parent) :
    WhatsThisWidget(parent, "ConnectedThresholdImageFilterWidget", "Connected Threshold Image Filter")
{
}

ConnectedThresholdImageFilterWidget::~ConnectedThresholdImageFilterWidget()
{
}

QString ConnectedThresholdImageFilterWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Connected Threshold Image Filter.</h3>"
      "<p>Labels pixels with a ReplaceValue that are connected to an initial seed and lies within a lower and upper threshold range.</p>"
      "<p><i></i></p>"
      "</html>";
}

}
