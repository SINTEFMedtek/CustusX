/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxStateServiceNull.h"
#include <QStringList>
#include <QActionGroup>

namespace cx
{

StateServiceNull::StateServiceNull()
{
	mActionGroup = new QActionGroup(this);
}

QString StateServiceNull::getVersionName() { return ""; }
//QActionGroup* StateServiceNull::getApplicationActions() { return mActionGroup; }
QString StateServiceNull::getApplicationStateName() const { return ""; }
QStringList StateServiceNull::getAllApplicationStateNames() const { return QStringList(); }

QActionGroup* StateServiceNull::getWorkflowActions() { return mActionGroup; }
void StateServiceNull::setWorkFlowState(QString uid) {}

Desktop StateServiceNull::getActiveDesktop() { return Desktop(); }
void StateServiceNull::saveDesktop(Desktop desktop) {}
void StateServiceNull::resetDesktop() {}

WorkflowStateMachinePtr StateServiceNull::getWorkflow() {return WorkflowStateMachinePtr(); }

bool StateServiceNull::isNull() { return true; }

}
