/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIDEOSERVICE_H
#define CXVIDEOSERVICE_H

#include "cxResourceExport.h"

#include <QObject>
#include <boost/shared_ptr.hpp>
#include "cxPlaybackTime.h"

#define VideoService_iid "cx::VideoService"

namespace cx
{
class StreamerService;

typedef boost::shared_ptr<class VideoService> VideoServicePtr;
typedef boost::shared_ptr<class StreamerService> StreamerServicePtr;
typedef boost::shared_ptr<class VideoSource> VideoSourcePtr;
typedef boost::shared_ptr<class USAcquisitionVideoPlayback> USAcquisitionVideoPlaybackPtr;
typedef boost::shared_ptr<class PlaybackTime> PlaybackTimePtr;

/**
 * \brief Provides access to all video sources in the
 * system, and connection stuff for the sources.
 * \ingroup cx_resource_core_video
 *
 * \image html videoservice_simple.png "VideoService Overview"
 *
 * VideoService provides a simple image stream class to its users, hiding
 * details. VideoService receives images over OpenIGTLink from an external
 * server.
 *
 * Time synchronization using Network Time Protocol (NTP) and temporal calibration.
 *
 * \image html cxArchitecture_rtstream.png "VideoService and Video Pipeline"
 *
 * The diagram displays all major classes concerned with realtime streaming.
 * VideoSource is the core interface that provides connection to a realtime
 * streaming source.
 *
 * VideoSource has to main users: Rendering to screen and recording to disk.
 * VideoGraphics contains the visualization functionality that the Reps uses
 * when rendering in the Views.  VideoGraphics needs a Probe to provide
 * position information. Probe also wraps the VideoSource with its
 * own ProbeAdapterVideoSource (using the adapter pattern) in order to add
 * special information not known to the VideoSource, such as pixel spacing. The
 * recording functionality is not shown. VideoSourceRecorder handles this
 * on the basic level.
 *
 * VideoConnectionManager manages the connection to the streaming source. The
 * implementation class OpenIGTLinkVideoSource uses the OpenIGTLink protocol
 * to receive data over TCP/IP. The actual data retrieval is done in the
 * internal thread class IGTLinkClient.
 *
 * The OpenIGTLinkRTSource is dependent on an external IGTLink server.
 * Optionally, the VideoConnectionManager can create this server on the local machine.
 *
 *  \ingroup cx_resource_video
 *  \date 2011-05-31
 *  \date 2014-09-19
 *  \author Christian Askeland, SINTEF
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT VideoService : public QObject
{
	Q_OBJECT
public:
	virtual void setActiveVideoSource(QString uid) = 0;
	virtual VideoSourcePtr getActiveVideoSource() = 0;
	virtual std::vector<VideoSourcePtr> getVideoSources() = 0;

	virtual void setConnectionMethod(QString connectionMethod) = 0;
	virtual QString getConnectionMethod() = 0;
	virtual void openConnection() = 0;
	virtual void closeConnection() = 0;
	virtual bool isConnected() const = 0;

	virtual void setPlaybackMode(PlaybackTimePtr controller) = 0;
	virtual std::vector<TimelineEvent> getPlaybackEvents() = 0;

    virtual StreamerServicePtr getStreamerService(QString uid) = 0;
    virtual QList<StreamerServicePtr> getStreamerServices() = 0;

	virtual bool isNull() = 0;
	static VideoServicePtr getNullObject();

signals:
	void connected(bool on);
	void connectionMethodChanged();
	/** Emitted when a video source is set to active,
	  * OR when the available set of sources are changed.
	  */
	void activeVideoSourceChanged();
	void fps(int);

	void StreamerServiceAdded(StreamerService* service);
	void StreamerServiceRemoved(StreamerService* service);
};

} //cx
Q_DECLARE_INTERFACE(cx::VideoService, VideoService_iid)

#endif // CXVIDEOSERVICE_H
