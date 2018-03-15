/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

