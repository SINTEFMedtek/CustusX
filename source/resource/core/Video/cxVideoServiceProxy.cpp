/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxVideoServiceProxy.h"

#include <boost/bind.hpp>
#include <ctkPluginContext.h>
#include "cxNullDeleter.h"

#include "cxStreamerService.h"
//#include "cxTypeConversions.h"

namespace cx
{

VideoServicePtr VideoServiceProxy::create(ctkPluginContext *pluginContext)
{
	return VideoServicePtr(new VideoServiceProxy(pluginContext));
}

VideoServiceProxy::VideoServiceProxy(ctkPluginContext *pluginContext) :
	mPluginContext(pluginContext),
	mVideoService(VideoService::getNullObject())
{
	this->initServiceListener();
}

void VideoServiceProxy::initServiceListener()
{
	mVideoServiceListener.reset(new ServiceTrackerListener<VideoService>(
								 mPluginContext,
								 boost::bind(&VideoServiceProxy::onVideoServiceAdded, this, _1),
								 boost::function<void (VideoService*)>(),
								 boost::bind(&VideoServiceProxy::onVideoServiceRemoved, this, _1)
								 ));
	mVideoServiceListener->open();
}

void VideoServiceProxy::onVideoServiceAdded(VideoService* service)
{
	mVideoService.reset(service, null_deleter());

	connect(service, &VideoService::connected, this, &VideoService::connected);
	connect(service, &VideoService::connectionMethodChanged, this, &VideoService::connectionMethodChanged);
	connect(service, &VideoService::activeVideoSourceChanged, this, &VideoService::activeVideoSourceChanged);
	connect(service, &VideoService::fps, this, &VideoService::fps);

	emit activeVideoSourceChanged();
}

void VideoServiceProxy::onVideoServiceRemoved(VideoService *service)
{
	disconnect(service, &VideoService::connected, this, &VideoService::connected);
	disconnect(service, &VideoService::connectionMethodChanged, this, &VideoService::connectionMethodChanged);
	disconnect(service, &VideoService::activeVideoSourceChanged, this, &VideoService::activeVideoSourceChanged);
	disconnect(service, &VideoService::fps, this, &VideoService::fps);

	mVideoService = VideoService::getNullObject();

	emit connected(false);
	emit activeVideoSourceChanged();
}

bool VideoServiceProxy::isNull()
{
	return mVideoService->isNull();
}

StreamerServicePtr VideoServiceProxy::getStreamerService(QString uid)
{
    return mVideoService->getStreamerService(uid);
}

QList<StreamerServicePtr> VideoServiceProxy::getStreamerServices()
{
	return mVideoService->getStreamerServices();
}

void VideoServiceProxy::setActiveVideoSource(QString uid)
{
	mVideoService->setActiveVideoSource(uid);
}

VideoSourcePtr VideoServiceProxy::getActiveVideoSource()
{
	return mVideoService->getActiveVideoSource();
}

std::vector<VideoSourcePtr> VideoServiceProxy::getVideoSources()
{
	return mVideoService->getVideoSources();
}

void VideoServiceProxy::setConnectionMethod(QString connectionMethod)
{
	mVideoService->setConnectionMethod(connectionMethod);
}

QString VideoServiceProxy::getConnectionMethod()
{
	return mVideoService->getConnectionMethod();
}

void VideoServiceProxy::openConnection()
{
	mVideoService->openConnection();
}

void VideoServiceProxy::closeConnection()
{
	mVideoService->closeConnection();
}

bool VideoServiceProxy::isConnected() const
{
	return mVideoService->isConnected();
}

void VideoServiceProxy::setPlaybackMode(PlaybackTimePtr controller)
{
	mVideoService->setPlaybackMode(controller);
}

std::vector<TimelineEvent> VideoServiceProxy::getPlaybackEvents()
{
    return mVideoService->getPlaybackEvents();
}



} //cx
