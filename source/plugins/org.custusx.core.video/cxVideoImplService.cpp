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

#include "cxVideoImplService.h"

#include <ctkPluginContext.h>
#include "boost/bind.hpp"
#include "cxData.h"

#include "cxRegistrationTransform.h"

#include "cxPlaybackUSAcquisitionVideo.h"
#include "cxVideoConnection.h"
#include "cxBasicVideoSource.h"
#include "cxTypeConversions.h"
#include "cxTrackingService.h"
#include "cxVideoServiceBackend.h"

#include "cxLogger.h"
#include "cxTrackingServiceProxy.h"
#include "cxPatientModelServiceProxy.h"
#include "cxFileHelpers.h"
#include "cxSpaceProviderImpl.h"
#include "cxVideoServiceBackend.h"
#include "cxStreamerService.h"
#include "cxStreamerServiceProxy.h"
#include "cxStreamerServiceNull.h"
#include "cxNullDeleter.h"

namespace cx
{

VideoImplService::VideoImplService(ctkPluginContext *context) :
    mContext(context )
{
	VideoServiceBackendPtr videoBackend;

	PatientModelServicePtr pasm = PatientModelServiceProxy::create(context);
	TrackingServicePtr tracking = TrackingServiceProxy::create(context);
	SpaceProviderPtr spaceProvider;
	spaceProvider.reset(new cx::SpaceProviderImpl(tracking, pasm));
	mBackend = VideoServiceBackend::create(pasm,tracking, spaceProvider, context);

	mEmptyVideoSource.reset(new BasicVideoSource());
	mVideoConnection.reset(new VideoConnection(mBackend));
	mActiveVideoSource = mEmptyVideoSource;
    //mUSAcquisitionVideoPlayback.reset(new USAcquisitionVideoPlayback(mBackend));

	connect(mVideoConnection.get(), &VideoConnection::connected, this, &VideoImplService::autoSelectActiveVideoSource);
	connect(mVideoConnection.get(), &VideoConnection::videoSourcesChanged, this, &VideoImplService::autoSelectActiveVideoSource);
	connect(mVideoConnection.get(), &VideoConnection::fps, this, &VideoImplService::fpsSlot);
	connect(mBackend->getToolManager().get(), &TrackingService::activeToolChanged, this, &VideoImplService::autoSelectActiveVideoSource);
	connect(mVideoConnection.get(), &VideoConnection::connected, this, &VideoImplService::connected);

	this->initServiceListener();
}

VideoImplService::~VideoImplService()
{
	// Disconnect before deleting videoconnection:
	// The VideoConnection might emit events AND call processevents, causing
	// recursive calls back to this during deletion.
	disconnect(mVideoConnection.get(), &VideoConnection::connected, this, &VideoImplService::autoSelectActiveVideoSource);
	disconnect(mVideoConnection.get(), &VideoConnection::videoSourcesChanged, this, &VideoImplService::autoSelectActiveVideoSource);
	disconnect(mVideoConnection.get(), &VideoConnection::fps, this, &VideoImplService::fpsSlot);
	disconnect(mBackend->getToolManager().get(), &TrackingService::activeToolChanged, this, &VideoImplService::autoSelectActiveVideoSource);
	disconnect(mVideoConnection.get(), &VideoConnection::connected, this, &VideoImplService::connected);

	mVideoConnection.reset();
}

StreamerServicePtr VideoImplService::getStreamerService(QString uid)
{
    QList<StreamerServicePtr> services = this->getStreamerServices();
    foreach(StreamerServicePtr service, services)
    {
        if (service->getType()==uid)
        {
            return service;
        }
    }
    return StreamerServicePtr(new StreamerServiceNull, null_deleter());
}

QList<StreamerServicePtr> VideoImplService::getStreamerServices()
{
    QList<StreamerServicePtr> retval;
    QList<StreamerService *> services = mStreamerServiceListener->getServices();
    foreach(StreamerService* service, services)
    {
        if(service)
        {
            StreamerServicePtr temp(new StreamerServiceProxy(mBackend->mContext, service->getName()), null_deleter());
            retval.append(temp);
        }
    }
    return retval;
}

bool VideoImplService::isNull()
{
	return false;
}

void VideoImplService::autoSelectActiveVideoSource()
{
	VideoSourcePtr suggestion = this->getGuessForActiveVideoSource(mActiveVideoSource);
	this->setActiveVideoSource(suggestion->getUid());
}

void VideoImplService::setActiveVideoSource(QString uid)
{
	mActiveVideoSource = mEmptyVideoSource;

	std::vector<VideoSourcePtr> sources = this->getVideoSources();
	for (unsigned i=0; i<sources.size(); ++i)
		if (sources[i]->getUid()==uid)
			mActiveVideoSource = sources[i];

	// set active stream in all probes if stream is present:
	TrackingService::ToolMap tools = mBackend->getToolManager()->getTools();
	for (TrackingService::ToolMap::iterator iter=tools.begin(); iter!=tools.end(); ++iter)
	{
		ProbePtr probe = iter->second->getProbe();
		if (!probe)
			continue;
		if (!probe->getAvailableVideoSources().count(uid))
			continue;
		probe->setActiveStream(uid);
	}

	emit activeVideoSourceChanged();
}

VideoSourcePtr VideoImplService::getGuessForActiveVideoSource(VideoSourcePtr old)
{
	// ask for playback stream:
    foreach(USAcquisitionVideoPlaybackPtr uSAcquisitionVideoPlayback,mUSAcquisitionVideoPlaybacks)
    {
        if (uSAcquisitionVideoPlayback->isActive())
            return uSAcquisitionVideoPlayback->getVideoSource();
    }


	// ask for active stream in first probe:
	ToolPtr tool = mBackend->getToolManager()->getFirstProbe();
	if (tool && tool->getProbe() && tool->getProbe()->getRTSource())
	{
		// keep existing if present
		if (old)
		{
			if (tool->getProbe()->getAvailableVideoSources().count(old->getUid()))
					return old;
		}

		return tool->getProbe()->getRTSource();
	}

	// ask for anything
	std::vector<VideoSourcePtr> allSources = this->getVideoSources();
	// keep existing if present
	if (old)
	{
		if (std::count(allSources.begin(), allSources.end(), old))
				return old;
	}
	if (!allSources.empty())
		return allSources.front();

	// give up: return empty
	return mEmptyVideoSource;
}

VideoSourcePtr VideoImplService::getActiveVideoSource()
{
	return mActiveVideoSource;
}

void VideoImplService::setPlaybackMode(PlaybackTimePtr controller)
{

    QStringList res = getAbsolutePathToFiles( mBackend->getDataManager()->getActivePatientFolder() + "/US_Acq/",QStringList("*.fts"), true);
    QSet<QString> types;
    foreach (const QString &acq, res)
    {
        types.insert(acq.split("_").back());
    }
    foreach(const QString type, types.toList() ){
        USAcquisitionVideoPlaybackPtr test = new USAcquisitionVideoPlayback(mBackend,type);
        mUSAcquisitionVideoPlaybacks.push_back(test  );


        mUSAcquisitionVideoPlaybacks.back()>setTime(controller);
        this->autoSelectActiveVideoSource();

        VideoSourcePtr playbackSource = mUSAcquisitionVideoPlaybacks.back()->getVideoSource();
        TrackingService::ToolMap tools = mBackend->getToolManager()->getTools();
        for (TrackingService::ToolMap::iterator iter=tools.begin(); iter!=tools.end(); ++iter)
        {
            ProbePtr probe = iter->second->getProbe();
            if (!probe)
                continue;
            if (mUSAcquisitionVideoPlaybacks.back()->isActive())
                probe->setRTSource(playbackSource);
            else
                probe->removeRTSource(playbackSource);
        }
        if (mUSAcquisitionVideoPlaybacks.back()->isActive())
            this->setActiveVideoSource(playbackSource->getUid());
        else
            this->autoSelectActiveVideoSource();

        mUSAcquisitionVideoPlaybacks.back()->setRoot(mBackend->getDataManager()->getActivePatientFolder() + "/US_Acq/");

    }
}

std::vector<VideoSourcePtr> VideoImplService::getVideoSources()
{
	std::vector<VideoSourcePtr> retval = mVideoConnection->getVideoSources();
    foreach(USAcquisitionVideoPlaybackPtr uSAcquisitionVideoPlayback,mUSAcquisitionVideoPlaybacks)
    {
        if (uSAcquisitionVideoPlayback->isActive())
            retval.push_back(uSAcquisitionVideoPlayback->getVideoSource());
    }


	return retval;
}

void VideoImplService::fpsSlot(QString source, int val)
{
	if (source==mActiveVideoSource->getUid())
		emit fps(val);
}

void VideoImplService::openConnection()
{
	if (mVideoConnection->isConnected())
		return;

    //StreamerService* service = this->getStreamerService(mConnectionMethod);
    StreamerServicePtr service = this->getStreamerService(mConnectionMethod);
	if (!service)
	{
		reportError(QString("Found no streamer for method [%1]").arg(mConnectionMethod));
		return;
	}

	mVideoConnection->runDirectLinkClient(service);
}

void VideoImplService::closeConnection()
{
	mVideoConnection->disconnectServer();
}

bool VideoImplService::isConnected() const
{
	return mVideoConnection->isConnected();
}

QString VideoImplService::getConnectionMethod()
{
	return mConnectionMethod;
}

void VideoImplService::setConnectionMethod(QString connectionMethod)
{
	if (mConnectionMethod == connectionMethod)
		return;

	if(connectionMethod.isEmpty())
	{
		reportWarning("Trying to set connection method to empty string");
		return;
	}

	mConnectionMethod = connectionMethod;
//	emit connectionMethodChanged();
}

std::vector<TimelineEvent> VideoImplService::getPlaybackEvents()
{
    std::vector<TimelineEvent> retval;
    foreach(USAcquisitionVideoPlaybackPtr uSAcquisitionVideoPlayback,mUSAcquisitionVideoPlaybacks)
    {
        retval.push_back(uSAcquisitionVideoPlayback->getEvents());
    }

    return retval;
}

void VideoImplService::initServiceListener()
{
	mStreamerServiceListener.reset(new ServiceTrackerListener<StreamerService>(
							   mBackend->mContext,
							   boost::bind(&VideoImplService::onStreamerServiceAdded, this, _1),
							   boost::function<void (StreamerService*)>(),
							   boost::bind(&VideoImplService::onStreamerServiceRemoved, this, _1)
							   ));
	mStreamerServiceListener->open();

}

void VideoImplService::onStreamerServiceAdded(StreamerService* service)
{
	if (mConnectionMethod.isEmpty())
		mConnectionMethod = service->getType();

	emit StreamerServiceAdded(service);
}

void VideoImplService::onStreamerServiceRemoved(StreamerService *service)
{
	emit StreamerServiceRemoved(service);
}


} /* namespace cx */
