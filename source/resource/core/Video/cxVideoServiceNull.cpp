/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
