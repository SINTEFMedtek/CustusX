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
#include "sscDataManager.h"
#include "cxStateService.h"
#include "cxPatientData.h"
#include "cxSettings.h"
#include "sscToolManager.h"
#include "cxPatientService.h"
#include "cxVideoService.h"

namespace cx
{

void WorkflowState::onEntry(QEvent * event)
{
	ssc::messageManager()->sendInfo("Workflow change to [" + mName + "]");
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
;

void WorkflowState::autoStartHardware()
{
	if (settings()->value("Automation/autoStartTracking").toBool())
		ssc::toolManager()->startTracking();
	if (settings()->value("Automation/autoStartStreaming").toBool())
		videoService()->getIGTLinkVideoConnection()->launchAndConnectServer();
}

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

NavigationWorkflowState::NavigationWorkflowState(QState* parent) :
				WorkflowState(parent, "NavigationUid", "Navigation")
{
	connect(patientService()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(canEnterSlot()));
}

void NavigationWorkflowState::onEntry(QEvent * event)
{
	this->autoStartHardware();
}

bool NavigationWorkflowState::canEnter() const
{
	return patientService()->getPatientData()->isPatientValid();
}

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

RegistrationWorkflowState::RegistrationWorkflowState(QState* parent) :
				WorkflowState(parent, "RegistrationUid", "Registration")
{
//	connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(canEnterSlot()));
	connect(patientService()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(canEnterSlot()));
}
;

bool RegistrationWorkflowState::canEnter() const
{
// We need to perform patient orientation prior to
// running and us acq. Thus we need access to the reg mode.
//	return !ssc::dataManager()->getImages().empty();
	return patientService()->getPatientData()->isPatientValid();
}
;

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

PreOpPlanningWorkflowState::PreOpPlanningWorkflowState(QState* parent) :
				WorkflowState(parent, "PreOpPlanningUid", "Preoperative Planning")
{
	connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(canEnterSlot()));
}

bool PreOpPlanningWorkflowState::canEnter() const
{
	return !ssc::dataManager()->getImages().empty();
}

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

IntraOpImagingWorkflowState::IntraOpImagingWorkflowState(QState* parent) :
				WorkflowState(parent, "IntraOpImagingUid", "Intraoperative Imaging")
{
	connect(patientService()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(canEnterSlot()));
}

void IntraOpImagingWorkflowState::onEntry(QEvent * event)
{
	this->autoStartHardware();
}

bool IntraOpImagingWorkflowState::canEnter() const
{
	return patientService()->getPatientData()->isPatientValid();
}

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

PostOpControllWorkflowState::PostOpControllWorkflowState(QState* parent) :
				WorkflowState(parent, "PostOpControllUid", "Postoperative Control")
{
	connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(canEnterSlot()));
}

bool PostOpControllWorkflowState::canEnter() const
{
	return !ssc::dataManager()->getImages().empty();
}

}

