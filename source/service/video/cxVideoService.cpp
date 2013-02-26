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
	mIGTLinkConnection.reset(new VideoConnectionManager());
	mActiveVideoSource = mIGTLinkConnection->getVideoSource();
	mUSAcquisitionVideoPlayback.reset(new USAcquisitionVideoPlayback());
//	mGrabberDirectLinkVideoSource.reset(new OpenIGTLinkDirectLinkRTSource());

	connect(mIGTLinkConnection.get(), SIGNAL(connected(bool)), this, SIGNAL(activeVideoSourceChanged()));
}

VideoService::~VideoService()
{

}

USAcquisitionVideoPlaybackPtr VideoService::getUSAcquisitionVideoPlayback()
{
	return mUSAcquisitionVideoPlayback;
}

VideoConnectionManagerPtr VideoService::getIGTLinkVideoConnection()
{
	return mIGTLinkConnection;
}

ssc::VideoSourcePtr VideoService::getActiveVideoSource()
{
	return mActiveVideoSource;
}

void VideoService::setPlaybackMode(PlaybackTimePtr controller)
{
	if (controller)
	{
		// turn on playback
		mUSAcquisitionVideoPlayback->setTime(controller);
		mActiveVideoSource = mUSAcquisitionVideoPlayback->getVideoSource();
		emit activeVideoSourceChanged();
	}
	else
	{
		// turn off playback
		mUSAcquisitionVideoPlayback->setTime(controller);
		mActiveVideoSource = mIGTLinkConnection->getVideoSource();
		emit activeVideoSourceChanged();
	}
}

//---------------------------------------------------------

VideoService* videoService()
{
	return VideoService::getInstance();
}


}
