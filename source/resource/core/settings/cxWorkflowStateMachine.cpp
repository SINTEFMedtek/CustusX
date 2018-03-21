/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxWorkflowStateMachine.h"
#include <QAbstractTransition>
#include <QMenu>
#include <QToolBar>
#include "cxWorkflowState.h"
#include "cxRequestEnterStateTransition.h"
#include "cxPatientModelService.h"
#include "cxCoreServices.h"

namespace cx
{

WorkflowStateMachine::WorkflowStateMachine(CoreServicesPtr services) : mServices(services)
{
	mStarted = false;
	connect(this, SIGNAL(started()), this, SLOT(startedSlot()));
	mActionGroup = new QActionGroup(this);

	mParentState = new ParentWorkflowState(this, mServices);

	connect(mServices->patient().get(), SIGNAL(clinicalApplicationChanged()), this, SLOT(clinicalApplicationChangedSlot()));
}

WorkflowStateMachine::~WorkflowStateMachine()
{
}

void WorkflowStateMachine::clinicalApplicationChangedSlot()
{
	this->setActiveState("PatientDataUid");
}

void WorkflowStateMachine::startedSlot()
{
	mStarted = true;
}

WorkflowState* WorkflowStateMachine::newState(WorkflowState* state)
{
	RequestEnterStateTransition* transToState = new RequestEnterStateTransition(state->getUid());
	transToState->setTargetState(state);
	mParentState->addTransition(transToState);

	connect(state, SIGNAL(entered()), this, SIGNAL(activeStateChanged()));

	mStates[state->getUid()] = state;

	connect(state, SIGNAL(aboutToExit()), this, SIGNAL(activeStateAboutToChange()));

	return state;
}

QActionGroup* WorkflowStateMachine::getActionGroup()
{
//	mActionGroup
	this->fillActionGroup(mParentState, mActionGroup);

	QList<QAction *> actions = mActionGroup->actions();
	for (int i = 1; i <= actions.size(); ++i)
	{
		QString shortcut = "Ctrl+" + QString::number(i);
		actions[i - 1]->setShortcut(shortcut);
	}

	return mActionGroup;
}

void WorkflowStateMachine::fillActionGroup(WorkflowState* current, QActionGroup* group)
{
	std::vector<WorkflowState*> childStates = current->getChildStates();

	if (childStates.empty())
	{
		group->addAction(current->createAction(group));
	}
	else // current is a node. create submenu and fill in recursively
	{
		for (unsigned i = 0; i < childStates.size(); ++i)
			this->fillActionGroup(childStates[i], group);
	}
}

void WorkflowStateMachine::setActiveState(QString uid)
{

	if (mStarted)
		this->postEvent(new RequestEnterStateEvent(uid));
}

QString WorkflowStateMachine::getActiveUidState()
{
	QSet<QAbstractState *> states = this->configuration();
	for (QSet<QAbstractState *>::iterator iter = states.begin(); iter != states.end(); ++iter)
	{
		WorkflowState* wfs = dynamic_cast<WorkflowState*>(*iter);
		if (!wfs || !wfs->getChildStates().empty())
			continue;

		return wfs->getUid();
	}
	return QString();
}

} //namespace cx
