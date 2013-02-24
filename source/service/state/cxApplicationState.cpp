// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxApplicationState.h"

#include <iostream>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QAction>
#include "sscTypeConversions.h"
#include "cxRequestEnterStateTransition.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "cxStateService.h"
#include "cxPatientData.h"
#include "cxPatientService.h"
#include "cxToolManager.h"
#include "cxWorkflowStateMachine.h"

namespace cx
{

ApplicationState::ApplicationState(QState* parent, QString uid, QString name) :
				QState(parent), mUid(uid), mName(name), mAction(NULL), mActive(false)
{
}
;

ApplicationState::~ApplicationState()
{
}

void ApplicationState::onEntry(QEvent * event)
{
	mActive = true;
	ssc::messageManager()->sendInfo("Application change to [" + mName + "]");
	if (mAction)
		mAction->setChecked(true);

	ssc::dataManager()->setClinicalApplication(this->getClinicalApplication());
	if (stateService()->getWorkflow())
		stateService()->getWorkflow()->setActiveState("PatientDataUid");
	patientService()->getPatientData()->clearPatient();

	cx::ToolManager::getInstance()->setClinicalApplication(this->getClinicalApplication());

}

void ApplicationState::onExit(QEvent * event)
{
	mActive = false;
}

QString ApplicationState::getUid() const
{
	return mUid;
}

QString ApplicationState::getName() const
{
	return mName;
}

std::vector<ApplicationState*> ApplicationState::getChildStates()
{
	QObjectList childrenList = this->children();
	std::vector<ApplicationState*> retval;
	for (int i = 0; i < childrenList.size(); ++i)
	{
		ApplicationState* state = dynamic_cast<ApplicationState*>(childrenList[i]);
		if (state)
			retval.push_back(state);
	}
	return retval;
}

QAction* ApplicationState::createAction(QActionGroup* group)
{
	if (mAction)
		return mAction;

	mAction = new QAction(this->getName(), group);
	mAction->setCheckable(true);
	mAction->setChecked(mActive);
	mAction->setData(QVariant(this->getUid()));

	connect(mAction, SIGNAL(triggered()), this, SLOT(setActionSlot()));

	return mAction;
}

void ApplicationState::setActionSlot()
{
	this->machine()->postEvent(new RequestEnterStateEvent(this->getUid()));
}

} // namespace cx

