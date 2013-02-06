#include "cxDataAdapterHelper.h"

#include "sscHelperWidgets.h"
#include "sscLabeledLineEditWidget.h"
#include "cxDataInterface.h"
#include "cxDataSelectWidget.h"

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
