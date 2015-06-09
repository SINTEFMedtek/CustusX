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

#ifndef CXVIDEOSERVICENULL_H
#define CXVIDEOSERVICENULL_H

#include "cxResourceExport.h"

#include "cxVideoService.h"

namespace cx
{

/**
 * \ingroup cx_resource_core_video
 */
class cxResource_EXPORT VideoServiceNull : public VideoService
{
public:
	virtual ~VideoServiceNull() {}

	virtual void setActiveVideoSource(QString uid);
	virtual VideoSourcePtr getActiveVideoSource();
	virtual std::vector<VideoSourcePtr> getVideoSources();

	virtual void setConnectionMethod(QString connectionMethod);
	virtual QString getConnectionMethod();
	virtual void openConnection();
	virtual void closeConnection();
	virtual bool isConnected() const;

	virtual void setPlaybackMode(PlaybackTimePtr controller);
	virtual std::vector<TimelineEvent> getPlaybackEvents();

    virtual StreamerServicePtr getStreamerService(QString uid);
    virtual QList<StreamerServicePtr> getStreamerServices();

	virtual bool isNull();
private:
	void printWarning();
};
} //cx
#endif // CXVIDEOSERVICENULL_H
