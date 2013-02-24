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
#include "cxDataAdapterHelper.h"

#include "sscHelperWidgets.h"
#include "sscLabeledLineEditWidget.h"
#include "cxDataInterface.h"
#include "cxDataSelectWidget.h"
#include "cxSelectDataStringDataAdapter.h"

namespace cx {


QWidget* createDataWidget(QWidget* parent, DataAdapterPtr data, QGridLayout* gridLayout, int row)
{
	QWidget* retval = NULL;

	//make cx widgets
	SelectDataStringDataAdapterBasePtr dsda = boost::shared_dynamic_cast<SelectDataStringDataAdapterBase>(data);
	if (dsda)
	{
		retval = new DataSelectWidget(parent, dsda);
		gridLayout->addWidget(retval, row, 0, 1, 2);
		return retval;
	}
	if(retval != NULL)
		return retval;

	retval = ssc::createDataWidget(parent, data, gridLayout, row);
	return retval;
}

} /* namespace cx */
