/*
 * cxIGTLinkClientBase.cpp
 *
 *  Created on: Oct 11, 2012
 *      Author: christiana
 */

#include "cxIGTLinkClientBase.h"

namespace cx
{


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
