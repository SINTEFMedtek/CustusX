/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
		connect(mRange[i], SIGNAL(valueChanged(double,double)), this, SIGNAL(changed()));
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
	for(unsigned i; i < 3; ++i)
		values.push_back(std::make_pair(0, 0));

	//Overwrite the zeroes with values (depends on the dim we are using)
	for(unsigned i; i < mRange.size(); ++i)
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
