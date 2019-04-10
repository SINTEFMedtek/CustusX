/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxImage.h"
#include "cxMesh.h"
#include "cxRegServices.h"

namespace cx {

TrainingWidget::TrainingWidget(RegServicesPtr services, QString objectName, QString windowTitle, QWidget* parent) :
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
										"Import new training dataset", "Clear current data and import new training patient folder",
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
//	CX_LOG_DEBUG() << "Creating " << numberOfSteps << " steps!";
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
//	std::cout << "toWelcomeStep" << std::endl;
}

void TrainingWidget::onImportSimulatedPatient()
{
	triggerMainWindowActionWithObjectName("ClearPatient");
	triggerMainWindowActionWithObjectName("LoadFileCopy");

	//Transition to first step after welcome
	this->stepTo(1);
}

void TrainingWidget::onStep(int delta)
{
	this->stepTo(mCurrentStep+delta);
}

void TrainingWidget::stepTo(int step)
{
//	CX_LOG_DEBUG() << "stepTo " << step;
	step = std::min<int>(step, mSessionIDs.size()-1);
	step = std::max<int>(step, 0);
	mCurrentStep = step;
//    CX_LOG_DEBUG() << "Current step is now " << mCurrentStep;
//	CX_LOG_DEBUG() << "mSessionIDs.size(): " << mSessionIDs.size();

	mBrowser->showHelpForKeyword(mSessionIDs[mCurrentStep]);

	this->transitionToStep(step);
}

void TrainingWidget::transitionToStep(int step)
{
//	CX_LOG_DEBUG() << "Want to transition to step " << step;
	int transitionNumber = step;
	if(transitionNumber >= 0)
	{
//		CX_LOG_DEBUG() << "Going to execute transition number " << transitionNumber;
		func_t transition = mTransitions.at(transitionNumber);
		if(transition)
		{
//			CX_LOG_DEBUG() << "Transitioning";
			transition();
		}
	}

}

QString TrainingWidget::getFirstUSVolume()
{
	std::map<QString, DataPtr> datas = mServices->patient()->getDatas();
	std::map<QString, DataPtr>::iterator iter = datas.begin();

	for(; iter != datas.end(); ++iter)
	{
		DataPtr data = iter->second;
		ImagePtr image = boost::dynamic_pointer_cast<Image>(data);

		if (image && image->getModality() == imUS)
			return image->getUid();
	}
	return QString();
}

MeshPtr TrainingWidget::getMesh(QString uidPart)
{
	std::map<QString, MeshPtr> datas = mServices->patient()->getDataOfType<Mesh>();
	std::map<QString, MeshPtr>::iterator iter = datas.begin();
	for(; iter != datas.end(); ++iter)
	{
		MeshPtr mesh = iter->second;
		if(mesh && mesh->getUid().contains(uidPart))
			return mesh;
	}
	return MeshPtr();
}

void TrainingWidget::makeUnavailable(IMAGE_MODALITY modality, bool makeModalityUnavailable)
{
	std::map<QString, DataPtr> datas = mServices->patient()->getDatas();
	this->setAvailability(datas, false, modality, makeModalityUnavailable);
}


void TrainingWidget::makeAvailable(IMAGE_MODALITY modality, bool makeModalityUnavailable)
{
	std::map<QString, DataPtr> datas = mServices->patient()->getDatas(PatientModelService::AllData);
	this->setAvailability(datas, true, modality, makeModalityUnavailable);
}

void TrainingWidget::setAvailability(std::map<QString, DataPtr> datas, bool available, IMAGE_MODALITY modality, bool makeModalityUnavailable)
{
	std::map<QString, DataPtr>::iterator iter = datas.begin();

	for(; iter != datas.end(); ++iter)
	{
		DataPtr data = iter->second;
		ImagePtr image = boost::dynamic_pointer_cast<Image>(data);

		if (makeModalityUnavailable && image && image->getModality() == modality)
			mServices->patient()->makeAvailable(image->getUid(), available);
//		else if (data && data->getUid().contains(uidPart))
//			mServices->patient()->makeAvailable(data->getUid(), available);
	}
}

} /* namespace cx */
