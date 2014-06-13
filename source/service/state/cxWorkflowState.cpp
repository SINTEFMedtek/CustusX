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

#include "cxWorkflowState.h"
#include "cxVideoConnectionManager.h"
#include "cxDataManager.h"
#include "cxStateService.h"
#include "cxPatientData.h"
#include "cxSettings.h"
#include "cxToolManager.h"
#include "cxPatientService.h"
#include "cxVideoService.h"
#include "cxStateServiceBackend.h"

namespace cx
{

void WorkflowState::onEntry(QEvent * event)
{
	report("Workflow change to [" + mName + "]");
	if (mAction)
		mAction->setChecked(true);
}

void WorkflowState::onExit(QEvent * event)
{
	emit aboutToExit();
}

std::vector<WorkflowState*> WorkflowState::getChildStates()
{
	QObjectList childrenList = this->children();
	std::vector<WorkflowState*> retval;
	for (int i = 0; i < childrenList.size(); ++i)
	{
		WorkflowState* state = dynamic_cast<WorkflowState*>(childrenList[i]);
		if (state)
			retval.push_back(state);
	}
	return retval;
}

QAction* WorkflowState::createAction(QActionGroup* group)
{
	if (mAction)
		return mAction;

	mAction = new QAction(this->getName(), group);
	mAction->setIcon(this->getIcon());
	mAction->setStatusTip(this->getName());
	mAction->setCheckable(true);
	mAction->setData(QVariant(this->getUid()));
	this->canEnterSlot();

	connect(mAction, SIGNAL(triggered()), this, SLOT(setActionSlot()));

	return mAction;
}

void WorkflowState::canEnterSlot()
{
	if (mAction)
		mAction->setEnabled(this->canEnter());
}

void WorkflowState::setActionSlot()
{
	this->machine()->postEvent(new RequestEnterStateEvent(this->getUid()));
}


void WorkflowState::autoStartHardware()
{
	if (settings()->value("Automation/autoStartTracking").toBool())
		mBackend->getToolManager()->startTracking();
	if (settings()->value("Automation/autoStartStreaming").toBool())
		mBackend->getVideoService()->getVideoConnection()->launchAndConnectServer();
}

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

NavigationWorkflowState::NavigationWorkflowState(QState* parent, StateServiceBackendPtr backend) :
				WorkflowState(parent, "NavigationUid", "Navigation", backend)
{
	connect(mBackend->getPatientService()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(canEnterSlot()));
}

void NavigationWorkflowState::onEntry(QEvent * event)
{
	this->autoStartHardware();
}

bool NavigationWorkflowState::canEnter() const
{
	return mBackend->getPatientService()->getPatientData()->isPatientValid();
}

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

RegistrationWorkflowState::RegistrationWorkflowState(QState* parent, StateServiceBackendPtr backend) :
				WorkflowState(parent, "RegistrationUid", "Registration", backend)
{
	connect(mBackend->getPatientService()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(canEnterSlot()));
}
;

bool RegistrationWorkflowState::canEnter() const
{
// We need to perform patient orientation prior to
// running and us acq. Thus we need access to the reg mode.
	return mBackend->getPatientService()->getPatientData()->isPatientValid();
}
;

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

PreOpPlanningWorkflowState::PreOpPlanningWorkflowState(QState* parent, StateServiceBackendPtr backend) :
				WorkflowState(parent, "PreOpPlanningUid", "Preoperative Planning", backend)
{
	connect(mBackend->getDataManager().get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(canEnterSlot()));
}

bool PreOpPlanningWorkflowState::canEnter() const
{
	return !mBackend->getDataManager()->getData().empty();
}

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

IntraOpImagingWorkflowState::IntraOpImagingWorkflowState(QState* parent, StateServiceBackendPtr backend) :
				WorkflowState(parent, "IntraOpImagingUid", "Intraoperative Imaging", backend)
{
	connect(mBackend->getPatientService()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(canEnterSlot()));
}

void IntraOpImagingWorkflowState::onEntry(QEvent * event)
{
	this->autoStartHardware();
}

bool IntraOpImagingWorkflowState::canEnter() const
{
	return mBackend->getPatientService()->getPatientData()->isPatientValid();
}

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

PostOpControllWorkflowState::PostOpControllWorkflowState(QState* parent, StateServiceBackendPtr backend) :
				WorkflowState(parent, "PostOpControllUid", "Postoperative Control", backend)
{
	connect(mBackend->getDataManager().get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(canEnterSlot()));
}

bool PostOpControllWorkflowState::canEnter() const
{
	return !mBackend->getDataManager()->getData().empty();
}

}

