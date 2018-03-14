/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSTATESERVICENULL_H
#define CXSTATESERVICENULL_H

#include "cxStateService.h"

namespace cx
{

/**
 * \ingroup cx_resource_core_settings
 */
class cxResource_EXPORT StateServiceNull: public StateService
{
Q_OBJECT

public:
	StateServiceNull();
	virtual ~StateServiceNull() {}

	virtual QString getVersionName();
//	virtual QActionGroup* getApplicationActions();
	virtual QString getApplicationStateName() const;
	virtual QStringList getAllApplicationStateNames() const;

	virtual QActionGroup* getWorkflowActions();
	virtual void setWorkFlowState(QString uid);

	virtual Desktop getActiveDesktop();
	virtual void saveDesktop(Desktop desktop);
	virtual void resetDesktop();

	virtual bool isNull();

private:
	QActionGroup* mActionGroup;
};

}

#endif // CXSTATESERVICENULL_H
