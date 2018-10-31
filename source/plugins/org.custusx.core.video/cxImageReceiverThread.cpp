/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

ImageReceiverThread::ImageReceiverThread(StreamerServicePtr streamerInterface, QObject* parent) :
		QObject(parent),
		mStreamerInterface(streamerInterface)
{
	this->setObjectName("imagereceiver worker");
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

	if(!mImageStreamer)
	{
		return false;
	}

	report(QString("Starting streamer: [%1]").arg(this->hostDescription()));

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

//	bool needToCalibrateMsgTimeStamp = this->imageComesFromSonix(imgMsg);

// moved to IGTLinkClientStreamer
//	//Should only be needed if time stamp is set on another computer that is
//	//not synched with the one running this code: e.g. The Ultrasonix scanner
//	if (needToCalibrateMsgTimeStamp)
//        mStreamSynchronizer.syncToCurrentTime(imgMsg);

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

//bool ImageReceiverThread::imageComesFromSonix(ImagePtr imgMsg)
//{
//	return imgMsg->getUid().contains("Sonix", Qt::CaseInsensitive);
//}

QString ImageReceiverThread::hostDescription() const
{
	if (!mStreamerInterface)
		return "none";
	return mStreamerInterface->getName();
}


} /* namespace cx */
