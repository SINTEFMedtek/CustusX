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

#include "cxTrainingWidget.h"
#include <QtWidgets>
#include <QPushButton>
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "cxHelpEngine.h"
#include "cxHelpBrowser.h"
#include "cxLogger.h"
#include "cxApplication.h"
#include "cxPatientModelService.h"
#include "cxVisServices.h"
#include "cxImage.h"

namespace cx {

TrainingWidget::TrainingWidget(VisServicesPtr services, QWidget* parent) :
		BaseWidget(parent, "TrainingWidget", "Training"),
		mServices(services)
{
	mEngine.reset(new HelpEngine);

	mBrowser = new HelpBrowser(this, mEngine);

	QVBoxLayout* topLayout = new QVBoxLayout(this);

	topLayout->addWidget(mBrowser);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	topLayout->addLayout(buttonLayout);


	buttonLayout->addStretch(1);

	mImportAction = this->createAction(this,
											QIcon(":/icons/open_icon_library/document-open-7.png"),
											"Import new training dataset", "Import new training patient folder",
											SLOT(onImport()),
											NULL);
	this->addToolButtonFor(buttonLayout, mImportAction);

	mPreviousAction = this->createAction2(this,
											QIcon(":/icons/open_icon_library/arrow-left-3.png"),
											"Previous", "Go to previous training step",
//											SLOT(onPrevious()),
											NULL);
	connect(mPreviousAction, &QAction::triggered,
			boost::function<void()>(boost::bind(&TrainingWidget::onStep, this, -1)));
	this->addToolButtonFor(buttonLayout, mPreviousAction);

	mCurrentAction = this->createAction2(this,
											QIcon(":/icons/open_icon_library/button-green.png"),
											"Reload", "Reload the current training step",
											NULL);
	connect(mCurrentAction, &QAction::triggered,
			boost::function<void()>(boost::bind(&TrainingWidget::onStep, this, 0)));
	CXToolButton* button = this->addToolButtonFor(buttonLayout, mCurrentAction);
	button->setToolButtonStyle(Qt::ToolButtonIconOnly);

	mNextAction = this->createAction2(this,
											QIcon(":/icons/open_icon_library/arrow-right-3.png"),
											"Next", "Go to next training step",
//											SLOT(onNext()),
											NULL);
	connect(mNextAction, &QAction::triggered,
			boost::function<void()>(boost::bind(&TrainingWidget::onStep, this, +1)));
	this->addToolButtonFor(buttonLayout, mNextAction);

	for (unsigned i=1; i<=3; ++i)
		mSessionIDs << QString("org_custusx_training_sessionA_step%1").arg(i);

	this->resetSteps();
}

CXToolButton* TrainingWidget::addToolButtonFor(QHBoxLayout* layout, QAction* action)
{
	CXToolButton* button = new CXToolButton();
	button->setDefaultAction(action);
	button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
//	button->setToolTip(action->toolTip());
	layout->addWidget(button);
	return button;
}

TrainingWidget::~TrainingWidget()
{
}

void TrainingWidget::resetSteps()
{
	mCurrentStep = -1;
	this->stepTo(0);
}

void TrainingWidget::onStep(int delta)
{
	this->stepTo(mCurrentStep+delta);
}

void TrainingWidget::stepTo(int step)
{
	step = std::min<int>(step, mSessionIDs.size()-1);
	step = std::max<int>(step, 0);
	mCurrentStep = step;

	mBrowser->showHelpForKeyword(mSessionIDs[mCurrentStep]);
}

void TrainingWidget::onImport()
{
	triggerMainWindowActionWithObjectName("LoadFile");

	this->resetSteps();

	this->makeUnavailable("Kaisa");
	this->makeUnavailable("US", true);
}

void TrainingWidget::makeUnavailable(QString uidPart, bool makeModalityUnavailable)
{
	std::map<QString, DataPtr> datas = mServices->patient()->getData();
	std::map<QString, DataPtr>::iterator iter = datas.begin();

	for(; iter != datas.end(); ++iter)
	{
		DataPtr data = iter->second;
		ImagePtr image = boost::dynamic_pointer_cast<Image>(data);

		if (makeModalityUnavailable && image && image->getModality().contains(uidPart))
			mServices->patient()->makeAvailable(image->getUid(), false);
		else if (data && data->getUid().contains(uidPart))
			mServices->patient()->makeAvailable(data->getUid(), false);
	}
}

} /* namespace cx */
