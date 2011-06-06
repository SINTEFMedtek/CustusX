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
 * VideoService provides access to all video sources in the
 * system, and connection stuff for the sources.
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

}

#endif /* CXVIDEOSERVICE_H_ */
