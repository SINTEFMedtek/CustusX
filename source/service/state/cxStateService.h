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

#ifndef CXSTATESERVICE_H_
#define CXSTATESERVICE_H_

#include <QDomNode>

#include "cxForwardDeclarations.h"
#include <QObject>

namespace cx
{
typedef boost::shared_ptr<class ApplicationStateMachine> ApplicationStateMachinePtr;
typedef boost::shared_ptr<class WorkflowStateMachine> WorkflowStateMachinePtr;

/**
 * \file
 * \addtogroup cxServiceState
 * @{
 */

/**
 * \brief Data class for CustusX desktop.
 *
 * Contains info about widget and toolbar info in the MainWindow,
 * and the Layout ID.
 *
 * \sa StateService
 *
 */
struct Desktop
{
	Desktop();
	Desktop(QString layout, QByteArray mainwindowstate);

	QString mLayoutUid;
	QByteArray mMainWindowState;
};

/**
 * \brief Global Application states for CustusX.
 *
 * The StateManager is the main class in \ref cxServiceState , and maintains
 * global state.
 *
 * Exception: \ref cxResourceSettings also contains some global state.
 *
 * \image html cxArchitecture_application.png "State Service main classes"
 *
 *
 *
 * \section cx_section_clinical_application_state Clinical Application State
 *
 * The active clinical application is known to the entire system.
 * The options are:
 *
 *  - Laboratory
 *  - Neurology
 *  - Laparascopy
 *  - Bronchoscopy
 *  - Endovascular
 *
 * The application can be used to customize the system to that clinical
 * area. Each one are represented as a state in StateManager. Currently the
 * tool configurations, the workflow states and the view orientations are
 * affected by this.
 *
 * \sa ApplicationStateMachine
 *
 *
 * \section cx_section_workflow Workflow
 *
 * The use of CustusX during a surgical procedure can be broken down into a series of steps:
 *
 *  - Preoperative data acquisition
 *  - Planning
 *  - Registration
 *  - Navigation + Intraoperative data acquisition.
 *  - Postoperative analysis
 *
 * This is the main workflow. Each step is named a Workflow State, and are
 * states in StateManager. They can be customized in the State Manager, and
 * they have a unique GUI setup.
 *
 * \image html workflow_steps_small.png "Workflow steps. The buttons are enlarged."
 *
 * \sa WorkflowStateMachine
 *
 * During each step, There are operations that usually are performed in
 * sequence. Examples:
 *
 *  - Acquire data -> reconstruct.
 *  - Import/acquire data -> show in view.
 *
 * These automations can be set in Preferences->Automation.
 *
 *
 *
 * \date 4. aug. 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class StateService: public QObject
{
Q_OBJECT

public:
	static StateService* getInstance(); ///< returns the only instance of this class
	static void destroyInstance(); ///< destroys the only instance of this class

	QString getVersionName();

	WorkflowStateMachinePtr getWorkflow();
	ApplicationStateMachinePtr getApplication();

	Desktop getActiveDesktop();
	void saveDesktop(Desktop desktop);
	void resetDesktop();

private:
	StateService();
	virtual ~StateService();

	void initialize(); ///< init stuff that is dependent of the statemanager
	void fillDefaultSettings();
	template<class T>
	void fillDefault(QString name, T value);
	QString getDefaultGrabberServer();
	QString checkGrabberServerExist(QString path, QString filename, QString args);
	QString getDefaultGrabberInitScript();

	static StateService* mTheInstance; ///< the only instance of this class

	WorkflowStateMachinePtr mWorkflowStateMachine;
	ApplicationStateMachinePtr mApplicationStateMachine;
};

StateService* stateService();

/**
 * @}
 */
}
#endif /* CXSTATESERVICE_H_ */
