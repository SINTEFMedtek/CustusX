/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxCustusXWorkflowStateMachine.h"
#include "cxCustusXWorkflowStates.h"
#include "cxVisServices.h"

namespace cx
{

CustusXWorkflowStateMachine::CustusXWorkflowStateMachine(VisServicesPtr services) :
	WorkflowStateMachine(services)
{
	WorkflowState* patientData = this->newState(new PatientDataWorkflowState(mParentState, services));
	this->newState(new RegistrationWorkflowState(mParentState, services));
	this->newState(new PreOpPlanningWorkflowState(mParentState, services));
	this->newState(new NavigationWorkflowState(mParentState, services));
	this->newState(new IntraOpImagingWorkflowState(mParentState, services));
	this->newState(new PostOpControllWorkflowState(mParentState, services));

	//set initial state on all levels
	this->setInitialState(mParentState);
	mParentState->setInitialState(patientData);
}

CustusXWorkflowStateMachine::~CustusXWorkflowStateMachine()
{}

} //namespace cx
