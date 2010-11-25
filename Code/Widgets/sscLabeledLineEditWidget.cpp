#include "sscLabeledLineEditWidget.h"
#include <iostream>
#include "sscTypeConversions.h"

namespace ssc
{

///----------------

LabeledLineEditWidget::LabeledLineEditWidget(QWidget* parent, ssc::EditableStringDataAdapterPtr dataInterface, QGridLayout* gridLayout, int row) : QWidget(parent)
{
  mData = dataInterface;
  connect(mData.get(), SIGNAL(changed()), this, SLOT(dataChanged()));

  QHBoxLayout* topLayout = new QHBoxLayout;
  topLayout->setMargin(0);
  this->setLayout(topLayout);

  mLabel = new QLabel(this);
  mLabel->setText(mData->getValueName());
  topLayout->addWidget(mLabel);

  mLine = new QLineEdit(this);
  topLayout->addWidget(mLine);
  connect(mLine, SIGNAL(editingFinished()), this, SLOT(editingFinished()));

  if (gridLayout) // add to input gridlayout
  {
    gridLayout->addWidget(mLabel,  row, 0);
    gridLayout->addWidget(mLine,  row, 1);
  }
  else // add directly to this
  {
    topLayout->addWidget(mLabel);
    topLayout->addWidget(mLine, 1);
  }

  dataChanged();
}

void LabeledLineEditWidget::editingFinished()
{
  mData->setValue(mLine->text());
}

void LabeledLineEditWidget::dataChanged()
{
  mLine->blockSignals(true);
  mLine->setReadOnly(mData->isReadOnly());
  mLine->setText(mData->getValue());
  mLine->setToolTip(mData->getHelp());
  mLabel->setToolTip(mData->getHelp());
  mLine->blockSignals(false);
}


} // namespace ssc
