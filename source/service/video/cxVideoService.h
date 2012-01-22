/*
 * cxVideoService.h
 *
 *  Created on: May 31, 2011
 *      Author: christiana
 */

#ifndef CXVIDEOSERVICE_H_
#define CXVIDEOSERVICE_H_

#include <QObject>

#include "cxVideoConnection.h"

namespace cx
{
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
 * VideoConnection manages the connection to the streaming source. The
 * implementation class OpenIGTLinkVideoSource uses the OpenIGTLink protocol
 * to receive data over TCP/IP. The actual data retrieval is done in the
 * internal thread class IGTLinkClient.
 *
 * The OpenIGTLinkRTSource is dependent on an external IGTLink server.
 * Optionally, the VideoConnection can create this server on the local machine.
 * This is typically the GrabberServer, chapter 5.7.2.
 *
 *
 */
class VideoService : public QObject
{
	Q_OBJECT
public:
  static VideoService* getInstance();

  static void initialize();
  static void shutdown();

  VideoConnectionPtr getVideoConnection();

private:
  static VideoService* mInstance;
  static void setInstance(VideoService* instance);

  VideoService();
	virtual ~VideoService();

	VideoService(VideoService const&); // not implemented
	VideoService& operator=(VideoService const&); // not implemented

  VideoConnectionPtr mIGTLinkConnection;
};

VideoService* videoService();

/**
* @}
*/
}

#endif /* CXVIDEOSERVICE_H_ */
