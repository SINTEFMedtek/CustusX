// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include <cxBoundingBoxWidget.h>
#include "sscBoundingBox3D.h"

namespace cx
{


BoundingBoxWidget::BoundingBoxWidget(QWidget* parent) : QWidget(parent)
{
	  QVBoxLayout* layout = new QVBoxLayout(this);

	  mXRange = new SliderRangeGroupWidget(this);
	  mXRange->setName("X");
	  mXRange->setRange(ssc::DoubleRange(-2000, 2000, 1));
	  connect(mXRange, SIGNAL(valueChanged(double,double)), this, SIGNAL(changed()));
	  layout->addWidget(mXRange);

	  mYRange = new SliderRangeGroupWidget(this);
	  mYRange->setName("Y");
	  mYRange->setRange(ssc::DoubleRange(-2000, 2000, 1));
	  connect(mYRange, SIGNAL(valueChanged(double,double)), this, SIGNAL(changed()));
	  layout->addWidget(mYRange);

	  mZRange = new SliderRangeGroupWidget(this);
	  mZRange->setName("Z");
	  mZRange->setRange(ssc::DoubleRange(-2000, 2000, 1));
	  connect(mZRange, SIGNAL(valueChanged(double,double)), this, SIGNAL(changed()));
	  layout->addWidget(mZRange);
}

void BoundingBoxWidget::setValue(const ssc::DoubleBoundingBox3D& value, const ssc::DoubleBoundingBox3D& range)
{
  mXRange->blockSignals(true);
  mYRange->blockSignals(true);
  mZRange->blockSignals(true);

  mXRange->setRange(ssc::DoubleRange(range.begin()[0], range.begin()[1], 1));
  mYRange->setRange(ssc::DoubleRange(range.begin()[2], range.begin()[3], 1));
  mZRange->setRange(ssc::DoubleRange(range.begin()[4], range.begin()[5], 1));

  mXRange->setValue(std::make_pair(value.begin()[0], value.begin()[1]));
  mYRange->setValue(std::make_pair(value.begin()[2], value.begin()[3]));
  mZRange->setValue(std::make_pair(value.begin()[4], value.begin()[5]));

  mXRange->blockSignals(false);
  mYRange->blockSignals(false);
  mZRange->blockSignals(false);
}

ssc::DoubleBoundingBox3D BoundingBoxWidget::getValue() const
{
  std::pair<double,double> x = mXRange->getValue();
  std::pair<double,double> y = mYRange->getValue();
  std::pair<double,double> z = mZRange->getValue();
  ssc::DoubleBoundingBox3D box(x.first, x.second, y.first, y.second, z.first, z.second);
  return box;
}

}
