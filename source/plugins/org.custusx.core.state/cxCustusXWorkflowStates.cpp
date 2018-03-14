/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxCustusXWorkflowStates.h"
#include "cxStateService.h"
#include "cxSettings.h"
#include "cxTrackingService.h"
#include "cxTool.h"
#include "cxVideoService.h"
#include "cxPatientModelService.h"
#include "cxLogger.h"
#include "cxCoreServices.h"

namespace cx
{

// --------------------------------------------------------
// --------------------------------------------------------

NavigationWorkflowState::NavigationWorkflowState(QState* parent, CoreServicesPtr services) :
				WorkflowState(parent, "NavigationUid", "Navigation", services)
{
	connect(mServices->patient().get(), SIGNAL(patientChanged()), this, SLOT(canEnterSlot()));
}

void NavigationWorkflowState::onEntry(QEvent * event)
{
	WorkflowState::onEntry(event);
	this->autoStartHardware();
}

bool NavigationWorkflowState::canEnter() const
{
	return mServices->patient()->isPatientValid();
}

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

RegistrationWorkflowState::RegistrationWorkflowState(QState* parent, CoreServicesPtr services) :
				WorkflowState(parent, "RegistrationUid", "Registration", services)
{
	connect(mServices->patient().get(), SIGNAL(patientChanged()), this, SLOT(canEnterSlot()));
}
;

bool RegistrationWorkflowState::canEnter() const
{
// We need to perform patient orientation prior to
// running and us acq. Thus we need access to the reg mode.
	return mServices->patient()->isPatientValid();
}
;

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

PreOpPlanningWorkflowState::PreOpPlanningWorkflowState(QState* parent, CoreServicesPtr services) :
				WorkflowState(parent, "PreOpPlanningUid", "Preoperative Planning", services)
{
	connect(mServices->patient().get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(canEnterSlot()));
}

bool PreOpPlanningWorkflowState::canEnter() const
{
	return !mServices->patient()->getDatas().empty();
}

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

IntraOpImagingWorkflowState::IntraOpImagingWorkflowState(QState* parent, CoreServicesPtr services) :
				WorkflowState(parent, "IntraOpImagingUid", "Intraoperative Imaging", services)
{
	connect(mServices->patient().get(), SIGNAL(patientChanged()), this, SLOT(canEnterSlot()));
}

void IntraOpImagingWorkflowState::onEntry(QEvent * event)
{
	WorkflowState::onEntry(event);
	this->autoStartHardware();
}

bool IntraOpImagingWorkflowState::canEnter() const
{
	return mServices->patient()->isPatientValid();
}

// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

PostOpControllWorkflowState::PostOpControllWorkflowState(QState* parent, CoreServicesPtr services) :
				WorkflowState(parent, "PostOpControllUid", "Postoperative Control", services)
{
	connect(mServices->patient().get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(canEnterSlot()));
}

bool PostOpControllWorkflowState::canEnter() const
{
	return !mServices->patient()->getDatas().empty();
}

}

