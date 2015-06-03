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
#include "cxFilenameWidget.h"
#include "cxLabeledLineEditWidget.h"
#include "cxDataSelectWidget.h"
#include "cxSelectDataStringProperty.h"
#include "cxFilePathProperty.h"

namespace cx
{

QWidget* createDataWidget(VisualizationServicePtr visualizationService, PatientModelServicePtr patientModelService, QWidget* parent, PropertyPtr data, QGridLayout* gridLayout, int row)
{
	QWidget* retval = NULL;

	//make cx widgets
	SelectDataStringPropertyBasePtr dsda = boost::dynamic_pointer_cast<SelectDataStringPropertyBase>(data);
	if (dsda)
	{
		retval = new DataSelectWidget(visualizationService, patientModelService, parent, dsda, gridLayout, row);
		return retval;
	}
	if(retval != NULL)
		return retval;

	retval = sscCreateDataWidget(parent, data, gridLayout, row);
	return retval;
}

QWidget* sscCreateDataWidget(QWidget* parent, PropertyPtr data, QGridLayout* gridLayout, int row)
{
	FilePathPropertyPtr fp = boost::dynamic_pointer_cast<FilePathProperty>(data);
	if (fp)
	{
		return new FilenameWidget(parent, fp, gridLayout, row);
	}

	StringPropertyBasePtr str = boost::dynamic_pointer_cast<StringPropertyBase>(data);
	if (str)
	{
//		if (str->getGuiRepresentation()==StringPropertyBase::grFILENAME)
//			return new FilenameWidget(parent, str, gridLayout, row);
		if (str->getAllowOnlyValuesInRange())
			return new LabeledComboBoxWidget(parent, str, gridLayout, row);
		else
			return new LabeledLineEditWidget(parent, str, gridLayout, row);
	}

	DoublePropertyBasePtr dbl = boost::dynamic_pointer_cast<DoublePropertyBase>(data);
	if (dbl)
	{
		DoublePropertyBase::GuiRepresentation gui = dbl->getGuiRepresentation();
        switch(gui)
        {
        case DoublePropertyBase::grSLIDER:
            return new SpinBoxAndSliderGroupWidget(parent, dbl, gridLayout, row);
            break;
        case DoublePropertyBase::grSPINBOX:
            return new SpinBoxGroupWidget(parent, dbl, gridLayout, row);
            break;
        case DoublePropertyBase::grDIAL:
        	return new SpinBoxAndDialGroupWidget(parent, dbl, gridLayout, row);
        }
	}

	BoolPropertyBasePtr bl = boost::dynamic_pointer_cast<BoolPropertyBase>(data);
	if (bl)
	{
		return new CheckBoxWidget(parent, bl, gridLayout, row);
	}

	ColorPropertyBasePtr cl = boost::dynamic_pointer_cast<ColorPropertyBase>(data);
	if (cl)
	{
		return new ColorSelectWidget(parent, cl, gridLayout, row);
	}
	DoublePairPropertyBasePtr doublepair = boost::dynamic_pointer_cast<DoublePairPropertyBase>(data);
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
