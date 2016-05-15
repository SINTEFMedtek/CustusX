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
