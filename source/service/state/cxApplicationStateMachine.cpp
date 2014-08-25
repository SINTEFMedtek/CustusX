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

/*
 * cxApplicationStateMachine.cpp
 *
 *  \date Aug 17, 2010
 *      \author christiana
 */

#include "cxApplicationStateMachine.h"
#include <QAbstractTransition>
#include <QMenu>
#include <QToolBar>
#include "cxSettings.h"
#include "cxApplicationState.h"
#include "cxRequestEnterStateTransition.h"

namespace cx
{

ApplicationStateMachine::ApplicationStateMachine(StateServiceBackendPtr backend) : mBackend(backend)
{
	mActionGroup = new QActionGroup(this);

	mParentState = new ParentApplicationState(this);

	ApplicationState* laboratory = this->newState(new LaboratoryApplicationState(mParentState));
	ApplicationState* neurology = this->newState(new NeurologyApplicationState(mParentState));
	ApplicationState* Laparoscopy = this->newState(new LaparoscopyApplicationState(mParentState));
	ApplicationState* lung = this->newState(new BronchoscopyApplicationState(mParentState));
	ApplicationState* endovascular = this->newState(new EndovascularApplicationState(mParentState));

	Q_UNUSED(neurology);
	Q_UNUSED(Laparoscopy);
	Q_UNUSED(lung);
	Q_UNUSED(endovascular);

	//set initial state on all levels
	this->setInitialState(mParentState);

	QString initState = settings()->value("globalApplicationName").toString();
	if (mStates.count(initState))
		mParentState->setInitialState(mStates[initState]);
	else
		mParentState->setInitialState(laboratory);
}

ApplicationState* ApplicationStateMachine::newState(ApplicationState* state)
{
	state->setBackend(mBackend);
	RequestEnterStateTransition* transToState = new RequestEnterStateTransition(state->getUid());
	transToState->setTargetState(state);
	mParentState->addTransition(transToState);

	//must call internal slot first! This updates internal state before informing the world.
	connect(state, SIGNAL(entered()), this, SLOT(activeStateChangedSlot()));
	connect(state, SIGNAL(entered()), this, SIGNAL(activeStateChanged()));

	mStates[state->getUid()] = state;
	return state;
}

ApplicationStateMachine::~ApplicationStateMachine()
{
}

void ApplicationStateMachine::activeStateChangedSlot()
{
	settings()->setValue("globalApplicationName", this->getActiveUidState());
}

QActionGroup* ApplicationStateMachine::getActionGroup()
{
	QString active = this->getActiveUidState();
	mActionGroup->setExclusive(true);
	//TODO rebuild action list when we need dynamic lists. Must rethink memory management then.
	for (ApplicationStateMap::iterator iter = mStates.begin(); iter != mStates.end(); ++iter)
	{
		iter->second->createAction(mActionGroup);
	}

	return mActionGroup;
}

QString ApplicationStateMachine::getActiveUidState()
{
	QSet<QAbstractState *> states = this->configuration();
	for (QSet<QAbstractState *>::iterator iter = states.begin(); iter != states.end(); ++iter)
	{
		ApplicationState* wfs = dynamic_cast<ApplicationState*>(*iter);
		if (!wfs || !wfs->getChildStates().empty())
			continue;

		return wfs->getUid();
	}
	return QString();
}

QString ApplicationStateMachine::getActiveStateName()
{
	QString uid = this->getActiveUidState();
	if (!mStates.count(uid))
		return "";
	return mStates[uid]->getName();
}

QStringList ApplicationStateMachine::getAllApplicationNames()
{
	QStringList retval;
	ApplicationStateMap::iterator it = mStates.begin();
	for (; it != mStates.end(); ++it)
		retval << it->second->getName();
	return retval;
}

} //namespace cx
