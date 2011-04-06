/*
 * sscDoubleWidgets.cpp
 *
 *  Created on: Jun 23, 2010
 *      Author: christiana
 */
#include "sscDoubleWidgets.h"

#include <iostream>
#include "sscVector3D.h"
#include "sscTypeConversions.h"

namespace ssc
{

SliderGroupWidget::SliderGroupWidget(QWidget* parent, ssc::DoubleDataAdapterPtr dataInterface, QGridLayout* gridLayout, int row) : QWidget(parent)
{
  mData = dataInterface;
  connect(mData.get(), SIGNAL(changed()), this, SLOT(dataChanged()));

  mLabel = new QLabel(this);
  mLabel->setText(mData->getValueName());

  mEdit = new ssc::DoubleLineEdit(this);
//  connect(mEdit, SIGNAL(textEdited(const QString&)), this, SLOT(textEditedSlot()));
  connect(mEdit, SIGNAL(editingFinished()), this, SLOT(textEditedSlot()));

  mSlider = new ssc::DoubleSlider(this);
  mSlider->setOrientation(Qt::Horizontal);
  //mSlider->setDoubleRange(mData->getValueRange());
  connect(mSlider, SIGNAL(doubleValueChanged(double)), this, SLOT(doubleValueChanged(double)));


  if (gridLayout) // add to input gridlayout
  {
    //Since SliderGroupWidget (this) is a widget we need to add this.
    //If not we will get an invisible widget on top of all the other widgets of the parent
    QHBoxLayout* hackLayout = new QHBoxLayout;
    hackLayout->setMargin(0);
    hackLayout->setSpacing(0);
    hackLayout->addWidget(mLabel);
    hackLayout->addWidget(this);
    gridLayout->addLayout(hackLayout,  row, 0);
    gridLayout->addWidget(mEdit,   row, 1);
    gridLayout->addWidget(mSlider, row, 2);
  }
  else // add directly to this
  {
    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->setMargin(0);
    this->setLayout(topLayout);

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

void SliderGroupWidget::textEditedSlot()
{
  double defVal =mData->convertInternal2Display(mData->getValue()); // defval in display space
  double newVal = mData->convertDisplay2Internal(mEdit->getDoubleValue(defVal)); // newval iin internal space

  if (ssc::similar(newVal, mData->getValue()))
      return;

  mData->setValue(newVal);
}

void SliderGroupWidget::dataChanged()
{
  mSlider->blockSignals(true);
  mEdit->blockSignals(true);

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
  mSlider->setToolTip(mData->getHelp());
  mEdit->setToolTip(mData->getHelp());
  mLabel->setToolTip(mData->getHelp());

  mSlider->blockSignals(false);
  mEdit->blockSignals(false);
}

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------


QSize DoubleLineEdit::sizeHint() const
{
//  return QLineEdit::sizeHint();
  QSize size = QLineEdit::minimumSizeHint();
  //std::cout << "DoubleLineEdit::minimumSizeHint() " << size.width() << ", " << size.height() << std::endl;
  size.setWidth(size.height()*3);
  return size;
}

QSize DoubleLineEdit::minimumSizeHint() const
{
  QSize size = QLineEdit::minimumSizeHint();
  //std::cout << "DoubleLineEdit::minimumSizeHint() " << size.width() << ", " << size.height() << std::endl;
  //size.setWidth(size.height()*2);
  return size;
}


// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------



SpinBoxGroupWidget::SpinBoxGroupWidget(QWidget* parent, ssc::DoubleDataAdapterPtr dataInterface, QGridLayout* gridLayout, int row) : QWidget(parent)
{
  mData = dataInterface;
  connect(mData.get(), SIGNAL(changed()), this, SLOT(dataChanged()));

  QHBoxLayout* topLayout = new QHBoxLayout;
  topLayout->setMargin(0);
  this->setLayout(topLayout);

  mLabel = new QLabel(this);
  mLabel->setText(mData->getValueName());
  topLayout->addWidget(mLabel);

  mSpinBox = new QDoubleSpinBox(this);
  topLayout->addWidget(mSpinBox);
  connect(mSpinBox, SIGNAL(valueChanged(double)), this, SLOT(doubleValueChanged(double)));

  if (gridLayout) // add to input gridlayout
  {
    gridLayout->addWidget(mLabel,  row, 0);
    gridLayout->addWidget(mSpinBox,   row, 1);
  }
  else // add directly to this
  {
    topLayout->addWidget(mLabel);
    topLayout->addWidget(mSpinBox);
  }

  dataChanged();
}

void SpinBoxGroupWidget::doubleValueChanged(double val)
{
  val = mData->convertDisplay2Internal(val);
  if (ssc::similar(val, mData->getValue()))
      return;
  mData->setValue(val);
}

void SpinBoxGroupWidget::dataChanged()
{
  mSpinBox->blockSignals(true);

  DoubleRange range = mData->getValueRange();
  DoubleRange dRange(
      mData->convertInternal2Display(range.min()),
      mData->convertInternal2Display(range.max()),
      mData->convertInternal2Display(range.step()));
  mSpinBox->setRange(dRange.min(), dRange.max()); // in case the image is changed
  mSpinBox->setSingleStep(dRange.step());
  mSpinBox->setDecimals(mData->getValueDecimals());

  mSpinBox->setValue(mData->convertInternal2Display(mData->getValue()));

  mSpinBox->setToolTip(mData->getHelp());
  mLabel->setToolTip(mData->getHelp());

  mSpinBox->blockSignals(false);
}

// --------------------------------------------------------


} // namespace ssc
