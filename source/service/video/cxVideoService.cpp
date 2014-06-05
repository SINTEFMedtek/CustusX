// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

/*
 * cxVideoService.cpp
 *
 *  \date May 31, 2011
 *      \author christiana
 */

#include "cxVideoService.h"

#include "ctkPluginContext.h"

#include "cxPlaybackUSAcquisitionVideo.h"
#include "cxVideoConnection.h"
#include "cxVideoConnectionManager.h"
#include "cxBasicVideoSource.h"
#include "cxTypeConversions.h"

#include "cxToolManager.h"
#include "cxVideoServiceBackend.h"

namespace cx
{

//// --------------------------------------------------------
//VideoServicePtr VideoService::mInstance = NULL; ///< static member
//// --------------------------------------------------------

VideoServicePtr VideoService::create(VideoServiceBackendPtr backend)
{
	VideoServicePtr retval;
	retval.reset(new VideoService(backend));
	return retval;
}

//void VideoService::initialize(VideoServiceBackendPtr videoBackend)
//{
//	VideoService::setInstance(new VideoService(videoBackend));
//	VideoService::getInstance();
//}

//void VideoService::shutdown()
//{
//  delete mInstance;
//  mInstance = NULL;
//}

//VideoServicePtr VideoService::getInstance()
//{
//	if (!mInstance)
//	{
//		VideoService::setInstance(new VideoService(VideoServiceBackendPtr()));
//	}
//	return mInstance;
//}

//void VideoService::setInstance(VideoServicePtr instance)
//{
//	if (mInstance)
//	{
//		delete mInstance;
//	}
//	mInstance = instance;
//}

VideoService::VideoService(VideoServiceBackendPtr videoBackend)
{
	mBackend = videoBackend;
	mEmptyVideoSource.reset(new BasicVideoSource());
	mVideoConnection.reset(new VideoConnectionManager(mBackend));
	mActiveVideoSource = mEmptyVideoSource;
	mUSAcquisitionVideoPlayback.reset(new USAcquisitionVideoPlayback(mBackend));

	connect(mVideoConnection.get(), SIGNAL(connected(bool)), this, SLOT(autoSelectActiveVideoSource()));
	connect(mVideoConnection.get(), SIGNAL(videoSourcesChanged()), this, SLOT(autoSelectActiveVideoSource()));
	connect(mVideoConnection.get(), SIGNAL(fps(QString, int)), this, SLOT(fpsSlot(QString, int)));
	connect(mBackend->getToolManager().get(), SIGNAL(dominantToolChanged(QString)), this, SLOT(autoSelectActiveVideoSource()));
}

VideoService::~VideoService()
{
	disconnect(mVideoConnection.get(), SIGNAL(connected(bool)), this, SLOT(autoSelectActiveVideoSource()));
	disconnect(mVideoConnection.get(), SIGNAL(videoSourcesChanged()), this, SLOT(autoSelectActiveVideoSource()));
	mVideoConnection.reset();
}

void VideoService::autoSelectActiveVideoSource()
{
	VideoSourcePtr suggestion = this->getGuessForActiveVideoSource(mActiveVideoSource);
//	std::cout << "VideoService::autoSelectActiveVideoSource() " << suggestion->getUid() << std::endl;
	this->setActiveVideoSource(suggestion->getUid());
}

void VideoService::setActiveVideoSource(QString uid)
{
	mActiveVideoSource = mEmptyVideoSource;

	std::vector<VideoSourcePtr> sources = this->getVideoSources();
	for (unsigned i=0; i<sources.size(); ++i)
		if (sources[i]->getUid()==uid)
			mActiveVideoSource = sources[i];

//	std::cout << "VideoService::setActiveVideoSource() " << mActiveVideoSource->getUid() << std::endl;

	// set active stream in all probes if stream is present:
	ToolManager::ToolMap tools = mBackend->getToolManager()->getTools();
	for (ToolManager::ToolMap::iterator iter=tools.begin(); iter!=tools.end(); ++iter)
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

VideoSourcePtr VideoService::getGuessForActiveVideoSource(VideoSourcePtr old)
{
	// ask for playback stream:
	if (mUSAcquisitionVideoPlayback->isActive())
		return mUSAcquisitionVideoPlayback->getVideoSource();

	// ask for active stream in first probe:
	ToolPtr tool = mBackend->getToolManager()->findFirstProbe();
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

USAcquisitionVideoPlaybackPtr VideoService::getUSAcquisitionVideoPlayback()
{
	return mUSAcquisitionVideoPlayback;
}

VideoConnectionManagerPtr VideoService::getVideoConnection()
{
	return mVideoConnection;
}

VideoSourcePtr VideoService::getActiveVideoSource()
{
	return mActiveVideoSource;
}

ctkPluginContext* VideoService::getPluginContext()
{
	return mBackend->getPluginContext();
}

void VideoService::setPlaybackMode(PlaybackTimePtr controller)
{
	mUSAcquisitionVideoPlayback->setTime(controller);
	this->autoSelectActiveVideoSource();

	VideoSourcePtr playbackSource = mUSAcquisitionVideoPlayback->getVideoSource();
	ToolManager::ToolMap tools = mBackend->getToolManager()->getTools();
	for (ToolManager::ToolMap::iterator iter=tools.begin(); iter!=tools.end(); ++iter)
	{
		ProbePtr probe = iter->second->getProbe();
		if (!probe)
			continue;
		if (mUSAcquisitionVideoPlayback->isActive())
			probe->setRTSource(playbackSource);
		else
			probe->removeRTSource(playbackSource);
	}
	if (mUSAcquisitionVideoPlayback->isActive())
		this->setActiveVideoSource(playbackSource->getUid());
	else
		this->autoSelectActiveVideoSource();
}

std::vector<VideoSourcePtr> VideoService::getVideoSources()
{
	std::vector<VideoSourcePtr> retval = mVideoConnection->getVideoSources();
	if (mUSAcquisitionVideoPlayback->isActive())
		retval.push_back(mUSAcquisitionVideoPlayback->getVideoSource());
	return retval;
}

void VideoService::fpsSlot(QString source, int val)
{
	if (source==mActiveVideoSource->getUid())
		emit fps(val);
}

VideoServiceBackendPtr VideoService::getBackend()
{
	return mBackend;
}

}
