/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXOPTIONSWIDGET_H_
#define CXOPTIONSWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include <QStackedLayout>
#include "cxProperty.h"
#include "cxSelectDataStringProperty.h"

namespace cx {

/** Widget for displaying a list of Property instances.
 * A stack of widgets is created if the setOptions() is called with different uids.
 *
 * \ingroup cx_resource_widgets
 * \date Nov 20, 2012
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResourceWidgets_EXPORT OptionsWidget : public QWidget
{
	Q_OBJECT
public:
	OptionsWidget(ViewServicePtr viewService, PatientModelServicePtr patientModelService, QWidget* parent);

	/**
	  * Set options for a given uid. Setting different uid will change the widget
	  * content but store the previous contents, making it easy to swap between uids.
	  */
	void setOptions(QString uid, std::vector<PropertyPtr> options, bool showAdvanced);
	void setOptions(QString uid, std::vector<SelectDataStringPropertyBasePtr> options, bool showAdvanced);
	QString getCurrentUid();
	void showAdvanced(bool show); //whether or not to display adapters marked as advanced
	void rebuild();
    bool hasOptions() const;
    bool hasAdvancedOptions() const;

public slots:
	void toggleAdvanced();

private:
	void clear();
	void populate(bool showAdvanced);
	QWidget* createGroupHeaderWidget(QString title);

	QStackedLayout* mStackedLayout;
	std::vector<PropertyPtr> mOptions;
	QString mUid;
	bool mShowAdvanced;
	ViewServicePtr mViewService;
	PatientModelServicePtr mPatientModelService;
};
} /* namespace cx */
#endif /* CXOPTIONSWIDGET_H_ */
