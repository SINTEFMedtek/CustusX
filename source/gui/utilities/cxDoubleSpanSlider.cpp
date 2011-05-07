/*
 * cxDoubleSpanSlider.cpp
 *
 *  Created on: Aug 26, 2010
 *      Author: christiana
 */
#include "cxDoubleSpanSlider.h"

namespace cx
{


void SliderRangeGroupWidget::addToGridLayout(QGridLayout* gridLayout, int row)
{
  gridLayout->addWidget(mLabel,      row, 0);
  gridLayout->addWidget(mLowerEdit,  row, 1);
  gridLayout->addWidget(mUpperEdit,  row, 1);
  gridLayout->addWidget(mSpanSlider, row, 2);
}

void SliderRangeGroupWidget::setName(QString text)
{
  mLabel->setText(text);
}

SliderRangeGroupWidget::SliderRangeGroupWidget(QWidget* parent) : QWidget(parent)
{
//  mData = dataInterface;
//  connect(mData.get(), SIGNAL(changed()), this, SLOT(dataChanged()));

  QHBoxLayout* topLayout = new QHBoxLayout;
  topLayout->setMargin(0);
  this->setLayout(topLayout);

  mLabel = new QLabel(this);
  //mLabel->setText(mData->getValueName());
  topLayout->addWidget(mLabel);

  mLowerEdit = new ssc::DoubleLineEdit(this);
  topLayout->addWidget(mLowerEdit);
  connect(mLowerEdit, SIGNAL(editingFinished()), this, SLOT(textEditedSlot()));

  mSpanSlider = new DoubleSpanSlider(this);
  mSpanSlider->setOrientation(Qt::Horizontal);
//  mSpanSlider->setRange(-500, 500);
//  mSpanSlider->setLowerValue(-200);
//  mSpanSlider->setUpperValue(200);
  mSpanSlider->setHandleMovementMode(QxtSpanSlider::NoOverlapping);
  topLayout->addWidget(mSpanSlider, 1);
  // connect to slider
  connect(mSpanSlider, SIGNAL(doubleSpanChanged(double, double)), this, SLOT(doubleSpanChangedSlot(double, double)));

  mUpperEdit = new ssc::DoubleLineEdit(this);
  topLayout->addWidget(mUpperEdit);
  connect(mUpperEdit, SIGNAL(editingFinished()), this, SLOT(textEditedSlot()));

//  mSlider = new ssc::DoubleSlider(this);
//  mSlider->setOrientation(Qt::Horizontal);
//  //mSlider->setDoubleRange(mData->getValueRange());
//  topLayout->addWidget(mSlider);
//  connect(mSlider, SIGNAL(doubleValueChanged(double)), this, SLOT(doubleValueChanged(double)));


//  dataChanged();
}

void SliderRangeGroupWidget::setRange(const ssc::DoubleRange& range)
{
//  std::cout << "SliderRangeGroupWidget::setRange " << mLabel->text() << " " << range.min() <<  "," << range.max() << std::endl;
  mSpanSlider->setDoubleRange(range);
}

void SliderRangeGroupWidget::setValue(std::pair<double,double> val)
{
  mValue = val;
  this->dataChanged(val);
}

std::pair<double,double> SliderRangeGroupWidget::getValue() const
{
  return mValue;
}

void SliderRangeGroupWidget::doubleSpanChangedSlot(double lower, double upper)
{
  mValue = std::make_pair(lower,upper);
  dataChanged(mValue);
  emit valueChanged(mValue.first, mValue.second);
}

void SliderRangeGroupWidget::textEditedSlot()
{
  mValue = std::make_pair(mLowerEdit->getDoubleValue(0), mUpperEdit->getDoubleValue(0));
  dataChanged(mValue);
  emit valueChanged(mValue.first, mValue.second);
}

void SliderRangeGroupWidget::dataChanged(std::pair<double,double> val)
{
  mSpanSlider->blockSignals(true);
  mLowerEdit->blockSignals(true);
  mUpperEdit->blockSignals(true);

  mSpanSlider->setDoubleLowerValue(val.first);
  mSpanSlider->setDoubleUpperValue(val.second);
  mLowerEdit->setDoubleValue(val.first);
  mUpperEdit->setDoubleValue(val.second);

  mSpanSlider->blockSignals(false);
  mLowerEdit->blockSignals(false);
  mUpperEdit->blockSignals(false);
}



} // namespace cx
