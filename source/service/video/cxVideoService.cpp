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
#include "cxPlaybackUSAcquisitionVideo.h"
#include "cxVideoConnection.h"
#include "cxVideoConnectionManager.h"
#include "cxBasicVideoSource.h"
#include "sscTypeConversions.h"

#include "cxToolManager.h"

namespace cx
{

// --------------------------------------------------------
VideoService* VideoService::mInstance = NULL; ///< static member
// --------------------------------------------------------

void VideoService::initialize()
{
	VideoService::getInstance();
}

void VideoService::shutdown()
{
  delete mInstance;
  mInstance = NULL;
}

VideoService* VideoService::getInstance()
{
	if (!mInstance)
	{
		VideoService::setInstance(new VideoService());
	}
	return mInstance;
}

void VideoService::setInstance(VideoService* instance)
{
	if (mInstance)
	{
		delete mInstance;
	}
	mInstance = instance;
}

VideoService::VideoService()
{
	mEmptyVideoSource.reset(new BasicVideoSource());
	mVideoConnection.reset(new VideoConnectionManager());
	mActiveVideoSource = mEmptyVideoSource;
	mUSAcquisitionVideoPlayback.reset(new USAcquisitionVideoPlayback());

	connect(mVideoConnection.get(), SIGNAL(connected(bool)), this, SLOT(autoSelectActiveVideoSource()));
	connect(mVideoConnection.get(), SIGNAL(videoSourcesChanged()), this, SLOT(autoSelectActiveVideoSource()));
	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(QString)), this, SLOT(autoSelectActiveVideoSource()));
}

VideoService::~VideoService()
{

}

void VideoService::autoSelectActiveVideoSource()
{
	ssc::VideoSourcePtr suggestion = this->getGuessForActiveVideoSource(mActiveVideoSource);
//	std::cout << "VideoService::autoSelectActiveVideoSource() " << suggestion->getUid() << std::endl;
	this->setActiveVideoSource(suggestion->getUid());
}

void VideoService::setActiveVideoSource(QString uid)
{
	std::vector<ssc::VideoSourcePtr> sources = videoService()->getVideoSources();
	for (unsigned i=0; i<sources.size(); ++i)
		if (sources[i]->getUid()==uid)
			mActiveVideoSource = sources[i];

//	std::cout << "VideoService::setActiveVideoSource() " << mActiveVideoSource->getUid() << std::endl;

	// set active stream in all probes if stream is present:
	ssc::ToolManager::ToolMap tools = *ssc::toolManager()->getTools();
	for (ssc::ToolManager::ToolMap::iterator iter=tools.begin(); iter!=tools.end(); ++iter)
	{
		ssc::ProbePtr probe = iter->second->getProbe();
		if (!probe)
			continue;
		if (!probe->getAvailableVideoSources().count(mActiveVideoSource->getUid()))
			continue;
		probe->setActiveStream(mActiveVideoSource->getUid());
	}

	emit activeVideoSourceChanged();
}

ssc::VideoSourcePtr VideoService::getGuessForActiveVideoSource(ssc::VideoSourcePtr old)
{
	// ask for playback stream:
	if (mUSAcquisitionVideoPlayback->isActive())
		return mUSAcquisitionVideoPlayback->getVideoSource();

	// ask for active stream in first probe:
	ssc::ToolPtr tool = ToolManager::getInstance()->findFirstProbe();
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
	std::vector<ssc::VideoSourcePtr> allSources = this->getVideoSources();
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

ssc::VideoSourcePtr VideoService::getActiveVideoSource()
{
	return mActiveVideoSource;
}

void VideoService::setPlaybackMode(PlaybackTimePtr controller)
{
	mUSAcquisitionVideoPlayback->setTime(controller);
	this->autoSelectActiveVideoSource();

	if (mUSAcquisitionVideoPlayback->isActive())
	{
		ssc::VideoSourcePtr playbackSource = mUSAcquisitionVideoPlayback->getVideoSource();
		ssc::ToolManager::ToolMap tools = *ssc::toolManager()->getTools();
		for (ssc::ToolManager::ToolMap::iterator iter=tools.begin(); iter!=tools.end(); ++iter)
		{
			ssc::ProbePtr probe = iter->second->getProbe();
			if (!probe)
				continue;
			probe->setRTSource(playbackSource);
		}
		this->setActiveVideoSource(playbackSource->getUid());
	}
	else
	{
		// clear playback
	}
}

std::vector<ssc::VideoSourcePtr> VideoService::getVideoSources()
{
	std::vector<ssc::VideoSourcePtr> retval = mVideoConnection->getVideoSources();
	if (mUSAcquisitionVideoPlayback->isActive())
		retval.push_back(mUSAcquisitionVideoPlayback->getVideoSource());
	return retval;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

VideoService* videoService()
{
	return VideoService::getInstance();
}

}
