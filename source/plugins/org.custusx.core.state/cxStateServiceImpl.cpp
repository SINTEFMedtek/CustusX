/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxStateServiceImpl.h"

#include <iostream>
#include <QApplication>
#include <QByteArray>
#include <QDir>
#include "cxEnumConverter.h"
#include "cxXmlOptionItem.h"

#include "cxDataLocations.h"
#include "cxWorkflowStateMachine.h"
#include "cxCustusXWorkflowStateMachine.h"
#include "cxDataLocations.h"
#include "cxConfig.h"

#include "cxTrackingServiceProxy.h"
#include "cxPatientModelServiceProxy.h"
#include "cxSpaceProviderImpl.h"
#include "cxVideoServiceProxy.h"
#include "cxApplicationsParser.h"
#include "cxProfile.h"
#include "cxLogger.h"
#include "cxVisServices.h"

namespace cx
{

StateServiceImpl::StateServiceImpl(ctkPluginContext* context)
{

	mServices = VisServices::create(context);
	this->initialize();
}

StateServiceImpl::~StateServiceImpl()
{
}

bool StateServiceImpl::isNull()
{
	return false;
}

void StateServiceImpl::initialize()
{
	this->fillDefaultSettings();

	ProfileManager::initialize();

	mWorkflowStateMachine.reset(new CustusXWorkflowStateMachine(mServices));
	mWorkflowStateMachine->start();

	connect(mWorkflowStateMachine.get(), &WorkflowStateMachine::activeStateChanged, this, &StateServiceImpl::workflowStateChanged);
	connect(mWorkflowStateMachine.get(), &WorkflowStateMachine::activeStateAboutToChange, this, &StateServiceImpl::workflowStateAboutToChange);

	connect(ProfileManager::getInstance(), &ProfileManager::activeProfileChanged, this, &StateServiceImpl::applicationStateChanged);
}

QString StateServiceImpl::getApplicationStateName() const
{
	return ProfileManager::getInstance()->activeProfile()->getUid();
}

QStringList StateServiceImpl::getAllApplicationStateNames() const
{
	return ProfileManager::getInstance()->getProfiles();
}

QString StateServiceImpl::getVersionName()
{
	return QString("%1").arg(CustusX_VERSION_STRING);
}

QActionGroup* StateServiceImpl::getWorkflowActions()
{
	return mWorkflowStateMachine->getActionGroup();
}

WorkflowStateMachinePtr StateServiceImpl::getWorkflow()
{
	return mWorkflowStateMachine;
}

void StateServiceImpl::setWorkFlowState(QString uid)
{
	mWorkflowStateMachine->setActiveState(uid);
}

Desktop StateServiceImpl::getActiveDesktop()
{
	ApplicationsParser parser;
	return parser.getDesktop(mWorkflowStateMachine->getActiveUidState());
}

void StateServiceImpl::saveDesktop(Desktop desktop)
{
	ApplicationsParser parser;
	parser.setDesktop(mWorkflowStateMachine->getActiveUidState(),
					  desktop);
}

void StateServiceImpl::resetDesktop()
{
	ApplicationsParser parser;
	parser.resetDesktop(mWorkflowStateMachine->getActiveUidState());
}

} //namespace cx
