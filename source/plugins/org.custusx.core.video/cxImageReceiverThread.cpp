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

#include "cxImageReceiverThread.h"

#include "cxCyclicActionLogger.h"
#include "cxXmlOptionItem.h"
#include "cxStreamer.h"
#include "cxStreamerService.h"
#include "cxDirectlyLinkedSender.h"
#include "cxLogger.h"
#include "cxProfile.h"

namespace cx
{

class AbsDoubleLess
{
public:
	AbsDoubleLess(double center) : mCenter(center) { };

  bool operator()(const double& d1, const double& d2)
  {
	return fabs(d1 - mCenter) < fabs(d2 - mCenter);
  }

  double mCenter;
};

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ImageReceiverThread::ImageReceiverThread(StreamerServicePtr streamerInterface, QObject* parent) :
		QObject(parent),
		mStreamerInterface(streamerInterface)
{
	this->setObjectName("imagereceiver worker");
	mGeneratingTimeCalibration = false;
	mLastReferenceTimestampDiff = 0.0;
	mLastTimeStamps.reserve(20);
}

void ImageReceiverThread::initialize()
{
	if (!this->attemptInitialize())
	{
		// cleanup here in order to do less in the destructor... ??
		mImageStreamer.reset();
		mSender.reset();

		emit finished();
	}
}

bool ImageReceiverThread::attemptInitialize()
{
	XmlOptionFile xmlFile = profile()->getXmlSettings().descend("video");
	QDomElement element = xmlFile.getElement("video");
	mImageStreamer = mStreamerInterface->createStreamer(element);
	report(QString("Starting streamer: [%1]").arg(this->hostDescription()));

	if(!mImageStreamer)
	{
		return false;
	}
	mSender.reset(new DirectlyLinkedSender());

	connect(mSender.get(), &DirectlyLinkedSender::newImage, this, &ImageReceiverThread::addImageToQueueSlot, Qt::DirectConnection);
	connect(mSender.get(), &DirectlyLinkedSender::newUSStatus, this, &ImageReceiverThread::addSonixStatusToQueueSlot, Qt::DirectConnection);

	mImageStreamer->startStreaming(mSender);

	return true;
}

void ImageReceiverThread::shutdown()
{
	if (mImageStreamer)
	{
		report(QString("Stopping streamer: [%1]...").arg(this->hostDescription()));
		mImageStreamer->stopStreaming();
		report(QString("Stopped streamer: [%1]").arg(this->hostDescription()));
		mImageStreamer.reset();
		mSender.reset();
	}

	emit finished();
}

void ImageReceiverThread::addImageToQueueSlot()
{
	this->addImageToQueue(mSender->popImage());
}

void ImageReceiverThread::addSonixStatusToQueueSlot()
{
	this->addSonixStatusToQueue(mSender->popUSStatus());
}


void ImageReceiverThread::addImageToQueue(ImagePtr imgMsg)
{
	this->reportFPS(imgMsg->getUid());
//	if(this->imageComesFromActiveVideoSource(imgMsg))
//		this->reportFPS();

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

void ImageReceiverThread::addSonixStatusToQueue(ProbeDefinitionPtr msg)
{
	QMutexLocker sentry(&mSonixStatusMutex);
	mMutexedSonixStatusMessageQueue.push_back(msg);
	sentry.unlock();
	emit sonixStatusReceived(); // emit signal outside lock, catch possibly in another thread
}

ImagePtr ImageReceiverThread::getLastImageMessage()
{
	QMutexLocker sentry(&mImageMutex);
	if (mMutexedImageMessageQueue.empty())
		return ImagePtr();
	ImagePtr retval = mMutexedImageMessageQueue.front();
	mMutexedImageMessageQueue.pop_front();

	// this happens when the main thread is busy. This is bad, but happens a lot during operation.
	// Removed this in order to remove spam from console
//	static int mQueueSizeOnLastGet = 0;
//	int queueSize = mMutexedImageMessageQueue.size();
//	mQueueSizeOnLastGet = queueSize;
	return retval;
}

ProbeDefinitionPtr ImageReceiverThread::getLastSonixStatusMessage()
{
	QMutexLocker sentry(&mSonixStatusMutex);
	if (mMutexedSonixStatusMessageQueue.empty())
		return ProbeDefinitionPtr();
	ProbeDefinitionPtr retval = mMutexedSonixStatusMessageQueue.front();
	mMutexedSonixStatusMessageQueue.pop_front();
	return retval;
}

void ImageReceiverThread::calibrateTimeStamp(ImagePtr imgMsg)
{
	QDateTime timestamp_dt = imgMsg->getAcquisitionTime();
	double timestamp_ms = timestamp_dt.toMSecsSinceEpoch();

	if (similar(mLastReferenceTimestampDiff, 0.0, 0.000001))
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

void ImageReceiverThread::reportFPS(QString streamUid)
{
	int timeout = 2000;
	if (!mFPSTimer.count(streamUid))
	{
		mFPSTimer[streamUid].reset(new CyclicActionLogger());
		mFPSTimer[streamUid]->reset(timeout);
	}

	CyclicActionLoggerPtr logger = mFPSTimer[streamUid];

	logger->begin();
	if (logger->intervalPassed())
	{
		emit fps(streamUid, logger->getFPS());
		logger->reset(timeout);
	}
}

bool ImageReceiverThread::imageComesFromSonix(ImagePtr imgMsg)
{
	return imgMsg->getUid().contains("Sonix", Qt::CaseInsensitive);
}

QString ImageReceiverThread::hostDescription() const
{
	if (!mStreamerInterface)
		return "none";
	return mStreamerInterface->getName();
}


} /* namespace cx */
