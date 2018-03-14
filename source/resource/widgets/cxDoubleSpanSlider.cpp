/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxDoubleSpanSlider.h"
#include <QDoubleSpinBox>
#include "cxHelperWidgets.h"

namespace cx
{

void SliderRangeGroupWidget::addToGridLayout(QGridLayout* gridLayout, int row)
{
  gridLayout->addWidget(mLabel,      row, 0);
  gridLayout->addWidget(mLowerEdit,  row, 1);
  gridLayout->addWidget(mUpperEdit,  row, 1);
  gridLayout->addWidget(mSpanSlider, row, 2);
}

SliderRangeGroupWidget::SliderRangeGroupWidget(QWidget* parent, DoublePairPropertyBasePtr dataInterface, QGridLayout* gridLayout, int row) : OptimizedUpdateWidget(parent)
{
	mData = dataInterface;
	this->init(gridLayout, row);
}

void SliderRangeGroupWidget::init(QGridLayout *gridLayout, int row)
{
	mLabel = new QLabel(this);
	mLabel->setText(mData->getDisplayName());
	mLowerEdit = new QDoubleSpinBox(this);
	mSpanSlider = new DoubleSpanSlider(this);
	mSpanSlider->setOrientation(Qt::Horizontal);
	mSpanSlider->setHandleMovementMode(QxtSpanSlider::NoOverlapping);
	mUpperEdit = new QDoubleSpinBox(this);

	this->setDecimals(mData->getValueDecimals());

	if (gridLayout) // add to input gridlayout
	{
			gridLayout->addLayout(mergeWidgetsIntoHBoxLayout(mLabel, addDummyMargin(this)), row, 0);

			QHBoxLayout* controlsLayout = new QHBoxLayout;
			controlsLayout->setSpacing(0);
			controlsLayout->setMargin(0);
			gridLayout->addLayout(controlsLayout, row, 1);

			controlsLayout->addWidget(mLowerEdit);
			controlsLayout->addWidget(mSpanSlider, 1);
			controlsLayout->addWidget(mUpperEdit);
	}
	else // add directly to this
	{
		QHBoxLayout* topLayout = new QHBoxLayout;
		topLayout->setMargin(0);
		this->setLayout(topLayout);

		topLayout->addWidget(mLabel);
		topLayout->addWidget(mLowerEdit);
		topLayout->addWidget(mSpanSlider, 1);
		topLayout->addWidget(mUpperEdit);
	}

	connect(mLowerEdit, SIGNAL(valueChanged(double)), this, SLOT(textEditedSlot()));
	// connect to slider
	connect(mSpanSlider, SIGNAL(doubleSpanChanged(double, double)), this, SLOT(doubleSpanChangedSlot(double, double)));
	connect(mUpperEdit, SIGNAL(valueChanged(double)), this, SLOT(textEditedSlot()));

	// connect to backend
	connect(mData.get(), SIGNAL(changed()), this, SLOT(dataChanged()));
	this->dataChanged();
}

void SliderRangeGroupWidget::setRange(const DoubleRange& range)
{
	mData->setValueRange(range);
	this->updateGuiRange();
}

void SliderRangeGroupWidget::setDecimals(int decimals)
{
	mLowerEdit->setDecimals(decimals);
	mUpperEdit->setDecimals(decimals);
}

std::pair<double,double> SliderRangeGroupWidget::getValue() const
{
	return std::make_pair(mData->getValue()[0], mData->getValue()[1]);
}

void SliderRangeGroupWidget::doubleSpanChangedSlot(double lower, double upper)
{
	this->setValue(lower, upper);
}

bool SliderRangeGroupWidget::setValue(double lower, double upper)
{
	Eigen::Vector2d val = Eigen::Vector2d(lower, upper);
	if (val == mData->getValue())
		return false;

	mData->setValue(val);
	return true;
}

void SliderRangeGroupWidget::textEditedSlot()
{
	this->setValue(mLowerEdit->value(), mUpperEdit->value());
}

void SliderRangeGroupWidget::dataChanged()
{
	this->updateGuiRange();

	mSpanSlider->blockSignals(true);
	mLowerEdit->blockSignals(true);
	mUpperEdit->blockSignals(true);

	mSpanSlider->setDoubleSpan(mData->getValue()[0], mData->getValue()[1]);
	mLowerEdit->setValue(mData->getValue()[0]);
	mUpperEdit->setValue(mData->getValue()[1]);

	mSpanSlider->blockSignals(false);
	mLowerEdit->blockSignals(false);
	mUpperEdit->blockSignals(false);

	emit valueChanged(mData->getValue()[0], mData->getValue()[1]);
	this->setModified();
}

void SliderRangeGroupWidget::updateGuiRange()
{
	DoubleRange range = mData->getValueRange();

	mSpanSlider->setDoubleRange(range);

	mLowerEdit->setRange(range.min(), range.max());
	mLowerEdit->setSingleStep(range.step());

	mUpperEdit->setRange(range.min(), range.max());
	mUpperEdit->setSingleStep(range.step());
}


} // namespace cx
