/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
