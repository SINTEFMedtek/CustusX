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

#include "cxGrabberReceiveThread.h"
#include "sscMessageManager.h"
#include "sscVector3D.h"
#include "cxRenderTimer.h"

namespace cx
{


GrabberReceiveThread::GrabberReceiveThread(QObject* parent) :
		QThread(parent)
{
	mFPSTimer.reset(new CyclicActionTimer());
	mGeneratingTimeCalibration = false;
	mLastReferenceTimestampDiff = 0.0;
	mLastTimeStamps.reserve(20);
}

// This was a problem encountered on MacOSX+OpenCV - this does not work well anyway - disable.
//
///**Give subclasses a change to override quit() by using this method instead.
// *
// * This is required for OpenCV using Direct Link: release() of the camera must (for
// * some unknown reason) be called inside exec().
// *
// */
//void IGTLinkClientBase::stop()
//{
//	//OpenCL hack removed
//	//This hack for OpenCV creates problems for GEStreamer
////	connect(this, SIGNAL(stopInternal()), this, SLOT(stopSlot()));
////	emit stopInternal();
//	this->quit();
//}

void GrabberReceiveThread::addImageToQueue(ssc::ImagePtr imgMsg)
{
	mFPSTimer->beginRender();
	mFPSTimer->endRender();
	if (mFPSTimer->intervalPassed())
	{
		emit fps(mFPSTimer->getFPS());
		mFPSTimer->reset(2000);
	}

	//Test if Sonix. Then calibrate time stamps
	//May need to test for other sources in the future.
	//E.g. if we want to use timestamps from other scanners, like GE
	bool calibrateMsgTimeStamp = false;
	if(imgMsg->getUid().contains("Sonix", Qt::CaseInsensitive))
	{
		calibrateMsgTimeStamp = true;
//		ssc::messageManager()->sendInfo("Calibrate time stamps");
	}
	else
	{
//		ssc::messageManager()->sendInfo("No time stamp calibration performed");
	}

	//Calibrate time stamp if needed.
	//Should only be needed if time stamp is set on another computer that is
	//not synched with the one running this code: e.g. The Ultrasonix scanner
	if (calibrateMsgTimeStamp)
		this->calibrateTimeStamp(imgMsg);

//	std::cout << "Queue size: " << mMutexedImageMessageQueue.size()  ;//<< std::endl;
//	std::cout << "\t diff: " << timestamp_dt.msecsTo(QDateTime::currentDateTime()) << std::endl;
//	std::cout << "\t image time: " << timestamp_dt.toString("ss.zzz").toStdString() << std::endl;

	QMutexLocker sentry(&mImageMutex);
	mMutexedImageMessageQueue.push_back(imgMsg);
//	int queueSize = mMutexedImageMessageQueue.size();
	sentry.unlock();

//	if (queueSize.size() > 5) // should not happen. Symptom of congestion.
//		ssc::messageManager()->sendInfo(QString("Added video frame to queue, size = %1.").arg(queueSize.size()));

	emit imageReceived(); // emit signal outside lock, catch possibly in another thread
}

/** add the message to a thread-safe queue
 */
void GrabberReceiveThread::addSonixStatusToQueue(ssc::ProbeData msg)
{
	QMutexLocker sentry(&mSonixStatusMutex);
	mMutexedSonixStatusMessageQueue.push_back(msg);
	sentry.unlock();
	emit sonixStatusReceived(); // emit signal outside lock, catch possibly in another thread
}

/** Threadsafe retrieval of last image message.
 *
 */
ssc::ImagePtr GrabberReceiveThread::getLastImageMessage()
{
	QMutexLocker sentry(&mImageMutex);
	if (mMutexedImageMessageQueue.empty())
		return ssc::ImagePtr();
	ssc::ImagePtr retval = mMutexedImageMessageQueue.front();
	mMutexedImageMessageQueue.pop_front();

	// this happens when the main thread is busy. This is bad, but happens a lot during operation.
	// Removed this in order to remove spam from console
	static int mQueueSizeOnLastGet = 0;
	int queueSize = mMutexedImageMessageQueue.size();
//	if (( queueSize > mQueueSizeOnLastGet )&&( queueSize > 9 )) // should not happen. Symptom of congestion.
//		ssc::messageManager()->sendInfo(QString("%1 remaining video frames in queue.").arg(queueSize));
	mQueueSizeOnLastGet = queueSize;
	return retval;
}

/** Threadsafe retrieval of last image message.
 *
 */
ssc::ProbeData GrabberReceiveThread::getLastSonixStatusMessage()
{
	QMutexLocker sentry(&mSonixStatusMutex);
	if (mMutexedSonixStatusMessageQueue.empty())
		return ssc::ProbeData();
	ssc::ProbeData retval = mMutexedSonixStatusMessageQueue.front();
	mMutexedSonixStatusMessageQueue.pop_front();
	return retval;
}

/**
 * Calibrate the time stamps of the incoming message based on the computer clock.
 * Calibration is based on an average of several of the last messages.
 * The calibration is updated every 20-30 sec.
 */
void GrabberReceiveThread::calibrateTimeStamp(ssc::ImagePtr imgMsg)
{
//	igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
//	imgMsg->GetTimeStamp(timestamp);
//	double timestamp_ms = timestamp->GetTimeStamp() * 1000;
//	QDateTime timestamp_dt = QDateTime::fromMSecsSinceEpoch(timestamp_ms);
	QDateTime timestamp_dt = imgMsg->getAcquisitionTime();
	double timestamp_ms = timestamp_dt.toMSecsSinceEpoch();

	if (ssc::similar(mLastReferenceTimestampDiff, 0.0, 0.000001))
	{
		mLastReferenceTimestampDiff = timestamp_dt.msecsTo(QDateTime::currentDateTime());
//		std::cout << "First timestamp calib: " << mLastReferenceTimestampDiff << " ms" << std::endl;
	}

	// Start collecting time stamps if 20 sec since last calibration time
	if(mLastSyncTime.isNull() || ( mLastSyncTime.msecsTo(QDateTime::currentDateTime()) > 2000) )
		mGeneratingTimeCalibration = true;

	if(mGeneratingTimeCalibration)
		mLastTimeStamps.push_back(timestamp_dt.msecsTo(QDateTime::currentDateTime()));

	// Perform time calibration if enough time stamps have been collected
	if(mLastTimeStamps.size() >= 20)
	{
		std::sort(mLastTimeStamps.begin(), mLastTimeStamps.end(), AbsDoubleLess(mLastReferenceTimestampDiff));

		//debug print
	  /*for (std::vector<double>::const_iterator citer = mLastTimeStamps.begin();
	      citer != mLastTimeStamps.end(); ++citer)
	  {
			std::cout << *citer - mLastReferenceTimestampDiff << " ";
	  }
	  std::cout << endl;*/

		mLastTimeStamps.resize(15);

		//debug print
	  /*for (std::vector<double>::const_iterator citer = mLastTimeStamps.begin();
	      citer != mLastTimeStamps.end(); ++citer)
	  {
			std::cout << *citer - mLastReferenceTimestampDiff << " ";
	  }
	  std::cout << endl;*/

		double sumTimes = 0;
	  for (std::vector<double>::const_iterator citer = mLastTimeStamps.begin();
	      citer != mLastTimeStamps.end(); ++citer)
	  {
	  	sumTimes += *citer;
	  }
	  mLastReferenceTimestampDiff = sumTimes / 15.0;

//		std::cout << "Timestamp calib: " << mLastReferenceTimestampDiff << " ms" << std::endl;

		//Reset
		mLastTimeStamps.clear();
		mLastSyncTime = QDateTime::currentDateTime();
		mGeneratingTimeCalibration = false;
	}

	//test code for testing how much time is added by the time calibration,
	//and if it is constant for local time stamps
	//igtl::TimeStamp::Pointer oldtimestamp = igtl::TimeStamp::New();
	//imgMsg->GetTimeStamp(oldtimestamp);

	// Update imgMsg timestamp
//	timestamp->SetTime((timestamp_ms + mLastReferenceTimestampDiff) / 1000.0); // in sec
//	imgMsg->SetTimeStamp(timestamp);
	imgMsg->setAcquisitionTime(QDateTime::fromMSecsSinceEpoch(timestamp_ms + mLastReferenceTimestampDiff));

	//test code
	//double timestampDiff = (timestamp->GetTimeStamp() - oldtimestamp->GetTimeStamp()) * 1000; //ms
	//std::cout << "timestampDiff: " << timestampDiff << " ms" << std::endl;
}

} /* namespace cx */
