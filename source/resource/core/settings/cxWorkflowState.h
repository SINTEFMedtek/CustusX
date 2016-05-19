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
