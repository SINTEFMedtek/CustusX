/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
