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

#ifndef CXCUSTUSXWORKFLOWSTATES_H_
#define CXCUSTUSXWORKFLOWSTATES_H_

#include "org_custusx_core_state_Export.h"

#include <iostream>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QAction>
#include "cxTypeConversions.h"
#include "cxRequestEnterStateTransition.h"
#include "cxWorkflowState.h"
#include "boost/shared_ptr.hpp"


namespace cx
{

class org_custusx_core_state_EXPORT PatientDataWorkflowState: public WorkflowState
{
Q_OBJECT

public:
	PatientDataWorkflowState(QState* parent, CoreServicesPtr backend) :
					WorkflowState(parent, "PatientDataUid", "Patient Data", backend)
	{}
	virtual ~PatientDataWorkflowState() {}
	virtual QIcon getIcon() const { return QIcon(":/icons/workflow_state_patient_data.png"); }

	virtual bool canEnter() const { return true; }
};

class org_custusx_core_state_EXPORT NavigationWorkflowState: public WorkflowState
{
Q_OBJECT

public:
	NavigationWorkflowState(QState* parent, CoreServicesPtr services);
	virtual ~NavigationWorkflowState() {}
	virtual QIcon getIcon() const
	{
		return QIcon(":/icons/workflow_state_navigation.png");
	}
	virtual void onEntry(QEvent* event);
	virtual bool canEnter() const;
};

class org_custusx_core_state_EXPORT RegistrationWorkflowState: public WorkflowState
{
Q_OBJECT

public:
	RegistrationWorkflowState(QState* parent, CoreServicesPtr services);

	virtual ~RegistrationWorkflowState() {}
	virtual QIcon getIcon() const
	{
		return QIcon(":/icons/workflow_state_registration.png");
	}

	virtual bool canEnter() const;
};

class org_custusx_core_state_EXPORT PreOpPlanningWorkflowState: public WorkflowState
{
Q_OBJECT

public:
	PreOpPlanningWorkflowState(QState* parent, CoreServicesPtr services);

	virtual ~PreOpPlanningWorkflowState()
	{}
	virtual QIcon getIcon() const
	{
		return QIcon(":/icons/workflow_state_planning.png");
	}

	virtual bool canEnter() const;
};

class org_custusx_core_state_EXPORT IntraOpImagingWorkflowState: public WorkflowState
{
Q_OBJECT

public:
	IntraOpImagingWorkflowState(QState* parent, CoreServicesPtr services);
	virtual ~IntraOpImagingWorkflowState()
	{}
	virtual QIcon getIcon() const
	{
		return QIcon(":/icons/workflow_state_acquisition.png");
	}

	virtual void onEntry(QEvent* event);
	virtual bool canEnter() const;
};

class org_custusx_core_state_EXPORT PostOpControllWorkflowState: public WorkflowState
{
Q_OBJECT

public:
	PostOpControllWorkflowState(QState* parent, CoreServicesPtr services);
	virtual ~PostOpControllWorkflowState() {}
	virtual QIcon getIcon() const
	{
		return QIcon(":/icons/workflow_state_post_op.png");
	}

	virtual bool canEnter() const;
};

/**
 * @}
 */
}
#endif /* CXCUSTUSXWORKFLOWSTATES_H_ */
