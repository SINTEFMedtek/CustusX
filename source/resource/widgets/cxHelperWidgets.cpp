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


/*
 * sscHelperWidgets.cpp
 *
 *  Created on: May 18, 2010
 *      Author: christiana
 */
#include "cxHelperWidgets.h"

#include <iostream>
#include "boost/shared_ptr.hpp"
#include "cxDoubleWidgets.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxCheckBoxWidget.h"
#include "cxColorSelectWidget.h"
#include "cxTypeConversions.h"
#include "cxLabeledLineEditWidget.h"
#include "cxDoubleSpanSlider.h"

namespace cx
{

QWidget* sscCreateDataWidget(QWidget* parent, DataAdapterPtr data, QGridLayout* gridLayout, int row)
{
	StringDataAdapterPtr str = boost::dynamic_pointer_cast<StringDataAdapter>(data);
	if (str)
	{
		if (str->getAllowOnlyValuesInRange())
			return new LabeledComboBoxWidget(parent, str, gridLayout, row);
		else
			return new LabeledLineEditWidget(parent, str, gridLayout, row);
	}

	DoubleDataAdapterPtr dbl = boost::dynamic_pointer_cast<DoubleDataAdapter>(data);
	if (dbl)
	{
		DoubleDataAdapter::GuiRepresentation gui = dbl->getGuiRepresentation();
        switch(gui)
        {
        case DoubleDataAdapter::grSLIDER:
            return new SpinBoxAndSliderGroupWidget(parent, dbl, gridLayout, row);
            break;
        case DoubleDataAdapter::grSPINBOX:
            return new SpinBoxGroupWidget(parent, dbl, gridLayout, row);
            break;
        case DoubleDataAdapter::grDIAL:
        	return new SpinBoxAndDialGroupWidget(parent, dbl, gridLayout, row);
        }
	}

	BoolDataAdapterPtr bl = boost::dynamic_pointer_cast<BoolDataAdapter>(data);
	if (bl)
	{
		return new CheckBoxWidget(parent, bl, gridLayout, row);
	}

	ColorDataAdapterPtr cl = boost::dynamic_pointer_cast<ColorDataAdapter>(data);
	if (cl)
	{
		return new ColorSelectWidget(parent, cl, gridLayout, row);
	}
	DoublePairDataAdapterPtr doublepair = boost::dynamic_pointer_cast<DoublePairDataAdapter>(data);
	if (doublepair)
	{
		return new SliderRangeGroupWidget(parent, doublepair, gridLayout, row);
	}

	std::cout << "Failed to create Data Widget for " << (data ? data->getDisplayName() : "NULL") << std::endl;

    return NULL;
}

QWidget* addDummyMargin(QWidget* widget)
{
	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(1,0,0,0);
	widget->setLayout(topLayout);
	return widget;
}

QHBoxLayout* mergeWidgetsIntoHBoxLayout(QWidget* first, QWidget* second)
{
    QHBoxLayout* hackLayout = new QHBoxLayout;
    hackLayout->setMargin(0);
    hackLayout->setSpacing(0);

    if (first)
        hackLayout->addWidget(first);
    if (second)
        hackLayout->addWidget(second);

    return hackLayout;
}

}
