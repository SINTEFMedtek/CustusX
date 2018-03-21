
/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
