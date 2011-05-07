/*
 * cxActiveToolWidget.cpp
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#include <cxActiveToolWidget.h>

#include <QVBoxLayout>
#include "sscLabeledComboBoxWidget.h"
#include "cxToolDataAdapters.h"

namespace cx
{

ActiveToolWidget::ActiveToolWidget(QWidget* parent) :
QWidget(parent)
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  this->setObjectName("ActiveToolWidget");
  layout->setMargin(0);

  ssc::LabeledComboBoxWidget*  combo = new ssc::LabeledComboBoxWidget(this, ActiveToolStringDataAdapter::New());
  layout->addWidget(combo);
}

}
