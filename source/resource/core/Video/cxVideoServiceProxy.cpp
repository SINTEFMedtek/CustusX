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
#include "cxReporter.h"

namespace cx
{

VideoServiceProxy::VideoServiceProxy(ctkPluginContext *pluginContext) :
	mPluginContext(pluginContext),
	mVideoService(VideoService::getNullObject())
{
	this->initServiceListener();
}

void VideoServiceProxy::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<VideoService>(
								 mPluginContext,
								 boost::bind(&VideoServiceProxy::onServiceAdded, this, _1),
								 boost::function<void (VideoService*)>(),
								 boost::bind(&VideoServiceProxy::onServiceRemoved, this, _1)
								 ));
	mServiceListener->open();
}
void VideoServiceProxy::onServiceAdded(VideoService* service)
{
	mVideoService.reset(service, null_deleter());
//	connect(mVideoService.get(), SIGNAL(fixedDataChanged(QString)), this, SIGNAL(fixedDataChanged(QString)));
//	connect(mVideoService.get(), SIGNAL(movingDataChanged(QString)), this, SIGNAL(movingDataChanged(QString)));
	if(mVideoService->isNull())
		reportWarning("VideoServiceProxy::onServiceAdded mVideoService->isNull()");
}

void VideoServiceProxy::onServiceRemoved(VideoService *service)
{
//	disconnect(service, SIGNAL(fixedDataChanged(QString)), this, SIGNAL(fixedDataChanged(QString)));
//	disconnect(service, SIGNAL(movingDataChanged(QString)), this, SIGNAL(movingDataChanged(QString)));
	mVideoService = VideoService::getNullObject();
}

bool VideoServiceProxy::isNull()
{
	return mVideoService->isNull();
}
} //cx
