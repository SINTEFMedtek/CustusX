/*
 * sscCheckBoxWidget.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: christiana
 */

#include <sscCheckBoxWidget.h>
#include <QGridLayout>


namespace ssc
{


CheckBoxWidget::CheckBoxWidget(QWidget* parent, ssc::BoolDataAdapterPtr dataInterface, QGridLayout* gridLayout, int row) : QWidget(parent)
{
  mData = dataInterface;
  connect(mData.get(), SIGNAL(changed()), this, SLOT(dataChanged()));

  QHBoxLayout* topLayout = new QHBoxLayout;
  topLayout->setMargin(0);
  this->setLayout(topLayout);

  mCheckBox = new QCheckBox(this);
  mCheckBox->setText(dataInterface->getValueName());
  topLayout->addWidget(mCheckBox);
  connect(mCheckBox, SIGNAL(toggled(bool)), this, SLOT(valueChanged(bool)));

  if (gridLayout) // add to input gridlayout
  {
    gridLayout->addWidget(mCheckBox,  row, 0, 1, 2);
  }
  else // add directly to this
  {
    topLayout->addWidget(mCheckBox);
  }

  dataChanged();
}

void CheckBoxWidget::valueChanged(bool val)
{
  if (val==mData->getValue())
      return;
  mData->setValue(val);
}

void CheckBoxWidget::dataChanged()
{
  mCheckBox->blockSignals(true);

  mCheckBox->setChecked(mData->getValue());
  mCheckBox->setToolTip(mData->getHelp());

  mCheckBox->blockSignals(false);
}

// --------------------------------------------------------


}
