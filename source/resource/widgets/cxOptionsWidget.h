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
