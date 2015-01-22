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

#include "cxStateServiceProxy.h"
#include "boost/bind.hpp"
#include "cxNullDeleter.h"

namespace cx
{

StateServicePtr StateServiceProxy::create(ctkPluginContext *pluginContext)
{
	return StateServicePtr(new StateServiceProxy(pluginContext));
}

StateServiceProxy::StateServiceProxy(ctkPluginContext *pluginContext) :
	mPluginContext(pluginContext),
	mService(StateService::getNullObject())
{
	this->initServiceListener();
}

void StateServiceProxy::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<StateService>(
								 mPluginContext,
								 boost::bind(&StateServiceProxy::onServiceAdded, this, _1),
								 boost::function<void (StateService*)>(),
								 boost::bind(&StateServiceProxy::onServiceRemoved, this, _1)
								 ));
	mServiceListener->open();
}
void StateServiceProxy::onServiceAdded(StateService* service)
{
	mService.reset(service, null_deleter());

	connect(mService.get(), &StateService::workflowStateChanged, this, &StateService::workflowStateChanged);
	connect(mService.get(), &StateService::workflowStateAboutToChange, this, &StateService::workflowStateAboutToChange);
	connect(mService.get(), &StateService::applicationStateChanged, this, &StateService::applicationStateChanged);
}

void StateServiceProxy::onServiceRemoved(StateService *service)
{
	disconnect(mService.get(), &StateService::workflowStateChanged, this, &StateService::workflowStateChanged);
	disconnect(mService.get(), &StateService::workflowStateAboutToChange, this, &StateService::workflowStateAboutToChange);
	disconnect(mService.get(), &StateService::applicationStateChanged, this, &StateService::applicationStateChanged);

	mService = StateService::getNullObject();
}



QString StateServiceProxy::getVersionName()
{
	return mService->getVersionName();
}

//QActionGroup* StateServiceProxy::getApplicationActions()
//{
//	return mService->getApplicationActions();
//}

QString StateServiceProxy::getApplicationStateName() const
{
	return mService->getApplicationStateName();
}

QStringList StateServiceProxy::getAllApplicationStateNames() const
{
	return mService->getAllApplicationStateNames();
}

QActionGroup* StateServiceProxy::getWorkflowActions()
{
	return mService->getWorkflowActions();
}

void StateServiceProxy::setWorkFlowState(QString uid)
{
	mService->setWorkFlowState(uid);
}

Desktop StateServiceProxy::getActiveDesktop()
{
	return mService->getActiveDesktop();
}

void StateServiceProxy::saveDesktop(Desktop desktop)
{
	mService->saveDesktop(desktop);
}

void StateServiceProxy::resetDesktop()
{
	mService->resetDesktop();
}

bool StateServiceProxy::isNull()
{
	return false;
}

}
