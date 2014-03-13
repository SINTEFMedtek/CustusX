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

#ifndef CXWORKFLOWSTATE_H_
#define CXWORKFLOWSTATE_H_

#include <iostream>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QAction>
#include "cxTypeConversions.h"
#include "cxRequestEnterStateTransition.h"
#include "cxMessageManager.h"

namespace cx
{
typedef boost::shared_ptr<class StateServiceBackend> StateServiceBackendPtr;

/**
 * \file
 * \addtogroup cx_service_state
 * @{
 */

/**\brief State in a WorkflowStateMachine
 *
 * \date 4. aug. 2010
 * \author jbake
 */
class WorkflowState: public QState
{
Q_OBJECT

public:
	WorkflowState(QState* parent, QString uid, QString name, StateServiceBackendPtr backend) :
					QState(parent),
					mUid(uid),
					mName(name),
					mAction(NULL),
		mBackend(backend)
	{}

	virtual ~WorkflowState() {}

	virtual void onEntry(QEvent * event);
	virtual void onExit(QEvent * event);

	virtual QString getUid() const { return mUid; }
	virtual QString getName() const { return mName; }

	std::vector<WorkflowState*> getChildStates();
	QAction* createAction(QActionGroup* group);

signals:
	void aboutToExit();

protected slots:
	void canEnterSlot();
	void setActionSlot();

protected:
	virtual bool canEnter() const = 0;
	virtual QIcon getIcon() const = 0;
	void autoStartHardware();

	QString mUid;
	QString mName;
	QAction* mAction;
	StateServiceBackendPtr mBackend;
};

class ParentWorkflowState: public WorkflowState
{
Q_OBJECT
public:
	ParentWorkflowState(QState* parent, StateServiceBackendPtr backend) :
					WorkflowState(parent, "ParentUid", "Parent", backend) {}
	virtual ~ParentWorkflowState() {}
	virtual void onEntry(QEvent * event) {}
	virtual void onExit(QEvent * event) {}
	virtual bool canEnter() const { return true; }
	virtual QIcon getIcon() const { return QIcon(""); }
};

class PatientDataWorkflowState: public WorkflowState
{
Q_OBJECT

public:
	PatientDataWorkflowState(QState* parent, StateServiceBackendPtr backend) :
					WorkflowState(parent, "PatientDataUid", "Patient Data", backend)
	{}
	virtual ~PatientDataWorkflowState() {}
	virtual QIcon getIcon() const { return QIcon(":/icons/workflow_state_patient_data.png"); }

	virtual bool canEnter() const { return true; }
};

class NavigationWorkflowState: public WorkflowState
{
Q_OBJECT

public:
	NavigationWorkflowState(QState* parent, StateServiceBackendPtr backend);
	virtual ~NavigationWorkflowState() {}
	virtual QIcon getIcon() const
	{
		return QIcon(":/icons/workflow_state_navigation.png");
	}
	virtual void onEntry(QEvent* event);
	virtual bool canEnter() const;
};

class RegistrationWorkflowState: public WorkflowState
{
Q_OBJECT

public:
	RegistrationWorkflowState(QState* parent, StateServiceBackendPtr backend);

	virtual ~RegistrationWorkflowState() {}
	virtual QIcon getIcon() const
	{
		return QIcon(":/icons/workflow_state_registration.png");
	}

	virtual bool canEnter() const;
};

class PreOpPlanningWorkflowState: public WorkflowState
{
Q_OBJECT

public:
	PreOpPlanningWorkflowState(QState* parent, StateServiceBackendPtr backend);

	virtual ~PreOpPlanningWorkflowState()
	{}
	virtual QIcon getIcon() const
	{
		return QIcon(":/icons/workflow_state_planning.png");
	}

	virtual bool canEnter() const;
};

class IntraOpImagingWorkflowState: public WorkflowState
{
Q_OBJECT

public:
	IntraOpImagingWorkflowState(QState* parent, StateServiceBackendPtr backend);
	virtual ~IntraOpImagingWorkflowState()
	{}
	virtual QIcon getIcon() const
	{
		return QIcon(":/icons/workflow_state_acquisition.png");
	}

	virtual void onEntry(QEvent* event);
	virtual bool canEnter() const;
};

class PostOpControllWorkflowState: public WorkflowState
{
Q_OBJECT

public:
	PostOpControllWorkflowState(QState* parent, StateServiceBackendPtr backend);
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
#endif /* CXWORKFLOWSTATE_H_ */
