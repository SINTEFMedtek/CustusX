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

#ifndef CXTRACKINGIMPLSERVICE_H_
#define CXTRACKINGIMPLSERVICE_H_

#include "cxVideoService.h"
#include "org_custusx_core_video_Export.h"
#include "cxServiceTrackerListener.h"

class ctkPluginContext;

namespace cx
{
typedef boost::shared_ptr<class VideoConnection> VideoConnectionPtr;
typedef boost::shared_ptr<class VideoServiceBackend> VideoServiceBackendPtr;

/**
 * Implementation of VideoService.
 *
 * \ingroup org_custusx_core_video
 *
 * \date 2014-09-19
 * \author Ole Vegard Solberg
 */
class org_custusx_core_video_EXPORT VideoImplService : public VideoService
{
	Q_INTERFACES(cx::VideoService)
public:
	VideoImplService(ctkPluginContext* context);
	virtual ~VideoImplService();
    virtual StreamerServicePtr getStreamerService(QString uid);
    virtual QList<StreamerServicePtr> getStreamerServices();

	virtual bool isNull();

	virtual VideoSourcePtr getActiveVideoSource();
	virtual void setActiveVideoSource(QString uid);
	virtual std::vector<VideoSourcePtr> getVideoSources();

	virtual void setPlaybackMode(PlaybackTimePtr controller);
	virtual std::vector<TimelineEvent> getPlaybackEvents();

	virtual QString getConnectionMethod();
	virtual void setConnectionMethod(QString connectionMethod);
	virtual void openConnection();
	virtual void closeConnection();
	virtual bool isConnected() const;

private slots:
	/** Autoselect the active VideoSource
	  *
	  * Call when video source configuration has changed. The active
	  * Video source will automatically be determined by calling
	  * autoGuessVideoSource().
	  */
	void autoSelectActiveVideoSource();
	void fpsSlot(QString source, int val);

private:
	/** Find the best guess for active VideoSource
	  *
	  * Select from the following in that priority:
	  *  - playback sources
	  *  - active probe sources
	  *  - other probe sources
	  *  - free sources (not connected to probe)
	  *  - empty source
	  *
	  * Within each group, keep existing active if it already belongs
	  * to that group.
	  *
	  */
	VideoSourcePtr getGuessForActiveVideoSource(VideoSourcePtr old);

	QString mConnectionMethod;
	VideoConnectionPtr mVideoConnection;

	VideoSourcePtr mActiveVideoSource;
	VideoSourcePtr mEmptyVideoSource;
    std::vector<USAcquisitionVideoPlaybackPtr> mUSAcquisitionVideoPlaybacks;
	VideoServiceBackendPtr mBackend;

	void initServiceListener();
	void onStreamerServiceAdded(StreamerService *service);
	void onStreamerServiceRemoved(StreamerService *service);

	boost::shared_ptr<ServiceTrackerListener<StreamerService> > mStreamerServiceListener;

private:
	ctkPluginContext *mContext;
};
typedef boost::shared_ptr<VideoImplService> VideoImplServicePtr;

} /* namespace cx */

#endif /* CXTRACKINGIMPLSERVICE_H_ */

