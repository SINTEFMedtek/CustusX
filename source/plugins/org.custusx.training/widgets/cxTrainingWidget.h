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

#ifndef CXTRAININGWIDGET_H_
#define CXTRAININGWIDGET_H_

#include "cxBaseWidget.h"
#include "boost/shared_ptr.hpp"
#include "org_custusx_training_Export.h"
#include "cxForwardDeclarations.h"
class ctkPluginContext;
class QPushButton;

namespace cx {

typedef boost::shared_ptr<class HelpEngine> HelpEnginePtr;
class HelpBrowser;


/**
 * Top-level help widget
 *
 * \ingroup org_custusx_training
 *
 * \date 2016-03-14
 * \author Christian Askeland
 */
class org_custusx_training_EXPORT TrainingWidget : public BaseWidget
{
	Q_OBJECT

public:
	explicit TrainingWidget(VisServicesPtr services, QWidget* parent = NULL);
	virtual ~TrainingWidget();

	HelpEnginePtr mEngine;

protected:
	void hideUSData();
	VisServicesPtr mServices;

protected slots:
	virtual void onImport();
private:
	CXToolButton *addToolButtonFor(QHBoxLayout *layout, QAction *action);
	void stepTo(int step);
	void onStep(int delta);

	QAction* mPreviousAction;
	QAction* mNextAction;
	QAction* mCurrentAction;
	QAction* mImportAction;
	HelpBrowser* mBrowser;
	QStringList mSessionIDs;
	int mCurrentStep;
	void resetSteps();
};

} /* namespace cx */
#endif /* CXTRAININGWIDGET_H_ */
