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

#include "cxTrackingServiceProxy.h"

#include <boost/bind.hpp>
#include <ctkPluginContext.h>
#include "cxNullDeleter.h"


namespace cx
{

TrackingServicePtr TrackingServiceProxy::create(ctkPluginContext *pluginContext)
{
	return TrackingServicePtr(new TrackingServiceProxy(pluginContext));
}

TrackingServiceProxy::TrackingServiceProxy(ctkPluginContext *pluginContext) :
	mPluginContext(pluginContext),
	mTrackingService(TrackingService::getNullObject())
{
	this->initServiceListener();
}

void TrackingServiceProxy::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<TrackingService>(
								 mPluginContext,
								 boost::bind(&TrackingServiceProxy::onServiceAdded, this, _1),
								 boost::function<void (TrackingService*)>(),
								 boost::bind(&TrackingServiceProxy::onServiceRemoved, this, _1)
								 ));
	mServiceListener->open();
}
void TrackingServiceProxy::onServiceAdded(TrackingService* service)
{
	mTrackingService.reset(service, null_deleter());

	connect(mTrackingService.get(), &TrackingService::stateChanged, this, &TrackingService::stateChanged);
	connect(mTrackingService.get(), &TrackingService::activeToolChanged, this, &TrackingService::activeToolChanged);

    emit stateChanged();
    emit activeToolChanged(mTrackingService->getActiveTool()->getUid());
}

void TrackingServiceProxy::onServiceRemoved(TrackingService *service)
{
	disconnect(mTrackingService.get(), &TrackingService::stateChanged, this, &TrackingService::stateChanged);
	disconnect(mTrackingService.get(), &TrackingService::activeToolChanged, this, &TrackingService::activeToolChanged);

	mTrackingService = TrackingService::getNullObject();

	emit stateChanged();
	emit activeToolChanged("");
}

bool TrackingServiceProxy::isNull()
{
	return mTrackingService->isNull();
}

Tool::State TrackingServiceProxy::getState() const
{
	return mTrackingService->getState();
}

void TrackingServiceProxy::setState(const Tool::State val)
{
	mTrackingService->setState(val);
}

ToolPtr TrackingServiceProxy::getTool(const QString& uid)
{
	return mTrackingService->getTool(uid);
}

ToolPtr TrackingServiceProxy::getActiveTool()
{
	return mTrackingService->getActiveTool();
}

void TrackingServiceProxy::setActiveTool(const QString& uid)
{
	mTrackingService->setActiveTool(uid);
}

ToolPtr TrackingServiceProxy::getFirstProbe()
{
	return mTrackingService->getFirstProbe();
}

ToolPtr TrackingServiceProxy::getReferenceTool() const
{
	return mTrackingService->getReferenceTool();
}

ToolPtr TrackingServiceProxy::getManualTool()
{
	return mTrackingService->getManualTool();
}

TrackingService::ToolMap TrackingServiceProxy::getTools()
{
	return mTrackingService->getTools();
}

bool TrackingServiceProxy::isPlaybackMode() const
{
	return mTrackingService->isPlaybackMode();
}

void TrackingServiceProxy::setPlaybackMode(PlaybackTimePtr controller)
{
	mTrackingService->setPlaybackMode(controller);
}

SessionToolHistoryMap TrackingServiceProxy::getSessionHistory(double startTime, double stopTime)
{
	return mTrackingService->getSessionHistory(startTime, stopTime);
}

void TrackingServiceProxy::runDummyTool(DummyToolPtr tool)
{
	mTrackingService->runDummyTool(tool);
}

TrackerConfigurationPtr TrackingServiceProxy::getConfiguration()
{
	return mTrackingService->getConfiguration();
}


void TrackingServiceProxy::installTrackingSystem(TrackingSystemServicePtr system)
{
	mTrackingService->installTrackingSystem(system);
}

void TrackingServiceProxy::unInstallTrackingSystem(TrackingSystemServicePtr system)
{
	mTrackingService->unInstallTrackingSystem(system);
}

std::vector<TrackingSystemServicePtr> TrackingServiceProxy::getTrackingSystems()
{
	return mTrackingService->getTrackingSystems();
}



} //cx
