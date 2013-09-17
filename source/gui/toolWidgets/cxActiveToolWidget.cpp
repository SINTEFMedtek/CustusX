#include <cxActiveToolWidget.h>

#include <QVBoxLayout>
#include "sscLabeledComboBoxWidget.h"
#include "cxToolDataAdapters.h"

namespace cx
{

ActiveToolWidget::ActiveToolWidget(QWidget* parent) :
    BaseWidget(parent, "ActiveToolWidget", "Active Tool")
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  this->setObjectName("ActiveToolWidget");
  layout->setMargin(0);

  LabeledComboBoxWidget*  combo = new LabeledComboBoxWidget(this, ActiveToolStringDataAdapter::New());
  layout->addWidget(combo);
}

QString ActiveToolWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Select active tool.</h3>"
      "<p>Lets you select which tool should be the active tool.</p>"
      "<p><i></i></p>"
      "</html>";
}

}
