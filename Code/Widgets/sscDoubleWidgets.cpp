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


ScalarInteractionWidget::ScalarInteractionWidget(QWidget* parent, ssc::DoubleDataAdapterPtr dataInterface) :
    QWidget(parent),
    mLabel(NULL), mSlider(NULL), mEdit(NULL), mSpinBox(NULL)
{
  mData = dataInterface;
  connect(mData.get(), SIGNAL(changed()), this, SLOT(dataChanged()));

  this->enableLabel();
}

void ScalarInteractionWidget::enableLabel()
{
  mLabel = new QLabel(this);
  mLabel->setText(mData->getValueName());
}
void ScalarInteractionWidget::enableSlider()
{
  mSlider = new ssc::DoubleSlider(this);
  mSlider->setMinimumWidth(50);
  mSlider->setOrientation(Qt::Horizontal);
  connect(mSlider, SIGNAL(doubleValueChanged(double)), this, SLOT(doubleValueChanged(double)));
}
void ScalarInteractionWidget::enableEdit()
{
  mEdit = new ssc::DoubleLineEdit(this);
  connect(mEdit, SIGNAL(editingFinished()), this, SLOT(textEditedSlot()));
}
void ScalarInteractionWidget::enableSpinBox()
{
  mSpinBox = new QDoubleSpinBox(this);
  connect(mSpinBox, SIGNAL(valueChanged(double)), this, SLOT(doubleValueChanged(double)));
}

/**Layout all widgets in this, horizontal style.
 *
 */
void ScalarInteractionWidget::addToOwnLayout()
{
  QHBoxLayout* topLayout = new QHBoxLayout;
  topLayout->setMargin(0);
  this->setLayout(topLayout);

  if (mLabel)
    topLayout->addWidget(mLabel, 0);
  if (mEdit)
    topLayout->addWidget(mEdit, 0);
  if (mSpinBox)
    topLayout->addWidget(mSpinBox, 0);
  if (mSlider)
    topLayout->addWidget(mSlider, 1);
}

/**Layout all widgets into the input grid, including this.
 *
 */
void ScalarInteractionWidget::addToGridLayout(QGridLayout* gridLayout, int row)
{
  //Since SliderGroupWidget (this) is a widget we need to add this.
  //If not we will get an invisible widget on top of all the other widgets of the parent
  QHBoxLayout* hackLayout = new QHBoxLayout;
  hackLayout->setMargin(0);
  hackLayout->setSpacing(0);

  int col=0;
  hackLayout->addWidget(mLabel);
  hackLayout->addWidget(this);
  gridLayout->addLayout(hackLayout,  row, col++);

  if (mEdit)
    gridLayout->addWidget(mEdit,   row, col++);
  if (mSpinBox)
    gridLayout->addWidget(mSpinBox, row, col++);
  if (mSlider)
  {
    gridLayout->setColumnStretch(col,1);
    gridLayout->addWidget(mSlider, row, col++);
  }
}

void ScalarInteractionWidget::doubleValueChanged(double val)
{
  val = mData->convertDisplay2Internal(val);

  if (ssc::similar(val, mData->getValue()))
      return;

  mData->setValue(val);
}

void ScalarInteractionWidget::textEditedSlot()
{
  if (!mEdit)
    return;

  double defVal =mData->convertInternal2Display(mData->getValue()); // defval in display space
  double newVal = mData->convertDisplay2Internal(mEdit->getDoubleValue(defVal)); // newval iin internal space

  if (ssc::similar(newVal, mData->getValue()))
      return;

  mData->setValue(newVal);
}

void ScalarInteractionWidget::dataChanged()
{
  DoubleRange range = mData->getValueRange();
  DoubleRange dRange(
      mData->convertInternal2Display(range.min()),
      mData->convertInternal2Display(range.max()),
      mData->convertInternal2Display(range.step()));

  if (mSlider)
  {
    mSlider->blockSignals(true);
    mSlider->setDoubleRange(dRange); // in case the image is changed
    mSlider->setDoubleValue(mData->convertInternal2Display(mData->getValue()));
    mSlider->setToolTip(mData->getHelp());
    mSlider->blockSignals(false);
  }

  if (mEdit)
  {
    mEdit->blockSignals(true);
    mEdit->setDoubleValue(mData->convertInternal2Display(mData->getValue()));
    mEdit->setToolTip(mData->getHelp());
    mEdit->blockSignals(false);
  }

  if (mSpinBox)
  {
    mSpinBox->blockSignals(true);
    mSpinBox->setRange(dRange.min(), dRange.max()); // in case the image is changed
    mSpinBox->setSingleStep(dRange.step());
    mSpinBox->setDecimals(mData->getValueDecimals());
    mSpinBox->setValue(mData->convertInternal2Display(mData->getValue()));
    mSpinBox->setToolTip(mData->getHelp());
    mSpinBox->blockSignals(false);
  }

  if (mLabel)
  {
    mLabel->setToolTip(mData->getHelp());
  }
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

SliderGroupWidget::SliderGroupWidget(QWidget* parent, ssc::DoubleDataAdapterPtr dataInterface, QGridLayout* gridLayout, int row) :
    ScalarInteractionWidget(parent, dataInterface)
{
  this->enableSlider();
  this->enableEdit();
//  this->enableSpinBox(true);

  if (gridLayout)
    this->addToGridLayout(gridLayout, row);
  else
    this->addToOwnLayout();

  dataChanged();
}

// --------------------------------------------------------
// --------------------------------------------------------

SpinBoxGroupWidget::SpinBoxGroupWidget(QWidget* parent, ssc::DoubleDataAdapterPtr dataInterface, QGridLayout* gridLayout, int row) :
    ScalarInteractionWidget(parent, dataInterface)
{
  this->enableSpinBox();

  if (gridLayout)
    this->addToGridLayout(gridLayout, row);
  else
    this->addToOwnLayout();

  dataChanged();
}

// --------------------------------------------------------

SpinBoxAndSliderGroupWidget::SpinBoxAndSliderGroupWidget(QWidget* parent, ssc::DoubleDataAdapterPtr dataInterface, QGridLayout* gridLayout, int row) :
    ScalarInteractionWidget(parent, dataInterface)
{
  this->enableSpinBox();
  this->enableSlider();

  if (gridLayout)
    this->addToGridLayout(gridLayout, row);
  else
    this->addToOwnLayout();

  dataChanged();
}

} // namespace ssc
