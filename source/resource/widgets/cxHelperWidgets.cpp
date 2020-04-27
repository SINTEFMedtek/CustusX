/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxFilePreviewProperty.h"
#include "cxFilePreviewWidget.h"
#include "cxLogger.h"

namespace cx
{
QWidget* createDataWidget(ViewServicePtr viewService, PatientModelServicePtr patientModelService, QWidget* parent, PropertyPtr data, QGridLayout* gridLayout, int row)
{
	QWidget* retval = NULL;

	//make cx widgets
	SelectDataStringPropertyBasePtr dsda = boost::dynamic_pointer_cast<SelectDataStringPropertyBase>(data);
	if (dsda)
	{
		retval = new DataSelectWidget(viewService, patientModelService, parent, dsda, gridLayout, row);
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

	FilePreviewPropertyPtr filePreview = boost::dynamic_pointer_cast<FilePreviewProperty>(data);
	if (filePreview)
	{
		return new FilePreviewWidget(parent, filePreview, gridLayout, row);
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
