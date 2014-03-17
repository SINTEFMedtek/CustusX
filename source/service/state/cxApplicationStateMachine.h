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

#ifndef CXAPPLICATIONSTATEMACHINE_H_
#define CXAPPLICATIONSTATEMACHINE_H_

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
 * \addtogroup cx_service_state
 * @{
 */

class ApplicationState;

/** \brief State Machine for the Clinical Application
 *
 *  See StateService for a description.
 *
 *  \date Aug 17, 2010
 *  \author christiana
 */
class ApplicationStateMachine: public QStateMachine
{
Q_OBJECT
public:
	ApplicationStateMachine(StateServiceBackendPtr backend);
	virtual ~ApplicationStateMachine();

	QActionGroup* getActionGroup();

	QString getActiveUidState();
	QString getActiveStateName();

	QStringList getAllApplicationNames();

signals:
	void activeStateChanged();

private slots:
	void activeStateChangedSlot();

private:
	QAction* addAction(QString stateUid, QActionGroup* group);
	ApplicationState* newState(ApplicationState* state);

	typedef std::map<QString, ApplicationState*> ApplicationStateMap;
	ApplicationStateMap mStates;
	ApplicationState* mParentState;
	QActionGroup* mActionGroup;
	StateServiceBackendPtr mBackend;
};

typedef boost::shared_ptr<ApplicationStateMachine> ApplicationStateMachinePtr;

/**
 * @}
 */
}

#endif /* CXAPPLICATIONSTATEMACHINE_H_ */
