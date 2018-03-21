/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXWORKFLOWSTATEMACHINE_H_
#define CXWORKFLOWSTATEMACHINE_H_

#include "cxResourceExport.h"

#include <QStateMachine>
#include <QActionGroup>
#include "cxForwardDeclarations.h"

class QToolBar;
class QMenu;

namespace cx
{

class WorkflowState;

/** \brief State Machine for the Workflow Steps
 *
 *  See StateService for a description.
 *
 * \ingroup org_custusx_core_state
 * \date 4. aug. 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResource_EXPORT WorkflowStateMachine: public QStateMachine
{
Q_OBJECT
public:
	WorkflowStateMachine(CoreServicesPtr services);
	virtual ~WorkflowStateMachine();

	QActionGroup* getActionGroup();

	QString getActiveUidState();
	void setActiveState(QString uid);

signals:
    void activeStateChanged();
    void activeStateAboutToChange();

private slots:
	void startedSlot();
	void clinicalApplicationChangedSlot();

protected:
    virtual WorkflowState* newState(WorkflowState* state);

    WorkflowState* mParentState;
	CoreServicesPtr mServices;

private:
	void fillActionGroup(WorkflowState* current, QActionGroup* group);
	QAction* addAction(QString stateUid, QActionGroup* group);

	typedef std::map<QString, WorkflowState*> WorkflowStateMap;
	WorkflowStateMap mStates;
	QActionGroup* mActionGroup;
	bool mStarted;
};

typedef boost::shared_ptr<WorkflowStateMachine> WorkflowStateMachinePtr;
}

#endif /* CXWORKFLOWSTATEMACHINE_H_ */
