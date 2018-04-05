/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSTATESERVICEPROXY_H
#define CXSTATESERVICEPROXY_H

#include "cxStateService.h"
#include "cxServiceTrackerListener.h"

namespace cx
{

/**
 * \ingroup cx_resource_core_settings
 */
class cxResource_EXPORT StateServiceProxy: public StateService
{
Q_OBJECT

public:
	static StateServicePtr create(ctkPluginContext *pluginContext);
	StateServiceProxy(ctkPluginContext *pluginContext);
	virtual ~StateServiceProxy() {}

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
	void initServiceListener();
	void onServiceAdded(StateService* service);
	void onServiceRemoved(StateService *service);

	ctkPluginContext *mPluginContext;
	StateServicePtr mService;
	boost::shared_ptr<ServiceTrackerListener<StateService> > mServiceListener;
};

}

#endif // CXSTATESERVICEPROXY_H
