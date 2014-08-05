/*
 * cxDoubleSpanSlider.cpp
 *
 *  \date Aug 26, 2010
 *      \author christiana
 */
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

SliderRangeGroupWidget::SliderRangeGroupWidget(QWidget* parent, DoublePairDataAdapterPtr dataInterface, QGridLayout* gridLayout, int row) : OptimizedUpdateWidget(parent)
{
	mData = dataInterface;
	init(gridLayout, row);
	connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));
}

void SliderRangeGroupWidget::init(QGridLayout *gridLayout, int row)
{
	mLabel = new QLabel(this);
	mLabel->setText(mData->getValueName());
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
}

void SliderRangeGroupWidget::setRange(const DoubleRange& range)
{
  mSpanSlider->setDoubleRange(range);

  mLowerEdit->setRange(range.min(), range.max());
	mLowerEdit->setSingleStep(range.step());
  mUpperEdit->setRange(range.min(), range.max());
  mUpperEdit->setSingleStep(range.step());
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
	if (val == mData->getValue()) // Using Eigen operator== instead if creating similar()
		return false;

	mData->setValue(val);
	dataChanged();
	return true;
}

void SliderRangeGroupWidget::textEditedSlot()
{
	this->setValue(mLowerEdit->value(), mUpperEdit->value());
}

void SliderRangeGroupWidget::dataChanged()
{
  mSpanSlider->blockSignals(true);
  mLowerEdit->blockSignals(true);
  mUpperEdit->blockSignals(true);

	mSpanSlider->setDoubleLowerValue(mData->getValue()[0]);
	mSpanSlider->setDoubleUpperValue(mData->getValue()[1]);
	mLowerEdit->setValue(mData->getValue()[0]);
	mUpperEdit->setValue(mData->getValue()[1]);

  mSpanSlider->blockSignals(false);
  mLowerEdit->blockSignals(false);
  mUpperEdit->blockSignals(false);

	emit valueChanged(mData->getValue()[0], mData->getValue()[1]);
}



} // namespace cx
