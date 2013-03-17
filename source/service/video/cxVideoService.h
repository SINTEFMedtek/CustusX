// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

/*
 * cxVideoService.h
 *
 *  \date May 31, 2011
 *      \author christiana
 */

#ifndef CXVIDEOSERVICE_H_
#define CXVIDEOSERVICE_H_

#include <QObject>

#include <vector>
#include "sscVideoSource.h"

namespace cx
{
typedef boost::shared_ptr<class USAcquisitionVideoPlayback> USAcquisitionVideoPlaybackPtr;
typedef boost::shared_ptr<class PlaybackTime> PlaybackTimePtr;
typedef boost::shared_ptr<class VideoConnectionManager> VideoConnectionManagerPtr;

//typedef boost::shared_ptr<class OpenIGTLinkDirectLinkRTSource> OpenIGTLinkDirectLinkRTSourcePtr;


/**
 * \file
 * \addtogroup cxServiceVideo
 * @{
 */

/**
 * \brief Provides access to all video sources in the
 * system, and connection stuff for the sources.
 * \ingroup cxServiceVideo
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
 * ssc::VideoSource is the core interface that provides connection to a realtime
 * streaming source.
 *
 * ssc::VideoSource has to main users: Rendering to screen and recording to disk.
 * VideoGraphics contains the visualization functionality that the Reps uses
 * when rendering in the Views.  VideoGraphics needs a ssc::Probe to provide
 * position information. ssc::Probe also wraps the ssc::VideoSource with its
 * own ssc::ProbeAdapterVideoSource (using the adapter pattern) in order to add
 * special information not known to the VideoSource, such as pixel spacing. The
 * recording functionality is not shown. ssc::VideoSourceRecorder handles this
 * on the basic level.
 *
 * VideoConnectionManager manages the connection to the streaming source. The
 * implementation class OpenIGTLinkVideoSource uses the OpenIGTLink protocol
 * to receive data over TCP/IP. The actual data retrieval is done in the
 * internal thread class IGTLinkClient.
 *
 * The OpenIGTLinkRTSource is dependent on an external IGTLink server.
 * Optionally, the VideoConnectionManager can create this server on the local machine.
 * This is typically the GrabberServer, chapter 5.7.2.
 *
 *
 */
class VideoService: public QObject
{
Q_OBJECT
public:
	static VideoService* getInstance();

	static void initialize();
	static void shutdown();

	VideoConnectionManagerPtr getVideoConnection();
	USAcquisitionVideoPlaybackPtr getUSAcquisitionVideoPlayback();
	ssc::VideoSourcePtr getActiveVideoSource();
	void setActiveVideoSource(QString uid);
	void setPlaybackMode(PlaybackTimePtr controller);
	/** Get all existing video sources.
	  */
	std::vector<ssc::VideoSourcePtr> getVideoSources();

signals:
	/** Emitted when a video source is set to active,
	  * OR when the available set of sources are changed.
	  */
	void activeVideoSourceChanged();

private slots:
	/** Autoselect the active ssc::VideoSource
	  *
	  * Call when video source configuration has changed. The active
	  * Video source will automatically be determined by calling
	  * autoGuessVideoSource().
	  */
	void autoSelectActiveVideoSource();
private:
	static VideoService* mInstance;
	static void setInstance(VideoService* instance);

	VideoService();
	virtual ~VideoService();

	VideoService(VideoService const&); // not implemented
	VideoService& operator=(VideoService const&); // not implemented

	/** Find the best guess for active ssc::VideoSource
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
	ssc::VideoSourcePtr getGuessForActiveVideoSource(ssc::VideoSourcePtr old);

	VideoConnectionManagerPtr mVideoConnection;
	ssc::VideoSourcePtr mActiveVideoSource;
	ssc::VideoSourcePtr mEmptyVideoSource;
	USAcquisitionVideoPlaybackPtr mUSAcquisitionVideoPlayback;
};

VideoService* videoService();

/**
 * @}
 */
}

#endif /* CXVIDEOSERVICE_H_ */