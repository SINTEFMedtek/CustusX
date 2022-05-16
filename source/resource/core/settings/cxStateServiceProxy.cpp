/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

	emit workflowStateAboutToChange();
	emit workflowStateChanged();
	emit applicationStateChanged();
}

void StateServiceProxy::onServiceRemoved(StateService *service)
{
	disconnect(mService.get(), &StateService::workflowStateChanged, this, &StateService::workflowStateChanged);
	disconnect(mService.get(), &StateService::workflowStateAboutToChange, this, &StateService::workflowStateAboutToChange);
	disconnect(mService.get(), &StateService::applicationStateChanged, this, &StateService::applicationStateChanged);

	mService = StateService::getNullObject();

	emit workflowStateAboutToChange();
	emit workflowStateChanged();
	emit applicationStateChanged();
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


WorkflowStateMachinePtr StateServiceProxy::getWorkflow()
{
	return mService->getWorkflow();
}

bool StateServiceProxy::isNull()
{
	return false;
}

}
