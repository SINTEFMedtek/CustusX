/*
 * cxVideoService.cpp
 *
 *  Created on: May 31, 2011
 *      Author: christiana
 */

#include "cxVideoService.h"

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
	mIGTLinkConnection.reset(new VideoConnection());
}

VideoService::~VideoService()
{

}

VideoConnectionPtr VideoService::getVideoConnection()
{
	return mIGTLinkConnection;
}

//---------------------------------------------------------

VideoService* videoService()
{
	return VideoService::getInstance();
}


}
