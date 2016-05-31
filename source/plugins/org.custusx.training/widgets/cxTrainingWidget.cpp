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

TrainingWidget::TrainingWidget(VisServicesPtr services, QString objectName, QString windowTitle, QWidget* parent) :
	BaseWidget(parent, objectName, windowTitle),
	mServices(services)
{
	mEngine.reset(new HelpEngine);
	mBrowser = new HelpBrowser(this, mEngine);

	this->createActions();

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	QHBoxLayout* buttonLayout = new QHBoxLayout;

	topLayout->addWidget(mBrowser);
	topLayout->addLayout(buttonLayout);

	buttonLayout->addStretch(1);
	this->addToolButtonFor(buttonLayout, mImportAction);
	this->addToolButtonFor(buttonLayout, mPreviousAction);
	CXToolButton* button = this->addToolButtonFor(buttonLayout, mCurrentAction);
	button->setToolButtonStyle(Qt::ToolButtonIconOnly);

	this->addToolButtonFor(buttonLayout, mNextAction);

	connect(mImportAction, &QAction::triggered, this, &TrainingWidget::onImportSimulatedPatient);
	connect(mPreviousAction, &QAction::triggered, boost::function<void()>(boost::bind(&TrainingWidget::onStep, this, -1)));
	connect(mCurrentAction, &QAction::triggered, boost::function<void()>(boost::bind(&TrainingWidget::onStep, this, 0)));
	connect(mNextAction, &QAction::triggered, boost::function<void()>(boost::bind(&TrainingWidget::onStep, this, +1)));

	//must always be the initial step
	func_t welcome = boost::bind(&TrainingWidget::toWelcomeStep, this);
	TrainingWidget::registrateTransition(welcome);
}

TrainingWidget::~TrainingWidget()
{
}

void TrainingWidget::resetSteps()
{
	mCurrentStep = -1;
	this->stepTo(0);
}

void TrainingWidget::registrateTransition(func_t transition)
{
	mTransitions.push_back(transition);
	int numberOfSteps = mTransitions.size();
	this->createSteps(numberOfSteps);
}

void TrainingWidget::createActions()
{
	mImportAction = this->createAction2(this,
										QIcon(":/icons/open_icon_library/document-open-7.png"),
										"Import new training dataset", "Import new training patient folder",
										//SLOT(onImport()),
										NULL);

	mPreviousAction = this->createAction2(this,
										  QIcon(":/icons/open_icon_library/arrow-left-3.png"),
										  "Previous", "Go to previous training step",
										  //											SLOT(onPrevious()),
										  NULL);

	mCurrentAction = this->createAction2(this,
										 QIcon(":/icons/open_icon_library/button-green.png"),
										 "Reload", "Reload the current training step",
										 NULL);

	mNextAction = this->createAction2(this,
									  QIcon(":/icons/open_icon_library/arrow-right-3.png"),
									  "Next", "Go to next training step",
									  //											SLOT(onNext()),
									  NULL);
}

void TrainingWidget::createSteps(unsigned numberOfSteps)
{
	CX_LOG_DEBUG() << "Creating " << numberOfSteps << " steps!";
	mSessionIDs.clear();

	for (unsigned i=1; i<=numberOfSteps; ++i)
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

void TrainingWidget::toWelcomeStep()
{
	std::cout << "toWelcomeStep" << std::endl;
}

void TrainingWidget::onImportSimulatedPatient()
{
	triggerMainWindowActionWithObjectName("LoadFile");

	//Transition to first step after welcome
	this->stepTo(1);
}

void TrainingWidget::onStep(int delta)
{
	this->stepTo(mCurrentStep+delta);
}

void TrainingWidget::stepTo(int step)
{
	CX_LOG_DEBUG() << "stepTo " << step;
	step = std::min<int>(step, mSessionIDs.size()-1);
	step = std::max<int>(step, 0);
	mCurrentStep = step;
    CX_LOG_DEBUG() << "Current step is now " << mCurrentStep;
	CX_LOG_DEBUG() << "mSessionIDs.size(): " << mSessionIDs.size();

	mBrowser->showHelpForKeyword(mSessionIDs[mCurrentStep]);

	this->transitionToStep(step);
}

void TrainingWidget::transitionToStep(int step)
{
	CX_LOG_DEBUG() << "Want to transition to step " << step;
	int transitionNumber = step;
	if(transitionNumber >= 0)
	{
		CX_LOG_DEBUG() << "Going to execute transition number " << transitionNumber;
		func_t transition = mTransitions.at(transitionNumber);
		if(transition)
		{
			CX_LOG_DEBUG() << "Transitioning";
			transition();
		}
	}

}

QString TrainingWidget::getFirstUSVolume()
{
	std::map<QString, DataPtr> datas = mServices->patient()->getData();
	std::map<QString, DataPtr>::iterator iter = datas.begin();

	for(; iter != datas.end(); ++iter)
	{
		DataPtr data = iter->second;
		ImagePtr image = boost::dynamic_pointer_cast<Image>(data);

		if (image && image->getModality().contains("US"))
			return image->getUid();
	}
	return QString();
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
