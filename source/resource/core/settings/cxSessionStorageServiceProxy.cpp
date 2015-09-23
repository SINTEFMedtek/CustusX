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
