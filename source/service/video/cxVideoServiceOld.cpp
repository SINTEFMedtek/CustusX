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

/*
 * cxVideoServiceOld.cpp
 *
 *  \date May 31, 2011
 *      \author christiana
 */

#include "cxVideoServiceOld.h"

#include "ctkPluginContext.h"

#include "cxPlaybackUSAcquisitionVideo.h"
#include "cxVideoConnection.h"
#include "cxVideoConnectionManager.h"
#include "cxBasicVideoSource.h"
#include "cxTypeConversions.h"

#include "cxToolManager.h"
#include "cxVideoServiceBackend.h"
#include "cxLogger.h"

namespace cx
{

//// --------------------------------------------------------
//VideoServiceOldPtr VideoServiceOld::mInstance = NULL; ///< static member
//// --------------------------------------------------------

VideoServiceOldPtr VideoServiceOld::create(VideoServiceBackendPtr backend)
{
	VideoServiceOldPtr retval;
	retval.reset(new VideoServiceOld(backend));
	return retval;
}

VideoServiceOld::VideoServiceOld(VideoServiceBackendPtr videoBackend)
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

VideoServiceOld::~VideoServiceOld()
{
	disconnect(mVideoConnection.get(), SIGNAL(connected(bool)), this, SLOT(autoSelectActiveVideoSource()));
	disconnect(mVideoConnection.get(), SIGNAL(videoSourcesChanged()), this, SLOT(autoSelectActiveVideoSource()));
	mVideoConnection.reset();
}

void VideoServiceOld::autoSelectActiveVideoSource()
{
	VideoSourcePtr suggestion = this->getGuessForActiveVideoSource(mActiveVideoSource);
//	std::cout << "VideoServiceOld::autoSelectActiveVideoSource() " << suggestion->getUid() << std::endl;
	this->setActiveVideoSource(suggestion->getUid());
}

void VideoServiceOld::setActiveVideoSource(QString uid)
{
	mActiveVideoSource = mEmptyVideoSource;

	std::vector<VideoSourcePtr> sources = this->getVideoSources();
	for (unsigned i=0; i<sources.size(); ++i)
		if (sources[i]->getUid()==uid)
			mActiveVideoSource = sources[i];

//	std::cout << "VideoServiceOld::setActiveVideoSource() " << mActiveVideoSource->getUid() << std::endl;

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

VideoSourcePtr VideoServiceOld::getGuessForActiveVideoSource(VideoSourcePtr old)
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

USAcquisitionVideoPlaybackPtr VideoServiceOld::getUSAcquisitionVideoPlayback()
{
	return mUSAcquisitionVideoPlayback;
}

VideoConnectionManagerPtr VideoServiceOld::getVideoConnection()
{
	return mVideoConnection;
}

VideoSourcePtr VideoServiceOld::getActiveVideoSource()
{
	return mActiveVideoSource;
}

void VideoServiceOld::setPlaybackMode(PlaybackTimePtr controller)
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

std::vector<VideoSourcePtr> VideoServiceOld::getVideoSources()
{
	std::vector<VideoSourcePtr> retval = mVideoConnection->getVideoSources();
	if (mUSAcquisitionVideoPlayback->isActive())
		retval.push_back(mUSAcquisitionVideoPlayback->getVideoSource());
	return retval;
}

void VideoServiceOld::fpsSlot(QString source, int val)
{
	if (source==mActiveVideoSource->getUid())
		emit fps(val);
}

VideoServiceBackendPtr VideoServiceOld::getBackend()
{
	return mBackend;
}

}
