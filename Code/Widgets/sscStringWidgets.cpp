/*
 * sscStringWidgets.cpp
 *
 *  Created on: Jun 23, 2010
 *      Author: christiana
 */
#include "sscStringWidgets.h"
#include <iostream>
#include "sscTypeConversions.h"
namespace ssc
{



///----------------

ComboGroupWidget::ComboGroupWidget(QWidget* parent, ssc::StringDataAdapterPtr dataInterface, QGridLayout* gridLayout, int row) : QWidget(parent)
{
  mData = dataInterface;
  connect(mData.get(), SIGNAL(changed()), this, SLOT(dataChanged()));

  QHBoxLayout* topLayout = new QHBoxLayout;
  topLayout->setMargin(0);
  this->setLayout(topLayout);

  mLabel = new QLabel(this);
  mLabel->setText(mData->getValueName());
  topLayout->addWidget(mLabel);

  mCombo = new QComboBox(this);
  topLayout->addWidget(mCombo);
  connect(mCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));

  if (gridLayout) // add to input gridlayout
  {
    gridLayout->addWidget(mLabel,  row, 0);
    gridLayout->addWidget(mCombo,  row, 1);
  }
  else // add directly to this
  {
    topLayout->addWidget(mLabel);
    topLayout->addWidget(mCombo, 1);
  }

  dataChanged();
}

void ComboGroupWidget::comboIndexChanged(int index)
{
  mData->setValue(mCombo->itemData(index).toString());
}

void ComboGroupWidget::dataChanged()
{
  mCombo->blockSignals(true);
  mCombo->clear();

  QString currentValue = mData->getValue();
  QStringList range = mData->getValueRange();
  for (int i=0; i<range.size(); ++i)
  {
    mCombo->addItem(mData->convertInternal2Display(range[i]));
    mCombo->setItemData(i, range[i]);
    if (range[i]==currentValue)
      mCombo->setCurrentIndex(i);
  }

  mCombo->setToolTip(mData->getHelp());
  mLabel->setToolTip(mData->getHelp());
  mCombo->blockSignals(false);
}


} // namespace ssc
