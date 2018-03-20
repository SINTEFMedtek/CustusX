/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSTATESERVICEIMPL_H
#define CXSTATESERVICEIMPL_H

#include "org_custusx_core_state_Export.h"
#include "cxStateService.h"

namespace cx
{
typedef boost::shared_ptr<class ApplicationStateMachine> ApplicationStateMachinePtr;
typedef boost::shared_ptr<class WorkflowStateMachine> WorkflowStateMachinePtr;


/**
 * \ingroup org_custusx_core_state
 * \date 4. aug. 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_core_state_EXPORT StateServiceImpl: public StateService
{
Q_OBJECT

public:
	virtual ~StateServiceImpl();
	StateServiceImpl(ctkPluginContext* context);

	virtual QString getVersionName();
	virtual QString getApplicationStateName() const;
	virtual QStringList getAllApplicationStateNames() const;

	virtual QActionGroup* getWorkflowActions();
	virtual void setWorkFlowState(QString uid);

	virtual Desktop getActiveDesktop();
	virtual void saveDesktop(Desktop desktop);
	virtual void resetDesktop();

	virtual bool isNull();

private:
	WorkflowStateMachinePtr getWorkflow();
	void initialize();

	WorkflowStateMachinePtr mWorkflowStateMachine;
	VisServicesPtr mServices;
};

}

#endif // CXSTATESERVICEIMPL_H
