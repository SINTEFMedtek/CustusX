/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxUSSavingRecorder.h"

#include <QtConcurrentRun>
#include "boost/bind.hpp"

#include "cxTypeConversions.h"

#include "cxTime.h"
#include "cxTool.h"
#include "cxVideoSource.h"
#include "cxLogger.h"
#include "cxDataLocations.h"
#include "cxSavingVideoRecorder.h"
#include "cxImageDataContainer.h"
#include "cxRecordSession.h"
#include "cxUsReconstructionFileMaker.h"


namespace cx
{


USSavingRecorder::USSavingRecorder() : mDoWriteColor(true), m_rMpr(Transform3D::Identity())
{

}

USSavingRecorder::~USSavingRecorder()
{
	std::list<QFutureWatcher<QString>*>::iterator iter;
	for (iter=mSaveThreads.begin(); iter!=mSaveThreads.end(); ++iter)
	{
		(*iter)->waitForFinished();
	}
}

void USSavingRecorder::setWriteColor(bool on)
{
	mDoWriteColor = on;
}

void USSavingRecorder::set_rMpr(Transform3D rMpr)
{
	m_rMpr = rMpr;
}

void USSavingRecorder::startRecord(RecordSessionPtr session, ToolPtr tool, ToolPtr reference, std::vector<VideoSourcePtr> video)
{
	this->clearRecording(); // clear previous data if any

	mRecordingTool = tool;
	mReference = reference;
	mSession = session;

	QString tempBaseFolder = DataLocations::getCachePath()+"/usacq/"+QDateTime::currentDateTime().toString(timestampSecondsFormat());
	QString cacheFolder = UsReconstructionFileMaker::createUniqueFolder(tempBaseFolder, session->getDescription());

	for (unsigned i=0; i<video.size(); ++i)
	{
		SavingVideoRecorderPtr videoRecorder;
		videoRecorder.reset(new SavingVideoRecorder(
								 video[i],
								 cacheFolder,
								 QString("%1_%2").arg(session->getDescription()).arg(video[i]->getUid()),
								 false, // no compression when saving to cache
								 mDoWriteColor));
		videoRecorder->startRecord();
		mVideoRecorder.push_back(videoRecorder);
	}

	reportSuccess("Ultrasound acquisition started.");
}

void USSavingRecorder::stopRecord()
{
	for (unsigned i=0; i<mVideoRecorder.size(); ++i)
		mVideoRecorder[i]->stopRecord();
	reportSuccess("Ultrasound acquisition stopped.");

	for (unsigned i=0; i<mVideoRecorder.size(); ++i)
	{
		// complete writing of images to temporary storage. Do this before using the image data.
		mVideoRecorder[i]->completeSave();
	}
}

void USSavingRecorder::cancelRecord()
{
	this->clearRecording();
	report("Ultrasound acquisition cancelled.");
}

USReconstructInputData USSavingRecorder::getDataForStream(QString streamUid)
{
	for (unsigned i=0; i<mVideoRecorder.size(); ++i)
	{
		if (mVideoRecorder[i]->getSource()->getUid() == streamUid)
			return this->getDataForStream(i);
	}
	return USReconstructInputData();
}

USReconstructInputData USSavingRecorder::getDataForStream(unsigned videoRecorderIndex)
{
	if (!mSession)
		return USReconstructInputData();
	if (videoRecorderIndex>=mVideoRecorder.size())
		return USReconstructInputData();

	SavingVideoRecorderPtr videoRecorder = mVideoRecorder[videoRecorderIndex];
	videoRecorder->completeSave(); // just in case - should have been done earlier.
	TimedTransformMap trackerRecordedData = RecordSession::getToolHistory_prMt(mRecordingTool, mSession, true);
	std::map<double, cx::ToolPositionMetadata> trackerMetadata = RecordSession::getToolHistory_metadata(mRecordingTool, mSession, true);
	std::map<double, cx::ToolPositionMetadata> referenceTrackerMetadata = RecordSession::getToolHistory_metadata(mReference, mSession, true);
	std::cout << "----------- "
				 "trackerMetadata : " << trackerMetadata.size() << std::endl;

	CachedImageDataContainerPtr imageData = videoRecorder->getImageData();
	std::vector<TimeInfo> imageTimestamps = videoRecorder->getTimestamps();
	QString streamSessionName = mSession->getDescription()+"_"+videoRecorder->getSource()->getUid();

	UsReconstructionFileMakerPtr fileMaker;
	fileMaker.reset(new UsReconstructionFileMaker(streamSessionName));
	USReconstructInputData reconstructData = fileMaker->getReconstructData(imageData,
																		   imageTimestamps,
																		   trackerRecordedData,
																		   trackerMetadata,
																		   referenceTrackerMetadata,
																		   mRecordingTool,
																		   videoRecorder->getSource()->getUid(),
																		   mDoWriteColor,
																		   m_rMpr);
	return reconstructData;
}

void USSavingRecorder::startSaveData(QString baseFolder, bool compressImages)
{
	if (!mSession)
		return;

	for (unsigned i=0; i<mVideoRecorder.size(); ++i)
	{
		USReconstructInputData data = this->getDataForStream(i);

		QString streamSessionName = mSession->getDescription()+"_"+mVideoRecorder[i]->getSource()->getUid();
		QString saveFolder = UsReconstructionFileMaker::createFolder(baseFolder, mSession->getDescription());

		this->saveStreamSession(data, saveFolder, streamSessionName, compressImages);
	}
}

void USSavingRecorder::clearRecording()
{
	mVideoRecorder.clear();
	mSession.reset();
	mRecordingTool.reset();
}


size_t USSavingRecorder::getNumberOfSavingThreads() const
{
	return mSaveThreads.size();
}

void USSavingRecorder::saveStreamSession(USReconstructInputData reconstructData, QString saveFolder, QString streamSessionName, bool compress)
{
	UsReconstructionFileMakerPtr fileMaker;
	fileMaker.reset(new UsReconstructionFileMaker(streamSessionName));
	fileMaker->setReconstructData(reconstructData);

	// now start saving of data to the patient folder, compressed version:
	QFuture<QString> fileMakerFuture =
			QtConcurrent::run(boost::bind(
								  &UsReconstructionFileMaker::writeToNewFolder,
								  fileMaker,
								  saveFolder,
								  compress
								  ));
	QFutureWatcher<QString>* fileMakerFutureWatcher = new QFutureWatcher<QString>();
    connect(fileMakerFutureWatcher, SIGNAL(finished()), this, SLOT(fileMakerWriteFinished()));
	fileMakerFutureWatcher->setFuture(fileMakerFuture);
	mSaveThreads.push_back(fileMakerFutureWatcher);
	fileMaker.reset(); // filemaker is now stored in the mSaveThreads queue, clear as current.
}

void USSavingRecorder::fileMakerWriteFinished()
{
	std::list<QFutureWatcher<QString>*>::iterator iter;
    for (iter=mSaveThreads.begin(); iter!=mSaveThreads.end();)
	{
		if (!(*iter)->isFinished())
        {
            ++iter;
			continue;
        }
		QString result = (*iter)->future().result();
		delete *iter;
        //this increments the iter, so no need to do it in the for statement
        iter = mSaveThreads.erase(iter);

		emit saveDataCompleted(result);
	}
}

}
