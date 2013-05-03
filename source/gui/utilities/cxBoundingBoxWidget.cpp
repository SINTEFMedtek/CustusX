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
#include "cxDoubleSpanSlider.h"

namespace cx
{

BoundingBoxWidget::BoundingBoxWidget(QWidget* parent) :
				QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);

	QStringList caption;
	caption << "X" << "Y" << "Z";

	for (int i=0; i<caption.size(); ++i)
	{
		mRange[i] = new SliderRangeGroupWidget(this);
		mRange[i]->setName(caption[i]);
		mRange[i]->setRange(ssc::DoubleRange(-2000, 2000, 1));
		mRange[i]->setDecimals(0);
		connect(mRange[i], SIGNAL(valueChanged(double,double)), this, SIGNAL(changed()));
		layout->addWidget(mRange[i]);
	}
}

void BoundingBoxWidget::showDim(int dim, bool visible)
{
	mRange[dim]->setVisible(visible);
}

void BoundingBoxWidget::setValue(const ssc::DoubleBoundingBox3D& value, const ssc::DoubleBoundingBox3D& range)
{
	for (int i=0; i<3; ++i)
	{
		mRange[i]->blockSignals(true);
		mRange[i]->setRange(ssc::DoubleRange(range.begin()[2*i], range.begin()[2*i+1], 1));
		mRange[i]->setValue(std::make_pair(  value.begin()[2*i], value.begin()[2*i+1]));
		mRange[i]->blockSignals(false);
	}
}

ssc::DoubleBoundingBox3D BoundingBoxWidget::getValue() const
{
	std::pair<double, double> x = mRange[0]->getValue();
	std::pair<double, double> y = mRange[1]->getValue();
	std::pair<double, double> z = mRange[2]->getValue();
	ssc::DoubleBoundingBox3D box(x.first, x.second, y.first, y.second, z.first, z.second);
	return box;
}

}
