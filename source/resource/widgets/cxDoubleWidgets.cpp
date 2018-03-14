/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxDoubleWidgets.h"

#include <iostream>
#include "cxVector3D.h"
#include "cxTypeConversions.h"
#include "cxMousePadWidget.h"
#include "cxHelperWidgets.h"

namespace cx
{

ScalarInteractionWidget::ScalarInteractionWidget(QWidget* parent, DoublePropertyBasePtr dataInterface) :
    OptimizedUpdateWidget(parent), mSlider(NULL), mDial(NULL), mSpinBox(NULL), mLabel(NULL), mEdit(NULL), mInfiniteSlider(NULL)
{
	mData = dataInterface;
    connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));
    this->enableAll(mData->getEnabled());
}

void ScalarInteractionWidget::enableLabel()
{
	mLabel = new QLabel(this);
	mLabel->setText(mData->getDisplayName());
}

void ScalarInteractionWidget::showLabel(bool on)
{
	if (mLabel)
		mLabel->setVisible(on);
}

void ScalarInteractionWidget::enableSlider()
{
	mSlider = new DoubleSlider(this);
    mSlider->setMinimumWidth(50);
    mSlider->setOrientation(Qt::Horizontal);
    connect(mSlider, SIGNAL(doubleValueChanged(double)), this, SLOT(doubleValueChanged(double)));
}

void ScalarInteractionWidget::enableDial()
{
	mDial = new QDial(this);
	mDial->setMaximumWidth(50);
	mDial->setMaximumHeight(50);
	mDial->setNotchesVisible(true);
    connect(mDial, SIGNAL(valueChanged(int)), this, SLOT(intValueChanged(int)));
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
	mEdit = new DoubleLineEdit(this);
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
	topLayout->setSpacing(0);
	this->setLayout(topLayout);

	if (mLabel)
		topLayout->addWidget(mLabel, 0);
	if (mEdit)
		topLayout->addWidget(mEdit, 0);
	if (mSpinBox)
		topLayout->addWidget(mSpinBox, 0);
	if (mDial)
		topLayout->addWidget(mDial, 0);
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
    gridLayout->addLayout(mergeWidgetsIntoHBoxLayout(mLabel, addDummyMargin(this)), row, 0);

    QHBoxLayout* controlsLayout = new QHBoxLayout;
    controlsLayout->setSpacing(0);
    controlsLayout->setMargin(0);
    gridLayout->addLayout(controlsLayout, row, 1);


    if (mEdit)
        controlsLayout->addWidget(mEdit);
	if (mSpinBox)
        controlsLayout->addWidget(mSpinBox);
	if (mDial)
		gridLayout->addWidget(mDial, row, 2);
	if (mSlider)
        controlsLayout->addWidget(mSlider, 1);
	if (mInfiniteSlider)
        controlsLayout->addWidget(mInfiniteSlider, 1);
}

void ScalarInteractionWidget::build(QGridLayout* gridLayout, int row)
{
	if (gridLayout)
		this->addToGridLayout(gridLayout, row);
	else
		this->addToOwnLayout();

    this->setModified();
}


void ScalarInteractionWidget::intValueChanged(int val)
{
	this->doubleValueChanged(val/100.0);
}


void ScalarInteractionWidget::doubleValueChanged(double val)
{
	val = mData->convertDisplay2Internal(val);

	if (similar(val, mData->getValue()))
		return;

	mData->setValue(val);
}

void ScalarInteractionWidget::textEditedSlot()
{
	if (!mEdit)
		return;

	double defVal = mData->convertInternal2Display(mData->getValue()); // defval in display space
	double newVal = mData->convertDisplay2Internal(mEdit->getDoubleValue(defVal)); // newval iin internal space

	if (similar(newVal, mData->getValue()))
		return;

	mData->setValue(newVal);
}

//void ScalarInteractionWidget::setModified()
//{
//	OptimizedUpdateWidget::setModified();
//    // Problem: If one of the sliders are visible, paint() is not called.
//    // Force repaint here.
//    //
//    // Possible solution: this is obscured by the slider,
//    // but repaint goes to the children. Maybe design is flawed and we need to
//    // listen to the children as well?
////    if (mSlider || mInfiniteSlider)
////        this->prePaintEvent();
//}

void ScalarInteractionWidget::prePaintEvent()
{
	this->enableAll(mData->getEnabled());

//    std::cout << "ScalarInteractionWidget::prePaintEvent() " << this << " " << mData->getDisplayName() << std::endl;
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

	if (mDial)
	{
		mDial->blockSignals(true);
		mDial->setContentsMargins(0,0,0,0);
		mDial->setRange(dRange.min()*100, dRange.max()*100);
		mDial->setSingleStep(dRange.step()*100);
		mDial->setValue(mData->convertInternal2Display(mData->getValue())*100);
		mDial->setToolTip(mData->getHelp());
		mDial->blockSignals(false);
	}

	if (mLabel)
	{
		mLabel->setToolTip(mData->getHelp());
	}
}

void ScalarInteractionWidget::enableAll(bool enable)
{
    QWidget::setEnabled(enable);

    if(mSlider)
		mSlider->setEnabled(enable);
	if(mSpinBox)
		mSpinBox->setEnabled(enable);
	if(mLabel)
		mLabel->setEnabled(enable);
	if(mEdit)
		mEdit->setEnabled(enable);
	if(mInfiniteSlider)
		mInfiniteSlider->setEnabled(enable);
}

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------


QSize DoubleLineEdit::sizeHint() const
{
	QSize size = QLineEdit::minimumSizeHint();
	size.setWidth(size.height() * 3);
	return size;
}

QSize DoubleLineEdit::minimumSizeHint() const
{
	QSize size = QLineEdit::minimumSizeHint();
	return size;
}

// --------------------------------------------------------
// --------------------------------------------------------

SliderGroupWidget::SliderGroupWidget(QWidget* parent, DoublePropertyBasePtr dataInterface, QGridLayout* gridLayout,
	int row) :
	ScalarInteractionWidget(parent, dataInterface)
{
	this->enableLabel();
	this->enableSlider();
	this->enableEdit();

	this->build(gridLayout, row);
}

// --------------------------------------------------------
// --------------------------------------------------------

SpinBoxGroupWidget::SpinBoxGroupWidget(QWidget* parent, DoublePropertyBasePtr dataInterface,
	QGridLayout* gridLayout, int row) :
	ScalarInteractionWidget(parent, dataInterface)
{
	this->enableLabel();
	this->enableSpinBox();

	this->build(gridLayout, row);
}

// --------------------------------------------------------

SpinBoxAndSliderGroupWidget::SpinBoxAndSliderGroupWidget(QWidget* parent, DoublePropertyBasePtr dataInterface,
	QGridLayout* gridLayout, int row) :
	ScalarInteractionWidget(parent, dataInterface)
{
	this->enableLabel();
	this->enableSpinBox();
	this->enableSlider();

	this->build(gridLayout, row);
}
// --------------------------------------------------------
SpinBoxAndDialGroupWidget::SpinBoxAndDialGroupWidget(QWidget* parent, DoublePropertyBasePtr dataInterface,
	QGridLayout* gridLayout, int row) :
	ScalarInteractionWidget(parent, dataInterface)
{
	this->enableLabel();
	this->enableSpinBox();
	this->enableDial();

	this->build(gridLayout, row);
}

// --------------------------------------------------------

SpinBoxInfiniteSliderGroupWidget::SpinBoxInfiniteSliderGroupWidget(QWidget* parent,
	DoublePropertyBasePtr dataInterface, QGridLayout* gridLayout, int row) :
	ScalarInteractionWidget(parent, dataInterface)
{
	this->enableLabel();
	this->enableSpinBox();
	this->enableInfiniteSlider();

	this->build(gridLayout, row);
}

} // namespace cx
