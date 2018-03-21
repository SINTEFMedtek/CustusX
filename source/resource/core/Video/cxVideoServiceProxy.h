/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIDEOSERVICEPROXY_H
#define CXVIDEOSERVICEPROXY_H

#include "cxResourceExport.h"

#include "cxVideoService.h"
#include "cxServiceTrackerListener.h"
class ctkPluginContext;

namespace cx
{

/**
 * \ingroup cx_resource_core_video
 */
class cxResource_EXPORT VideoServiceProxy : public VideoService
{
public:
	static VideoServicePtr create(ctkPluginContext *pluginContext);
	VideoServiceProxy(ctkPluginContext *pluginContext);
	bool isNull();

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
private:
	void initServiceListener();
	void onVideoServiceAdded(VideoService* service);
	void onVideoServiceRemoved(VideoService *service);

	ctkPluginContext *mPluginContext;
	VideoServicePtr mVideoService;
	boost::shared_ptr<ServiceTrackerListener<VideoService> > mVideoServiceListener;
};
} //cx
#endif // CXVIDEOSERVICEPROXY_H
