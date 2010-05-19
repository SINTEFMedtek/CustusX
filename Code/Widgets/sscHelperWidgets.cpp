/*
 * sscHelperWidgets.cpp
 *
 *  Created on: May 18, 2010
 *      Author: christiana
 */
#include "sscHelperWidgets.h"

#include "sscVector3D.h"

namespace ssc
{

SliderGroupWidget::SliderGroupWidget(QWidget* parent, ssc::DoubleDataInterfacePtr dataInterface, QGridLayout* gridLayout, int row)
{
  mData = dataInterface;
  connect(mData.get(), SIGNAL(changed()), this, SLOT(dataChanged()));

  QHBoxLayout* topLayout = new QHBoxLayout;
  topLayout->setMargin(0);
  this->setLayout(topLayout);

  mLabel = new QLabel(this);
  mLabel->setText(mData->getValueName());
  topLayout->addWidget(mLabel);

  mEdit = new ssc::DoubleLineEdit(this);
  topLayout->addWidget(mEdit);
  connect(mEdit, SIGNAL(textEdited(const QString&)), this, SLOT(textEditedSlot(const QString&)));

  mSlider = new ssc::DoubleSlider(this);
  mSlider->setOrientation(Qt::Horizontal);
  //mSlider->setDoubleRange(mData->getValueRange());
  topLayout->addWidget(mSlider);
  connect(mSlider, SIGNAL(doubleValueChanged(double)), this, SLOT(doubleValueChanged(double)));


  if (gridLayout) // add to input gridlayout
  {
    gridLayout->addWidget(mLabel,  row, 0);
    gridLayout->addWidget(mEdit,   row, 1);
    gridLayout->addWidget(mSlider, row, 2);
  }
  else // add directly to this
  {
    topLayout->addWidget(mLabel);
    topLayout->addWidget(mEdit);
    topLayout->addWidget(mSlider);
  }

  dataChanged();
}

void SliderGroupWidget::doubleValueChanged(double val)
{
  val = mData->convertDisplay2Internal(val);

  if (ssc::similar(val, mData->getValue()))
      return;

  mData->setValue(val);
}

void SliderGroupWidget::textEditedSlot(const QString& text)
{
  double defVal =mData->convertInternal2Display(mData->getValue()); // defval in display space
  double newVal = mData->convertDisplay2Internal(mEdit->getDoubleValue(defVal)); // newval iin internal space

  if (ssc::similar(newVal, mData->getValue()))
      return;

  mData->setValue(newVal);
}

void SliderGroupWidget::dataChanged()
{
  DoubleRange range = mData->getValueRange();
  DoubleRange dRange(
      mData->convertInternal2Display(range.min()),
      mData->convertInternal2Display(range.max()),
      mData->convertInternal2Display(range.step()));
  mSlider->setDoubleRange(dRange); // in case the image is changed

  //ssc::DoubleRange range = mData->getValueRange();
  //std::cout << "SliderGroup::dataChanged() " << range.min() << "," << range.max() << "," << range.step() << std::endl;
  mSlider->setDoubleValue(mData->convertInternal2Display(mData->getValue()));
  mEdit->setDoubleValue(mData->convertInternal2Display(mData->getValue()));
}

}
