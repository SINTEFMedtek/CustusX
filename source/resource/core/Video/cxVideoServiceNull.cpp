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

#include "cxVideoServiceNull.h"

#include "cxStreamerService.h"
#include "cxStreamerServiceNull.h"
#include "cxNullDeleter.h"
#include "cxBasicVideoSource.h"

namespace cx
{

void VideoServiceNull::setActiveVideoSource(QString uid)
{
}

VideoSourcePtr VideoServiceNull::getActiveVideoSource()
{
	return VideoSourcePtr(new BasicVideoSource());
}

std::vector<VideoSourcePtr> VideoServiceNull::getVideoSources()
{
	return std::vector<VideoSourcePtr>();
}

void VideoServiceNull::setConnectionMethod(QString connectionMethod)
{
}

QString VideoServiceNull::getConnectionMethod()
{
	return "";
}

void VideoServiceNull::openConnection()
{
}

void VideoServiceNull::closeConnection()
{
}

bool VideoServiceNull::isConnected() const
{
	return false;
}

void VideoServiceNull::setPlaybackMode(PlaybackTimePtr controller)
{
}

std::vector<TimelineEvent> VideoServiceNull::getPlaybackEvents()
{
	return std::vector<TimelineEvent>();
}

StreamerServicePtr VideoServiceNull::getStreamerService(QString uid)
{
    return StreamerServicePtr(new StreamerServiceNull() , null_deleter());
}

QList<StreamerServicePtr> VideoServiceNull::getStreamerServices()
{
	printWarning();
    return QList<StreamerServicePtr>();
}
bool VideoServiceNull::isNull()
{
	return true;
}

void VideoServiceNull::printWarning()
{
//	reportWarning("Trying to use VideoServiceNull. Is VideoService (org.custusx.core.video) disabled?");
}

} //cx
