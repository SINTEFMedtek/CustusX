// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

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
#include "sscMousePadWidget.h"

namespace ssc
{

ScalarInteractionWidget::ScalarInteractionWidget(QWidget* parent, ssc::DoubleDataAdapterPtr dataInterface) :
    OptimizedUpdateWidget(parent), mSlider(NULL), mSpinBox(NULL), mLabel(NULL), mEdit(NULL), mInfiniteSlider(NULL)
{
	mData = dataInterface;
    connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));
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
void ScalarInteractionWidget::enableInfiniteSlider()
{
	QSize minBarSize = QSize(20, 20);
	mInfiniteSlider = new MousePadWidget(this, minBarSize);
	mInfiniteSlider->setFixedYPos(true);
	connect(mInfiniteSlider, SIGNAL(mouseMoved(QPointF)), this, SLOT(infiniteSliderMouseMoved(QPointF)));
}

void ScalarInteractionWidget::infiniteSliderMouseMoved(QPointF delta)
{
	double scale = mData->getValueRange().range() / 2.0;
	//  double scale = M_PI_2;
	double factor = scale * delta.x();
	double current = mData->getValue();
	mData->setValue(current + factor);
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
	if (mInfiniteSlider)
		topLayout->addWidget(mInfiniteSlider, 1);
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

	int col = 0;
	if (mLabel)
		hackLayout->addWidget(mLabel);
	hackLayout->addWidget(this);
	gridLayout->addLayout(hackLayout, row, col++);

	if (mEdit)
		gridLayout->addWidget(mEdit, row, col++);
	if (mSpinBox)
		gridLayout->addWidget(mSpinBox, row, col++);
	if (mSlider)
	{
		gridLayout->setColumnStretch(col, 1);
		gridLayout->addWidget(mSlider, row, col++);
	}
	if (mInfiniteSlider)
	{
		gridLayout->setColumnStretch(col, 1);
		gridLayout->addWidget(mInfiniteSlider, row, col++);
	}
}

void ScalarInteractionWidget::build(QGridLayout* gridLayout, int row)
{
	if (gridLayout)
		this->addToGridLayout(gridLayout, row);
	else
		this->addToOwnLayout();

    this->setModified();
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

	double defVal = mData->convertInternal2Display(mData->getValue()); // defval in display space
	double newVal = mData->convertDisplay2Internal(mEdit->getDoubleValue(defVal)); // newval iin internal space

	if (ssc::similar(newVal, mData->getValue()))
		return;

	mData->setValue(newVal);
}

void ScalarInteractionWidget::prePaintEvent()
{
//    std::cout << "ScalarInteractionWidget::prePaintEvent() " << this << " " << mData->getValueName() << std::endl;
	DoubleRange range = mData->getValueRange();
	DoubleRange dRange(mData->convertInternal2Display(range.min()), mData->convertInternal2Display(range.max()),
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
	size.setWidth(size.height() * 3);
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

SliderGroupWidget::SliderGroupWidget(QWidget* parent, ssc::DoubleDataAdapterPtr dataInterface, QGridLayout* gridLayout,
	int row) :
	ScalarInteractionWidget(parent, dataInterface)
{
	this->enableLabel();
	this->enableSlider();
	this->enableEdit();
	//  this->enableSpinBox(true);

	this->build(gridLayout, row);
}

// --------------------------------------------------------
// --------------------------------------------------------

SpinBoxGroupWidget::SpinBoxGroupWidget(QWidget* parent, ssc::DoubleDataAdapterPtr dataInterface,
	QGridLayout* gridLayout, int row) :
	ScalarInteractionWidget(parent, dataInterface)
{
	this->enableLabel();
	this->enableSpinBox();

	this->build(gridLayout, row);
}

// --------------------------------------------------------

SpinBoxAndSliderGroupWidget::SpinBoxAndSliderGroupWidget(QWidget* parent, ssc::DoubleDataAdapterPtr dataInterface,
	QGridLayout* gridLayout, int row) :
	ScalarInteractionWidget(parent, dataInterface)
{
	this->enableLabel();
	this->enableSpinBox();
	this->enableSlider();

	this->build(gridLayout, row);
}

// --------------------------------------------------------

SpinBoxInfiniteSliderGroupWidget::SpinBoxInfiniteSliderGroupWidget(QWidget* parent,
	ssc::DoubleDataAdapterPtr dataInterface, QGridLayout* gridLayout, int row) :
	ScalarInteractionWidget(parent, dataInterface)
{
	this->enableLabel();
	this->enableSpinBox();
	this->enableInfiniteSlider();

	this->build(gridLayout, row);
}

} // namespace ssc
