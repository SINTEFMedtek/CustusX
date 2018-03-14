/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxVideoPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxVideoImplService.h"
#include "cxRegisteredService.h"
#include "cxIGTLinkStreamerService.h"
#include "cxOpenCVStreamerService.h"
#include "cxImageFileStreamerService.h"

namespace cx
{

VideoPluginActivator::VideoPluginActivator()
{
}

VideoPluginActivator::~VideoPluginActivator()
{
}

void VideoPluginActivator::start(ctkPluginContext* context)
{
	mRegistration = RegisteredService::create<VideoImplService>(context, VideoService_iid);
	// the first added streamer will be the default selected
	mOpenCVStreamer = RegisteredService::create<OpenCVStreamerService>(context, StreamerService_iid);
	mIGTLinkStreamer = RegisteredService::create<IGTLinkStreamerService>(context, StreamerService_iid);
	mImageFileStreamer = RegisteredService::create<ImageFileStreamerService>(context, StreamerService_iid);
}

void VideoPluginActivator::stop(ctkPluginContext* context)
{
	mIGTLinkStreamer.reset();
	mRegistration.reset();
	mOpenCVStreamer.reset();
	mImageFileStreamer.reset();
	Q_UNUSED(context);
}

} // namespace cx
