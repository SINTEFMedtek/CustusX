/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSTATESERVICE_H_
#define CXSTATESERVICE_H_

#include "cxResourceExport.h"

#include "cxForwardDeclarations.h"
#include <QObject>
#include "cxSettings.h"
class QActionGroup;
class ctkPluginContext;

#define StateService_iid "cx::StateService"

namespace cx
{

/**
 * \file
 * \addtogroup cx_resource_core_settings
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
struct cxResource_EXPORT Desktop
{
	Desktop();
	Desktop(QString layout, QByteArray mainwindowstate);
	void addPreset(QString name, int pos, bool tabbed=true);

	QString mLayoutUid;
	QString mSecondaryLayoutUid;
	QByteArray mMainWindowState;

	struct Preset
	{
		QString name; // name of widget without postfix (i.e. Workflow instead of WorkflowToolbar)
		int position; // corresponding to values in Qt::DockWidgetArea and Qt::ToolBarArea
		bool tabbed;
	};
	std::vector<Preset> mPresets; // used on top of the MainWindowState: gives programmatic control over positioning
};

/**
 * \brief Workflow states for CustusX.
 *
 * Maintains global state. The responsibility of this service
 * has been reduced, it now controls the workflow steps.
 *
 * - \ref cx_resource_core_settings also contains some global state.
 * - \ref cx::Profile contains more state.
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
class cxResource_EXPORT StateService: public QObject
{
Q_OBJECT

public:
	virtual ~StateService() {}

	virtual QString getVersionName() = 0;
	virtual QString getApplicationStateName() const = 0;
	virtual QStringList getAllApplicationStateNames() const = 0;

	virtual QActionGroup* getWorkflowActions() = 0;
	virtual void setWorkFlowState(QString uid) = 0;

	virtual Desktop getActiveDesktop() = 0;
	virtual void saveDesktop(Desktop desktop) = 0;
	virtual void resetDesktop() = 0;

	virtual bool isNull() = 0;
	static StateServicePtr getNullObject();

protected:
	void fillDefaultSettings();
	template<class T>
	void fillDefault(QString name, T value)
	{
		settings()->fillDefault(name, value);
	}

signals:
	void workflowStateChanged();
	void workflowStateAboutToChange();
	void applicationStateChanged();
};

/**
 * @}
 */
}
Q_DECLARE_INTERFACE(cx::StateService, StateService_iid)

#endif /* CXSTATESERVICE_H_ */
