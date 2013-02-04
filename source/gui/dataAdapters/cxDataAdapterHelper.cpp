#include "cxDataAdapterHelper.h"

#include "sscHelperWidgets.h"
#include "sscLabeledLineEditWidget.h"
#include "cxDataInterface.h"
#include "cxDataSelectWidget.h"

namespace cx {


QWidget* createDataWidget(QWidget* parent, DataAdapterPtr data, QGridLayout* gridLayout, int row)
{
	QWidget* retval = ssc::createDataWidget(parent, data, gridLayout, row);

	if(retval != NULL)
		return retval;

	//make cx widgets
	SelectDataStringDataAdapterBasePtr dsda = boost::shared_dynamic_cast<SelectDataStringDataAdapterBase>(data);
	if (dsda)
	{
		QWidget* temp = new DataSelectWidget(parent, dsda);
		gridLayout->addWidget(temp, row, 0, 1, 2);
		return temp;
	}

}

} /* namespace cx */
