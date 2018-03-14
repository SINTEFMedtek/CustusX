/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
