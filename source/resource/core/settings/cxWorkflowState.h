/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXWORKFLOWSTATE_H_
#define CXWORKFLOWSTATE_H_

#include "cxResourceExport.h"

#include <iostream>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QAction>
#include "cxTypeConversions.h"
#include "cxRequestEnterStateTransition.h"
#include "boost/shared_ptr.hpp"
#include "cxForwardDeclarations.h"


namespace cx
{

/**
 * \file
 * \addtogroup org_custusx_core_state
 * @{
 */

/** \brief State in a WorkflowStateMachine
 *
 * \date 4. aug. 2010
 * \author jbake
 */
class cxResource_EXPORT WorkflowState: public QState
{
Q_OBJECT

public:
	WorkflowState(QState* parent, QString uid, QString name, CoreServicesPtr services, bool enableAction = true) :
		QState(parent),
		mUid(uid),
		mName(name),
		mAction(NULL),
		mServices(services),
		mEnableAction(enableAction)
	{}

	virtual ~WorkflowState() {}

	virtual void onEntry(QEvent * event);
	virtual void onExit(QEvent * event);

	virtual QString getUid() const { return mUid; }
	virtual QString getName() const { return mName; }

	std::vector<WorkflowState*> getChildStates();
	QAction* createAction(QActionGroup* group);

	virtual void enableAction(bool enable);

signals:
	void aboutToExit();

public slots:
	void canEnterSlot();
protected slots:
	void setActionSlot();

protected:
	virtual bool canEnter() const = 0;
	virtual QIcon getIcon() const = 0;
	void autoStartHardware();

	QString mUid;
	QString mName;
	QAction* mAction;
	CoreServicesPtr mServices;

private:
	bool mEnableAction;
};

class cxResource_EXPORT ParentWorkflowState: public WorkflowState
{
Q_OBJECT
public:
	ParentWorkflowState(QState* parent, CoreServicesPtr services) :
					WorkflowState(parent, "ParentUid", "Parent", services) {}
	virtual ~ParentWorkflowState() {}
	virtual void onEntry(QEvent * event) {}
	virtual void onExit(QEvent * event) {}
	virtual bool canEnter() const { return true; }
	virtual QIcon getIcon() const { return QIcon(""); }
};


/**
 * @}
 */
}
#endif /* CXWORKFLOWSTATE_H_ */
