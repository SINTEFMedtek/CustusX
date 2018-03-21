/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxWorkflowState.h"
#include "cxStateService.h"
#include "cxSettings.h"
#include "cxTrackingService.h"
#include "cxTool.h"
#include "cxVideoService.h"
#include "cxPatientModelService.h"
#include "cxLogger.h"
#include "cxCoreServices.h"

namespace cx
{

void WorkflowState::onEntry(QEvent * event)
{
	report("Workflow change to [" + mName + "]");
	if (mAction)
		mAction->setChecked(true);
}

void WorkflowState::onExit(QEvent * event)
{
	emit aboutToExit();
}

std::vector<WorkflowState*> WorkflowState::getChildStates()
{
	QObjectList childrenList = this->children();
	std::vector<WorkflowState*> retval;
	for (int i = 0; i < childrenList.size(); ++i)
	{
		WorkflowState* state = dynamic_cast<WorkflowState*>(childrenList[i]);
		if (state)
			retval.push_back(state);
	}
	return retval;
}

QAction* WorkflowState::createAction(QActionGroup* group)
{
	if (mAction)
		return mAction;

	mAction = new QAction(this->getName(), group);
	mAction->setIcon(this->getIcon());
	mAction->setStatusTip(this->getName());
	mAction->setCheckable(true);
	mAction->setData(QVariant(this->getUid()));
	this->canEnterSlot();

	connect(mAction, SIGNAL(triggered()), this, SLOT(setActionSlot()));

	this->enableAction(mEnableAction);
	return mAction;
}

void WorkflowState::canEnterSlot()
{
	if (mAction)
		mAction->setEnabled(this->canEnter());
}

void WorkflowState::setActionSlot()
{
	this->machine()->postEvent(new RequestEnterStateEvent(this->getUid()));
}


void WorkflowState::autoStartHardware()
{
	if (settings()->value("Automation/autoStartTracking").toBool())
		mServices->tracking()->setState(Tool::tsTRACKING);
	if (settings()->value("Automation/autoStartStreaming").toBool())
		mServices->video()->openConnection();
}

void WorkflowState::enableAction(bool enable)
{
	mEnableAction = enable;
	if(mAction)
		mAction->setEnabled(enable);
}

} //namespace cx

