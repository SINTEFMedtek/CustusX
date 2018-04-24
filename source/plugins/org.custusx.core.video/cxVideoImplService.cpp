/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

    connect(mVideoConnection.get(), &VideoConnection::connected, this, &VideoImplService::autoSelectActiveVideoSource);
    connect(mVideoConnection.get(), &VideoConnection::videoSourcesChanged, this, &VideoImplService::autoSelectActiveVideoSource);
    connect(mVideoConnection.get(), &VideoConnection::fps, this, &VideoImplService::fpsSlot);
    connect(mBackend->tracking().get(), &TrackingService::activeToolChanged, this, &VideoImplService::autoSelectActiveVideoSource);
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
    disconnect(mBackend->tracking().get(), &TrackingService::activeToolChanged, this, &VideoImplService::autoSelectActiveVideoSource);
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
    TrackingService::ToolMap tools = mBackend->tracking()->getTools();
    for (TrackingService::ToolMap::iterator iter=tools.begin(); iter!=tools.end(); ++iter)
    {
        ProbePtr probe = iter->second->getProbe();
        if (!probe)
            continue;
        if (!probe->getAvailableVideoSources().count(uid)){
            report("No active streams");
            continue;
        }
        probe->setActiveStream(uid);
    }

    emit activeVideoSourceChanged();
}

VideoSourcePtr VideoImplService::getGuessForActiveVideoSource(VideoSourcePtr old)
{

    if(old && old->getUid().contains("playback"))
        return old;

    QStringList nameFilters;
    nameFilters << "TissueAngio.fts" << "TissueFlow.fts" << "ScanConverted.fts";
    // ask for playback stream:
    foreach(USAcquisitionVideoPlaybackPtr uSAcquisitionVideoPlayback,mUSAcquisitionVideoPlaybacks)
    {
        if (uSAcquisitionVideoPlayback->isActive() && nameFilters.contains(uSAcquisitionVideoPlayback->getType()) )
            return uSAcquisitionVideoPlayback->getVideoSource();
     }

    // ask for playback stream:
    foreach(USAcquisitionVideoPlaybackPtr uSAcquisitionVideoPlayback,mUSAcquisitionVideoPlaybacks)
    {
        if (uSAcquisitionVideoPlayback->isActive())
            return uSAcquisitionVideoPlayback->getVideoSource();
    }

    // ask for active stream in first probe:
    ToolPtr tool = mBackend->tracking()->getFirstProbe();
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

    std::vector<VideoSourcePtr> allSources = this->getVideoSources();
    // keep existing if present
    if (old)
    {
        if (std::count(allSources.begin(), allSources.end(), old))
                return old;
    }
    // ask for anything
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
    USAcquisitionVideoPlaybackPtr tempUSAcquisitionVideoPlayback;
    foreach(const QString type, types.toList() ){

        tempUSAcquisitionVideoPlayback.reset(new USAcquisitionVideoPlayback(mBackend,type));
        mUSAcquisitionVideoPlaybacks.push_back(tempUSAcquisitionVideoPlayback  );


        mUSAcquisitionVideoPlaybacks.back()->setTime(controller);

        VideoSourcePtr playbackSource = mUSAcquisitionVideoPlaybacks.back()->getVideoSource();
        TrackingService::ToolMap tools = mBackend->tracking()->getTools();
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
        mUSAcquisitionVideoPlaybacks.back()->setRoot(mBackend->getDataManager()->getActivePatientFolder() + "/US_Acq/");
    }
    this->autoSelectActiveVideoSource();
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
	emit connectionMethodChanged();
}

std::vector<TimelineEvent> VideoImplService::getPlaybackEvents()
{
    std::vector<TimelineEvent> retval;
    foreach(USAcquisitionVideoPlaybackPtr uSAcquisitionVideoPlayback,mUSAcquisitionVideoPlaybacks)
    {
        std::vector<TimelineEvent> events = uSAcquisitionVideoPlayback->getEvents();
        retval.reserve(retval.size() + events.size());
        retval.insert( retval.end(), events.begin(), events.end() );
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
