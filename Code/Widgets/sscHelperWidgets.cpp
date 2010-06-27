/*
 * sscHelperWidgets.cpp
 *
 *  Created on: May 18, 2010
 *      Author: christiana
 */
#include "sscHelperWidgets.h"

#include "sscDoubleWidgets.h"
#include "sscStringWidgets.h"

namespace ssc
{

QWidget* createDataWidget(QWidget* parent, DataAdapterPtr data, QGridLayout* gridLayout, int row)
{
	StringDataAdapterPtr str = boost::shared_dynamic_cast<ssc::StringDataAdapter>(data);
	if (str)
	{
		return new ssc::ComboGroupWidget(parent, str, gridLayout, row);
	}

	DoubleDataAdapterPtr dbl = boost::shared_dynamic_cast<ssc::DoubleDataAdapter>(data);
	if (dbl)
	{
		return new ssc::SliderGroupWidget(parent, dbl, gridLayout, row);
	}

	return NULL;
}


}
