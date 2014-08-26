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

#include "cxApplicationState.h"

#include <iostream>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QAction>
#include "cxTypeConversions.h"
#include "cxRequestEnterStateTransition.h"
#include "cxReporter.h"
#include "cxDataManager.h"
#include "cxStateService.h"
#include "cxPatientData.h"
#include "cxPatientService.h"
#include "cxToolManager.h"
#include "cxWorkflowStateMachine.h"
#include "cxStateServiceBackend.h"

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

void ApplicationState::setBackend(StateServiceBackendPtr backend)
{
	mBackend = backend;
}

void ApplicationState::onEntry(QEvent * event)
{
	mActive = true;
	report("Application change to [" + mName + "]");
	if (mAction)
		mAction->setChecked(true);

	mBackend->getDataManager()->setClinicalApplication(this->getClinicalApplication());
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

