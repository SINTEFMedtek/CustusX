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

#ifndef CXWORKFLOWSTATEMACHINE_H_
#define CXWORKFLOWSTATEMACHINE_H_

#include <QStateMachine>
#include <QActionGroup>
#include "cxForwardDeclarations.h"

class QToolBar;
class QMenu;

namespace cx
{
typedef boost::shared_ptr<class StateServiceBackend> StateServiceBackendPtr;

/**
 * \file
 * \addtogroup cxServiceState
 * @{
 */

class WorkflowState;

/** \brief State Machine for the Workflow Steps
 *
 *  See StateService for a description.
 *
 * \date 4. aug. 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class WorkflowStateMachine: public QStateMachine
{
Q_OBJECT
public:
	WorkflowStateMachine(StateServiceBackendPtr backend);
	virtual ~WorkflowStateMachine();

	QActionGroup* getActionGroup();
	void fillMenu(QMenu* menu);
	void fillToolBar(QToolBar* toolbar);

	QString getActiveUidState();
	void setActiveState(QString uid);

signals:
	void activeStateChanged();
	void activeStateAboutToChange();

private slots:
	void startedSlot();
	void clinicalApplicationChangedSlot();

private:
	void fillMenu(QMenu* menu, WorkflowState* current);
	void fillToolbar(QToolBar* toolbar, WorkflowState* current);

	QAction* addAction(QString stateUid, QActionGroup* group);
	WorkflowState* newState(WorkflowState* state);

	typedef std::map<QString, WorkflowState*> WorkflowStateMap;
	WorkflowStateMap mStates;
	WorkflowState* mParentState;
	QActionGroup* mActionGroup;
	bool mStarted;
	StateServiceBackendPtr mBackend;
};

typedef boost::shared_ptr<WorkflowStateMachine> WorkflowStateMachinePtr;

/**
 * @}
 */
}

#endif /* CXWORKFLOWSTATEMACHINE_H_ */
