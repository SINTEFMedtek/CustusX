
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


#ifndef CXImageReceiverThread_H_
#define CXImageReceiverThread_H_

#include "org_custusx_core_video_Export.h"


#include <vector>
#include "boost/shared_ptr.hpp"
#include <QThread>
#include <QMutex>
#include <QDateTime>
#include "cxForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class Streamer> StreamerPtr;
typedef boost::shared_ptr<class StreamerService> StreamerServicePtr;
typedef boost::shared_ptr<class DirectlyLinkedSender> DirectlyLinkedSenderPtr;
typedef boost::shared_ptr<class ProbeDefinition> ProbeDefinitionPtr;

/**
 * \file
 * \addtogroup org_custusx_core_video
 * @{
 */


typedef boost::shared_ptr<class ImageReceiverThread> ImageReceiverThreadPtr;

/** \brief Base class for receiving images from a video stream.
 *
 * Subclass to implement for a specific protocol.
 * Supported messages:
 *  - Image : contains vtkImageData, timestamp, uid, all else is discarded.
 *  - ProbeDefinition : contains sector and image definition, temporal cal is discarded.
 *
 * \ingroup org_custusx_core_video
 * \date Oct 11, 2012
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_video_EXPORT ImageReceiverThread: public QObject
{
Q_OBJECT
public:
	ImageReceiverThread(StreamerServicePtr streamerInterface, QObject* parent = NULL);
	virtual ~ImageReceiverThread() {}
	virtual ImagePtr getLastImageMessage(); // threadsafe, Threadsafe retrieval of last image message.
	virtual ProbeDefinitionPtr getLastSonixStatusMessage(); // threadsafe,Threadsafe retrieval of last status message.
	virtual QString hostDescription() const; // threadsafe

public slots:
	void initialize(); // not threadsafe, call via postevent
	void shutdown(); // not threadsafe, call via postevent

signals:
	void imageReceived();
	void sonixStatusReceived();
	void fps(QString, double);
	void finished(); // emitted when object has completed shutdown

protected:
	/** Add the message to a thread-safe queue.
	 * Tests if the time stamps of image messages should be calibrated based on the computer clock.
	 * Time stamps only need to be synched if set on another computer that is
	 * not synched, e.g. the Ultrasonix scanner
	 * \param[in] imgMsg Incoming image message
	 */
	void addImageToQueue(ImagePtr imgMsg);
	void addSonixStatusToQueue(ProbeDefinitionPtr msg); ///< add the message to a thread-safe queue

private slots:

	void addImageToQueueSlot();
	void addSonixStatusToQueueSlot();

private:
	void reportFPS(QString streamUid);
//	bool imageComesFromSonix(ImagePtr imgMsg);
	bool attemptInitialize();

	std::map<QString, cx::CyclicActionLoggerPtr> mFPSTimer;
	QMutex mImageMutex;
	QMutex mSonixStatusMutex;
	std::list<ImagePtr> mMutexedImageMessageQueue;
	std::list<ProbeDefinitionPtr> mMutexedSonixStatusMessageQueue;

//    StreamedTimestampSynchronizer mStreamSynchronizer;

	StreamerServicePtr mStreamerInterface;
	StreamerPtr mImageStreamer;
	DirectlyLinkedSenderPtr mSender;

};

/**
 * @}
 */
} //end namespace cx

#endif /* CXImageReceiverThread_H_ */
