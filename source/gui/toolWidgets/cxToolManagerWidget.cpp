#include <cxToolManagerWidget.h>

namespace cx
{

ToolManagerWidget::ToolManagerWidget(QWidget* parent) :
    BaseWidget(parent, "ToolManagerWidget", "ToolManager debugger")
{
}

ToolManagerWidget::~ToolManagerWidget()
{
}

QString ToolManagerWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>ToolManager debugging utilities.</h3>"
      "<p>Lets you test different aspcets of the toolmanager.</p>"
      "<p><i></i></p>"
      "</html>";
}

}
