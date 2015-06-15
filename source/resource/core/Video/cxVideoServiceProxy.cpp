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
//	connect(service, &VideoService::connectionMethodChanged, this, &VideoService::connectionMethodChanged);
	connect(service, &VideoService::activeVideoSourceChanged, this, &VideoService::activeVideoSourceChanged);
	connect(service, &VideoService::fps, this, &VideoService::fps);

	emit activeVideoSourceChanged();
}

void VideoServiceProxy::onVideoServiceRemoved(VideoService *service)
{
	disconnect(service, &VideoService::connected, this, &VideoService::connected);
//	disconnect(service, &VideoService::connectionMethodChanged, this, &VideoService::connectionMethodChanged);
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
