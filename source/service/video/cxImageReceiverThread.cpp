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

#include "cxImageReceiverThread.h"
#include "sscMessageManager.h"
#include "sscVector3D.h"
#include "cxRenderTimer.h"
#include "cxVideoService.h"

namespace cx
{

ImageReceiverThread::ImageReceiverThread(QObject* parent) :
		QThread(parent)
{
	mFPSTimer.reset(new CyclicActionTimer());
	mGeneratingTimeCalibration = false;
	mLastReferenceTimestampDiff = 0.0;
	mLastTimeStamps.reserve(20);
}

void ImageReceiverThread::addImageToQueue(ssc::ImagePtr imgMsg)
{
	if(this->imageComesFromActiveVideoSource(imgMsg))
		this->reportFPS();

	bool needToCalibrateMsgTimeStamp = this->imageComesFromSonix(imgMsg);

	//Should only be needed if time stamp is set on another computer that is
	//not synched with the one running this code: e.g. The Ultrasonix scanner
	if (needToCalibrateMsgTimeStamp)
		this->calibrateTimeStamp(imgMsg);

	QMutexLocker sentry(&mImageMutex);
	mMutexedImageMessageQueue.push_back(imgMsg);
	sentry.unlock();

	emit imageReceived(); // emit signal outside lock, catch possibly in another thread
}

void ImageReceiverThread::addSonixStatusToQueue(ssc::ProbeDataPtr msg)
{
	QMutexLocker sentry(&mSonixStatusMutex);
	mMutexedSonixStatusMessageQueue.push_back(msg);
	sentry.unlock();
	emit sonixStatusReceived(); // emit signal outside lock, catch possibly in another thread
}

ssc::ImagePtr ImageReceiverThread::getLastImageMessage()
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
	mQueueSizeOnLastGet = queueSize;
	return retval;
}

ssc::ProbeDataPtr ImageReceiverThread::getLastSonixStatusMessage()
{
	QMutexLocker sentry(&mSonixStatusMutex);
	if (mMutexedSonixStatusMessageQueue.empty())
		return ssc::ProbeDataPtr();
	ssc::ProbeDataPtr retval = mMutexedSonixStatusMessageQueue.front();
	mMutexedSonixStatusMessageQueue.pop_front();
	return retval;
}

void ImageReceiverThread::calibrateTimeStamp(ssc::ImagePtr imgMsg)
{
	QDateTime timestamp_dt = imgMsg->getAcquisitionTime();
	double timestamp_ms = timestamp_dt.toMSecsSinceEpoch();

	if (ssc::similar(mLastReferenceTimestampDiff, 0.0, 0.000001))
		mLastReferenceTimestampDiff = timestamp_dt.msecsTo(QDateTime::currentDateTime());

	// Start collecting time stamps if 20 sec since last calibration time
	if(mLastSyncTime.isNull() || ( mLastSyncTime.msecsTo(QDateTime::currentDateTime()) > 2000) )
		mGeneratingTimeCalibration = true;

	if(mGeneratingTimeCalibration)
		mLastTimeStamps.push_back(timestamp_dt.msecsTo(QDateTime::currentDateTime()));

	// Perform time calibration if enough time stamps have been collected
	if(mLastTimeStamps.size() >= 20)
	{
		std::sort(mLastTimeStamps.begin(), mLastTimeStamps.end(), AbsDoubleLess(mLastReferenceTimestampDiff));

		mLastTimeStamps.resize(15);

		double sumTimes = 0;
	  for (std::vector<double>::const_iterator citer = mLastTimeStamps.begin(); citer != mLastTimeStamps.end(); ++citer)
	  {
	  	sumTimes += *citer;
	  }
	  mLastReferenceTimestampDiff = sumTimes / 15.0;

		//Reset
		mLastTimeStamps.clear();
		mLastSyncTime = QDateTime::currentDateTime();
		mGeneratingTimeCalibration = false;
	}
	imgMsg->setAcquisitionTime(QDateTime::fromMSecsSinceEpoch(timestamp_ms + mLastReferenceTimestampDiff));

}

void ImageReceiverThread::reportFPS()
{
	mFPSTimer->beginRender();
	mFPSTimer->endRender();
	if (mFPSTimer->intervalPassed())
	{
		emit fps(mFPSTimer->getFPS());
		mFPSTimer->reset(2000);
	}
}

bool ImageReceiverThread::imageComesFromActiveVideoSource(ssc::ImagePtr imgMsg)
{
	return imgMsg->getUid().compare(videoService()->getActiveVideoSource()->getUid()) == 0;
}

bool ImageReceiverThread::imageComesFromSonix(ssc::ImagePtr imgMsg)
{
	return imgMsg->getUid().contains("Sonix", Qt::CaseInsensitive);
}


} /* namespace cx */
