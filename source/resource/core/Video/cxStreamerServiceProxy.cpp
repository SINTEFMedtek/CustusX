/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxStreamerServiceProxy.h"

#include <boost/bind.hpp>
#include "cxNullDeleter.h"

namespace cx
{
StreamerServiceProxy::StreamerServiceProxy(ctkPluginContext *context, QString name) :
    mPluginContext(context),
    mServiceName(name),
    mStreamerService(StreamerService::getNullObject())
{
    this->initServiceListener();
}

QString StreamerServiceProxy::getName()
{
    return mStreamerService->getName();
}

QString StreamerServiceProxy::getType() const
{
	return mStreamerService->getType();
}

void StreamerServiceProxy::stop()
{
	return mStreamerService->stop();
}

std::vector<PropertyPtr> StreamerServiceProxy::getSettings(QDomElement root)
{
    return mStreamerService->getSettings(root);
}

StreamerPtr StreamerServiceProxy::createStreamer(QDomElement root)
{
    return mStreamerService->createStreamer(root);
}

void StreamerServiceProxy::initServiceListener()
{
    mServiceListener.reset(new ServiceTrackerListener<StreamerService>(
                                 mPluginContext,
                                 boost::bind(&StreamerServiceProxy::onServiceAdded, this, _1),
                                 boost::function<void (StreamerService*)>(),
                                 boost::bind(&StreamerServiceProxy::onServiceRemoved, this, _1)
                                 ));
    mServiceListener->open();
}

void StreamerServiceProxy::onServiceAdded(StreamerService* service)
{
    if(service && service->getName() == mServiceName)
        mStreamerService.reset(service, null_deleter());
}

void StreamerServiceProxy::onServiceRemoved(StreamerService *service)
{
    if(service && (service->getName() == mServiceName))
        mStreamerService = StreamerService::getNullObject();
}
} //end namespace cx
