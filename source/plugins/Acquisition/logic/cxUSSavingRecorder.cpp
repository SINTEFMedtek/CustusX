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

#include "cxUSSavingRecorder.h"

#include <QtConcurrentRun>
#include "boost/bind.hpp"

#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscTime.h"
#include "sscTool.h"
#include "sscVideoSource.h"

#include "cxDataLocations.h"
#include "cxSavingVideoRecorder.h"
#include "cxImageDataContainer.h"
#include "cxRecordSession.h"
#include "cxUsReconstructionFileMaker.h"
#include "sscLogger.h"

namespace cx
{


USSavingRecorder::USSavingRecorder() : mDoWriteColor(true), m_rMpr(ssc::Transform3D::Identity())
{

}

void USSavingRecorder::setWriteColor(bool on)
{
	mDoWriteColor = on;
}

void USSavingRecorder::set_rMpr(ssc::Transform3D rMpr)
{
	m_rMpr = rMpr;
}

void USSavingRecorder::startRecord(RecordSessionPtr session, ssc::ToolPtr tool, std::vector<ssc::VideoSourcePtr> video)
{
	this->clearRecording(); // clear previous data if any

	mRecordingTool = tool;
	mSession = session;

	QString tempBaseFolder = DataLocations::getCachePath()+"/usacq/"+QDateTime::currentDateTime().toString(ssc::timestampSecondsFormat());
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

	ssc::messageManager()->sendSuccess("Ultrasound acquisition started.", true);
}

void USSavingRecorder::stopRecord()
{
	for (unsigned i=0; i<mVideoRecorder.size(); ++i)
		mVideoRecorder[i]->stopRecord();
	ssc::messageManager()->sendSuccess("Ultrasound acquisition stopped.", true);

	for (unsigned i=0; i<mVideoRecorder.size(); ++i)
	{
		// complete writing of images to temporary storage. Do this before using the image data.
		mVideoRecorder[i]->completeSave();
//		std::cout << QString("completed save of cached video stream %1").arg(i) << std::endl;
	}
}

void USSavingRecorder::cancelRecord()
{
	this->clearRecording();
	ssc::messageManager()->sendInfo("Ultrasound acquisition cancelled.");
}

ssc::USReconstructInputData USSavingRecorder::getDataForStream(QString streamUid)
{
	for (unsigned i=0; i<mVideoRecorder.size(); ++i)
	{
		if (mVideoRecorder[i]->getSource()->getUid() == streamUid)
			return this->getDataForStream(i);
	}
	return ssc::USReconstructInputData();
}

ssc::USReconstructInputData USSavingRecorder::getDataForStream(unsigned videoRecorderIndex)
{
	if (!mSession)
		return ssc::USReconstructInputData();
	if (videoRecorderIndex>=mVideoRecorder.size())
		return ssc::USReconstructInputData();

	SavingVideoRecorderPtr videoRecorder = mVideoRecorder[videoRecorderIndex];
	videoRecorder->completeSave(); // just in case - should have been done earlier.
	ssc::TimedTransformMap trackerRecordedData = this->getRecording();

	CachedImageDataContainerPtr imageData = videoRecorder->getImageData();
	std::vector<double> imageTimestamps = videoRecorder->getTimestamps();
	QString streamSessionName = mSession->getDescription()+"_"+videoRecorder->getSource()->getUid();

	UsReconstructionFileMakerPtr fileMaker;
	fileMaker.reset(new UsReconstructionFileMaker(streamSessionName));
	ssc::USReconstructInputData reconstructData = fileMaker->getReconstructData(imageData,
																				imageTimestamps,
																				trackerRecordedData,
																				mRecordingTool,
																				mDoWriteColor,
																				m_rMpr);
	return reconstructData;
}

void USSavingRecorder::startSaveData(QString baseFolder, bool compressImages)
{
	if (!mSession)
		return;

	ssc::TimedTransformMap trackerRecordedData = this->getRecording();

	for (unsigned i=0; i<mVideoRecorder.size(); ++i)
	{
		ssc::USReconstructInputData data = this->getDataForStream(i);

		QString streamSessionName = mSession->getDescription()+"_"+mVideoRecorder[i]->getSource()->getUid();
		QString saveFolder = UsReconstructionFileMaker::createFolder(baseFolder, mSession->getDescription());

		this->saveStreamSession(data, saveFolder, streamSessionName, compressImages);
	}

//	this->clearRecording();
}

void USSavingRecorder::clearRecording()
{
	mVideoRecorder.clear();
	mSession.reset();
	mRecordingTool.reset();
}


unsigned USSavingRecorder::getNumberOfSavingThreads() const
{
	return mSaveThreads.size();
}

void USSavingRecorder::saveStreamSession(ssc::USReconstructInputData reconstructData, QString saveFolder, QString streamSessionName, bool compress)
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
	fileMakerFutureWatcher->setFuture(fileMakerFuture);
	connect(fileMakerFutureWatcher, SIGNAL(finished()), this, SLOT(fileMakerWriteFinished()));
	mSaveThreads.push_back(fileMakerFutureWatcher);
	fileMaker.reset(); // filemaker is now stored in the mSaveThreads queue, clear as current.
}

void USSavingRecorder::fileMakerWriteFinished()
{
	std::list<QFutureWatcher<QString>*>::iterator iter;
	for (iter=mSaveThreads.begin(); iter!=mSaveThreads.end(); ++iter)
	{
		if (!(*iter)->isFinished())
			continue;
		QString result = (*iter)->future().result();
		delete *iter;
		iter = mSaveThreads.erase(iter);
		emit saveDataCompleted(result);
	}
}

std::map<double, ssc::Transform3D> USSavingRecorder::getRecording()
{
	ssc::TimedTransformMap retval;

	if(mRecordingTool && mSession)
		retval = mRecordingTool->getSessionHistory(mSession->getStartTime(), mSession->getStopTime());

	if(retval.empty() && mSession)
	{
		ssc::messageManager()->sendError("Could not find any tracking data from session "+mSession->getUid()+". Volume data only will be written.");
	}

	return retval;
}

}
