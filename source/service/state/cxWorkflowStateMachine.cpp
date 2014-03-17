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

#include "cxWorkflowStateMachine.h"
#include <QAbstractTransition>
#include <QMenu>
#include <QToolBar>
#include "cxWorkflowState.h"
#include "cxRequestEnterStateTransition.h"
#include "cxStateServiceBackend.h"
#include "cxDataManager.h"

namespace cx
{

WorkflowStateMachine::WorkflowStateMachine(StateServiceBackendPtr backend) : mBackend(backend)
{
	mStarted = false;
	connect(this, SIGNAL(started()), this, SLOT(startedSlot()));
	mActionGroup = new QActionGroup(this);

	mParentState = new ParentWorkflowState(this, mBackend);

	WorkflowState* patientData = this->newState(new PatientDataWorkflowState(mParentState, mBackend));
	this->newState(new RegistrationWorkflowState(mParentState, mBackend));
	this->newState(new PreOpPlanningWorkflowState(mParentState, mBackend));
	this->newState(new NavigationWorkflowState(mParentState, mBackend));
	this->newState(new IntraOpImagingWorkflowState(mParentState, mBackend));
	this->newState(new PostOpControllWorkflowState(mParentState, mBackend));

	//set initial state on all levels
	this->setInitialState(mParentState);
	mParentState->setInitialState(patientData);

	connect(mBackend->getDataManager().get(), SIGNAL(clinicalApplicationChanged()), this, SLOT(clinicalApplicationChangedSlot()));
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

WorkflowStateMachine::~WorkflowStateMachine()
{
}

QActionGroup* WorkflowStateMachine::getActionGroup()
{
	mActionGroup->setExclusive(true);
	//TODO rebuild action list when we need dynamic lists. Must rethink memory management then.
	for (WorkflowStateMap::iterator iter = mStates.begin(); iter != mStates.end(); ++iter)
	{
		iter->second->createAction(mActionGroup);
	}

	return mActionGroup;
}

void WorkflowStateMachine::fillMenu(QMenu* menu)
{
	this->fillMenu(menu, mParentState);
}

void WorkflowStateMachine::fillMenu(QMenu* menu, WorkflowState* current)
{
	std::vector<WorkflowState*> childStates = current->getChildStates();

	if (childStates.empty())
	{
		menu->addAction(current->createAction(mActionGroup));
	}
	else // current is a node. create submenu and fill in recursively
	{
		QMenu* submenu = menu;
		if (current != mParentState) // ignore creation of submenu for parent state
			submenu = menu->addMenu(current->getName());
		for (unsigned i = 0; i < childStates.size(); ++i)
			this->fillMenu(submenu, childStates[i]);
	}
}

void WorkflowStateMachine::setActiveState(QString uid)
{

	if (mStarted)
		this->postEvent(new RequestEnterStateEvent(uid));
}

void WorkflowStateMachine::fillToolBar(QToolBar* toolbar)
{
	this->fillToolbar(toolbar, mParentState);
}

void WorkflowStateMachine::fillToolbar(QToolBar* toolbar, WorkflowState* current)
{
	std::vector<WorkflowState*> childStates = current->getChildStates();

	if (childStates.empty())
	{
		toolbar->addAction(current->createAction(mActionGroup));
	}
	else // current is a node. fill in recursively
	{
		for (unsigned i = 0; i < childStates.size(); ++i)
			this->fillToolbar(toolbar, childStates[i]);
	}
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
