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

#ifndef CXWORKFLOWSTATE_H_
#define CXWORKFLOWSTATE_H_

#include <iostream>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QAction>
#include "cxTypeConversions.h"
#include "cxRequestEnterStateTransition.h"
#include "cxReporter.h"

namespace cx
{
typedef boost::shared_ptr<class StateServiceBackend> StateServiceBackendPtr;
typedef boost::shared_ptr<class VideoService> VideoServicePtr;

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
	WorkflowState(VideoServicePtr videoService, QState* parent, QString uid, QString name, StateServiceBackendPtr backend) :
		QState(parent),
		mUid(uid),
		mName(name),
		mAction(NULL),
		mBackend(backend),
		mVideoService(videoService)
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
	VideoServicePtr mVideoService;
};

class ParentWorkflowState: public WorkflowState
{
Q_OBJECT
public:
	ParentWorkflowState(VideoServicePtr videoService, QState* parent, StateServiceBackendPtr backend) :
					WorkflowState(videoService, parent, "ParentUid", "Parent", backend) {}
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
	PatientDataWorkflowState(VideoServicePtr videoService, QState* parent, StateServiceBackendPtr backend) :
					WorkflowState(videoService, parent, "PatientDataUid", "Patient Data", backend)
	{}
	virtual ~PatientDataWorkflowState() {}
	virtual QIcon getIcon() const { return QIcon(":/icons/workflow_state_patient_data.png"); }

	virtual bool canEnter() const { return true; }
};

class NavigationWorkflowState: public WorkflowState
{
Q_OBJECT

public:
	NavigationWorkflowState(VideoServicePtr videoService, QState* parent, StateServiceBackendPtr backend);
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
	RegistrationWorkflowState(VideoServicePtr videoService, QState* parent, StateServiceBackendPtr backend);

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
	PreOpPlanningWorkflowState(VideoServicePtr videoService, QState* parent, StateServiceBackendPtr backend);

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
	IntraOpImagingWorkflowState(VideoServicePtr videoService, QState* parent, StateServiceBackendPtr backend);
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
	PostOpControllWorkflowState(VideoServicePtr videoService, QState* parent, StateServiceBackendPtr backend);
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
