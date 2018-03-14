/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <cxBoundingBoxWidget.h>
#include "cxBoundingBox3D.h"
#include "cxDoubleSpanSlider.h"
#include "cxDoublePairProperty.h"

namespace cx
{

BoundingBoxWidget::BoundingBoxWidget(QWidget* parent, QStringList captions) :
				QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);

	if(captions.isEmpty())
		captions = QStringList() << "X (mm)" << "Y (mm)" << "Z (mm)";

	for (int i=0; i<captions.size(); ++i)
	{
		DoublePairPropertyPtr property = DoublePairProperty::initialize(captions[i], captions[i], captions[i], DoubleRange(-2000, 2000, 1), 0);
		mRange.push_back(new SliderRangeGroupWidget(this, property));
		connect(mRange[i], &SliderRangeGroupWidget::valueChanged, this, &BoundingBoxWidget::changed);
		layout->addWidget(mRange[i]);
		if(i >= captions.size())
			mRange[i]->setVisible(false);
	}
}

void BoundingBoxWidget::showDim(int dim, bool visible)
{
	mRange[dim]->setVisible(visible);
}

void BoundingBoxWidget::setValue(const DoubleBoundingBox3D& value, const DoubleBoundingBox3D& range)
{
	for (int i=0; i<mRange.size(); ++i)
	{
		mRange[i]->blockSignals(true);
		mRange[i]->setRange(DoubleRange(range.begin()[2*i], range.begin()[2*i+1], 1));
		mRange[i]->setValue(value.begin()[2*i], value.begin()[2*i+1]);
		mRange[i]->blockSignals(false);
	}
}

DoubleBoundingBox3D BoundingBoxWidget::getValue() const
{
	//Init with zeroes
	std::vector< std::pair<double, double> > values;
	for(unsigned i = 0; i < 3; ++i)
		values.push_back(std::make_pair(0, 0));

	//Overwrite the zeroes with values (depends on the dim we are using)
	for(unsigned i = 0; i < mRange.size(); ++i)
		values[i] = mRange[i]->getValue();

	DoubleBoundingBox3D box(
				values[0].first, values[0].second,
			values[1].first, values[1].second,
			values[2].first, values[2].second);

//	std::pair<double, double> x = mRange[0]->getValue();
//	std::pair<double, double> y = mRange[1]->getValue();
//	std::pair<double, double> z = mRange[2]->getValue();
//	DoubleBoundingBox3D box(x.first, x.second, y.first, y.second, z.first, z.second);

	return box;
}

}
