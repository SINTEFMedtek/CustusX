/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCOMPACTFILTERSETUPWIDGET_H_
#define CXCOMPACTFILTERSETUPWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include "cxBaseWidget.h"
//#include "cxFilterWidget.h"
#include "cxFilter.h"


namespace cx {

class OptionsWidget;
class WidgetObscuredListener;

/**
 * \brief Helper widget for displaying the input/output/options part of a Filter.
 * Intended to be included in other Filter widgets.
 *
 * \ingroup cx_resource_widgets
 * \date Dec 13, 2012
 * \author Christian Askeland, SINTEF
 */
class cxResourceWidgets_EXPORT CompactFilterSetupWidget : public BaseWidget
{
	Q_OBJECT

public:
	CompactFilterSetupWidget(ViewServicePtr viewService, PatientModelServicePtr patientModelService, QWidget* parent, XmlOptionFile options, bool addFrame);
	void setFilter(FilterPtr filter);
	QString getHelpText() const;

private slots:
	void obscuredSlot(bool obscured);

private:
	XmlOptionFile mOptions;
	FilterPtr mCurrentFilter;
	OptionsWidget* mOptionsWidget;
	QGroupBox* mFrame;
	boost::shared_ptr<WidgetObscuredListener> mObscuredListener;
};
} /* namespace cx */
#endif /* CXCOMPACTFILTERSETUPWIDGET_H_ */
