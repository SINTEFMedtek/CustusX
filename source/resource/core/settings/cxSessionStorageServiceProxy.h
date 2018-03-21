/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSESSIONSTORAGESERVICEPROXY_H
#define CXSESSIONSTORAGESERVICEPROXY_H

#include "cxResourceExport.h"
#include "cxSessionStorageService.h"

#include "cxServiceTrackerListener.h"


namespace cx
{

/**
 * \ingroup cx_resource_core_settings
 */
class cxResource_EXPORT SessionStorageServiceProxy : public SessionStorageService
{
public:
    static SessionStorageServicePtr create(ctkPluginContext *pluginContext);
	SessionStorageServiceProxy(ctkPluginContext *pluginContext);

	virtual ~SessionStorageServiceProxy() {}
	virtual void load(QString dir);
	virtual void save();
	virtual void clear();
	virtual bool isValid() const;
	virtual QString getRootFolder() const;
	virtual bool isNull() const;

private:
	void initServiceListener();
	void onServiceAdded(SessionStorageService* service);
	void onServiceRemoved(SessionStorageService *service);

	ctkPluginContext *mPluginContext;
	SessionStorageServicePtr mService;
	boost::shared_ptr<ServiceTrackerListener<SessionStorageService> > mServiceListener;
};

} // namespace cx

#endif // CXSESSIONSTORAGESERVICEPROXY_H
