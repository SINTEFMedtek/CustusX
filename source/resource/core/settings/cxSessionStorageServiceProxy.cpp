/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSessionStorageServiceProxy.h"
#include "boost/bind.hpp"
#include "cxNullDeleter.h"

namespace cx
{

SessionStorageServicePtr SessionStorageServiceProxy::create(ctkPluginContext *pluginContext)
{
	return SessionStorageServicePtr(new SessionStorageServiceProxy(pluginContext));
}

SessionStorageServiceProxy::SessionStorageServiceProxy(ctkPluginContext *pluginContext) :
	mPluginContext(pluginContext),
	mService(SessionStorageService::getNullObject())
{
	this->initServiceListener();
}

void SessionStorageServiceProxy::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<SessionStorageService>(
								 mPluginContext,
								 boost::bind(&SessionStorageServiceProxy::onServiceAdded, this, _1),
								 boost::function<void (SessionStorageService*)>(),
								 boost::bind(&SessionStorageServiceProxy::onServiceRemoved, this, _1)
								 ));
	mServiceListener->open();
}

void SessionStorageServiceProxy::onServiceAdded(SessionStorageService* service)
{
	mService.reset(service, null_deleter());

	connect(mService.get(), &SessionStorageService::cleared, this, &SessionStorageService::cleared);
	connect(mService.get(), &SessionStorageService::sessionChanged, this, &SessionStorageService::sessionChanged);
	connect(mService.get(), &SessionStorageService::isSaving, this, &SessionStorageService::isSaving);
	connect(mService.get(), &SessionStorageService::isLoading, this, &SessionStorageService::isLoading);
	connect(mService.get(), &SessionStorageService::isLoadingSecond, this, &SessionStorageService::isLoadingSecond);

	emit sessionChanged();
}

void SessionStorageServiceProxy::onServiceRemoved(SessionStorageService *service)
{
	disconnect(mService.get(), &SessionStorageService::cleared, this, &SessionStorageService::cleared);
	disconnect(mService.get(), &SessionStorageService::sessionChanged, this, &SessionStorageService::sessionChanged);
	disconnect(mService.get(), &SessionStorageService::isSaving, this, &SessionStorageService::isSaving);
	disconnect(mService.get(), &SessionStorageService::isLoading, this, &SessionStorageService::isLoading);
	disconnect(mService.get(), &SessionStorageService::isLoadingSecond, this, &SessionStorageService::isLoadingSecond);

	mService = SessionStorageServiceProxy::getNullObject();

	emit cleared();
	emit sessionChanged();
}

void SessionStorageServiceProxy::load(QString dir)
{
	mService->load(dir);
}

void SessionStorageServiceProxy::save()
{
	mService->save();
}

void SessionStorageServiceProxy::clear()
{
	mService->clear();
}

bool SessionStorageServiceProxy::isValid() const
{
	return mService->isValid();
}

QString SessionStorageServiceProxy::getRootFolder() const
{
	return 	mService->getRootFolder();
}

bool SessionStorageServiceProxy::isNull() const
{
	return false;
}

}
