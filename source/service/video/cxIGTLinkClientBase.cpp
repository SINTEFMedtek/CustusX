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

#include "cxIGTLinkClientBase.h"

namespace cx
{

/**Give subclasses a change to override quit() by using this method instead.
 *
 * This is required for OpenCV using Direct Link: release() of the camera must (for
 * some unknown reason) be called inside exec().
 *
 */
void IGTLinkClientBase::stop()
{
	connect(this, SIGNAL(stopInternal()), this, SLOT(stopSlot()));
	emit stopInternal();
	this->quit();
}


/** add the message to a thread-safe queue
 */
void IGTLinkClientBase::addImageToQueue(IGTLinkImageMessage::Pointer imgMsg)
{

	mFPSTimer.beginRender();
	mFPSTimer.endRender();
	if (mFPSTimer.intervalPassed())
	{
		emit fps(mFPSTimer.getFPS());
		mFPSTimer.reset(2000);
	}

	// testcode?
	//Get modified timestamp
	igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
	imgMsg->GetTimeStamp(timestamp);
	double timestamp_ms = timestamp->GetTimeStamp() * 1000;
	QDateTime timestamp_dt = QDateTime::fromMSecsSinceEpoch(timestamp_ms);
	// end testcode

//	std::cout << "Queue size: " << mMutexedImageMessageQueue.size()  ;//<< std::endl;
//	std::cout << "\t diff: " << timestamp_dt.msecsTo(QDateTime::currentDateTime()) << std::endl;


	QMutexLocker sentry(&mImageMutex);
	mMutexedImageMessageQueue.push_back(imgMsg);
	sentry.unlock();
	emit imageReceived(); // emit signal outside lock, catch possibly in another thread
}

/** add the message to a thread-safe queue
 */
void IGTLinkClientBase::addSonixStatusToQueue(IGTLinkUSStatusMessage::Pointer msg)
{
	QMutexLocker sentry(&mSonixStatusMutex);
	mMutexedSonixStatusMessageQueue.push_back(msg);
	sentry.unlock();
	emit sonixStatusReceived(); // emit signal outside lock, catch possibly in another thread
}

/** Threadsafe retrieval of last image message.
 *
 */
IGTLinkImageMessage::Pointer IGTLinkClientBase::getLastImageMessage()
{
	QMutexLocker sentry(&mImageMutex);
	if (mMutexedImageMessageQueue.empty())
		return IGTLinkImageMessage::Pointer();
	IGTLinkImageMessage::Pointer retval = mMutexedImageMessageQueue.front();
	mMutexedImageMessageQueue.pop_front();
	return retval;
}

/** Threadsafe retrieval of last image message.
 *
 */
IGTLinkUSStatusMessage::Pointer IGTLinkClientBase::getLastSonixStatusMessage()
{
	QMutexLocker sentry(&mSonixStatusMutex);
	if (mMutexedSonixStatusMessageQueue.empty())
		return IGTLinkUSStatusMessage::Pointer();
	IGTLinkUSStatusMessage::Pointer retval = mMutexedSonixStatusMessageQueue.front();
	mMutexedSonixStatusMessageQueue.pop_front();
	return retval;
}

} /* namespace cx */
