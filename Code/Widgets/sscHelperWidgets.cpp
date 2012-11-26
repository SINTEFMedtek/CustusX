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
 * sscHelperWidgets.cpp
 *
 *  Created on: May 18, 2010
 *      Author: christiana
 */
#include "sscHelperWidgets.h"

#include "sscDoubleWidgets.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscCheckBoxWidget.h"
#include "sscColorSelectWidget.h"
#include "sscTypeConversions.h"
#include <iostream>

namespace ssc
{

QWidget* createDataWidget(QWidget* parent, DataAdapterPtr data, QGridLayout* gridLayout, int row)
{
	StringDataAdapterPtr str = boost::shared_dynamic_cast<ssc::StringDataAdapter>(data);
	if (str)
	{
		return new ssc::LabeledComboBoxWidget(parent, str, gridLayout, row);
	}

	DoubleDataAdapterPtr dbl = boost::shared_dynamic_cast<ssc::DoubleDataAdapter>(data);
	if (dbl)
	{
        if (dbl->addSlider())
            return new ssc::SpinBoxAndSliderGroupWidget(parent, dbl, gridLayout, row);
        else
            return new ssc::SpinBoxGroupWidget(parent, dbl, gridLayout, row);
	}

	BoolDataAdapterPtr bl = boost::shared_dynamic_cast<ssc::BoolDataAdapter>(data);
	if (bl)
	{
		return new ssc::CheckBoxWidget(parent, bl, gridLayout, row);
	}

    ColorDataAdapterPtr cl = boost::shared_dynamic_cast<ssc::ColorDataAdapter>(data);
    if (cl)
    {
        return new ssc::ColorSelectWidget(parent, cl, gridLayout, row);
    }

    std::cout << "Failed to create Data Widget for " << (data ? data->getValueName() : "NULL") << std::endl;

    return NULL;
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
