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

/*
 * cxVideoService.h
 *
 *  \date May 31, 2011
 *      \author christiana
 */

#ifndef CXVIDEOSERVICEOLD_H_
#define CXVIDEOSERVICEOLD_H_

#include "cxVideoServiceExport.h"

#include <QObject>

#include <vector>
#include "cxVideoSource.h"
#include "cxForwardDeclarations.h"
class ctkPluginContext;

namespace cx
{
typedef boost::shared_ptr<class USAcquisitionVideoPlayback> USAcquisitionVideoPlaybackPtr;
typedef boost::shared_ptr<class PlaybackTime> PlaybackTimePtr;
typedef boost::shared_ptr<class VideoConnectionManager> VideoConnectionManagerPtr;
typedef boost::shared_ptr<class VideoServiceBackend> VideoServiceBackendPtr;

/**
 * \file
 * \addtogroup cx_service_video
 * @{
 */

/**
 * \brief Provides access to all video sources in the
 * system, and connection stuff for the sources.
 * \ingroup cx_service_video
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
 *
 */
class cxVideoService_EXPORT VideoServiceOld: public QObject
{
Q_OBJECT
public:
	static VideoServiceOldPtr create(VideoServiceBackendPtr backend);
	virtual ~VideoServiceOld();

	VideoConnectionManagerPtr getVideoConnection();
	USAcquisitionVideoPlaybackPtr getUSAcquisitionVideoPlayback();
	VideoSourcePtr getActiveVideoSource();
	void setActiveVideoSource(QString uid);
	void setPlaybackMode(PlaybackTimePtr controller);
	/** Get all existing video sources.
	  */
	std::vector<VideoSourcePtr> getVideoSources();
	VideoServiceBackendPtr getBackend();

signals:
	/** Emitted when a video source is set to active,
	  * OR when the available set of sources are changed.
	  */
	void activeVideoSourceChanged();
	void fps(int);

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

	VideoServiceOld(VideoServiceBackendPtr videoBackend);

	VideoServiceOld(VideoServiceOld const&); // not implemented
	VideoServiceOld& operator=(VideoServiceOld const&); // not implemented

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

	VideoConnectionManagerPtr mVideoConnection;
	VideoSourcePtr mActiveVideoSource;
	VideoSourcePtr mEmptyVideoSource;
	USAcquisitionVideoPlaybackPtr mUSAcquisitionVideoPlayback;
	VideoServiceBackendPtr mBackend;
};

//VideoServicePtr videoService();

/**
 * @}
 */
}

#endif /* CXVIDEOSERVICEOLD_H_ */
