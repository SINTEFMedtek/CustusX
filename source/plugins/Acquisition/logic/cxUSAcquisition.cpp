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

#include "cxUSAcquisition.h"

#include <QtConcurrentRun>
#include "boost/bind.hpp"

#include "sscToolManager.h"
#include "sscTypeConversions.h"
#include "cxPatientData.h"
#include "cxSettings.h"
#include "sscMessageManager.h"
#include "cxTool.h"
#include "cxPatientService.h"
#include "cxVideoService.h"
#include "sscReconstructManager.h"
#include "cxDataLocations.h"
#include "sscTime.h"
#include "cxUsReconstructionFileMaker.h"
#include "cxSavingVideoRecorder.h"
#include "sscReconstructParams.h"
#include "sscBoolDataAdapterXml.h"
#include "cxVideoConnectionManager.h"
#include "cxToolManager.h"
#include "sscLogger.h"
#include "cxImageDataContainer.h"

namespace cx
{

USAcquisition::USAcquisition(AcquisitionPtr base, QObject* parent) : QObject(parent), mBase(base)
{
	connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(checkIfReadySlot()));
	connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(checkIfReadySlot()));
	connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(checkIfReadySlot()));
	connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(checkIfReadySlot()));
	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(checkIfReadySlot()));
	connect(videoService(), SIGNAL(activeVideoSourceChanged()), this, SLOT(checkIfReadySlot()));
	connect(videoService()->getVideoConnection().get(), SIGNAL(connected(bool)), this, SLOT(checkIfReadySlot()));

	connect(mBase.get(), SIGNAL(started()), this, SLOT(recordStarted()));
	connect(mBase.get(), SIGNAL(stopped()), this, SLOT(recordStopped()));
	connect(mBase.get(), SIGNAL(cancelled()), this, SLOT(recordCancelled()));

	this->checkIfReadySlot();
}

USAcquisition::~USAcquisition()
{

}

void USAcquisition::checkIfReadySlot()
{
	bool tracking = ssc::toolManager()->isTracking();
	bool streaming = videoService()->getVideoConnection()->isConnected();
	ssc::ToolPtr tool = ToolManager::getInstance()->findFirstProbe();

	QString mWhatsMissing;
	mWhatsMissing.clear();

	if(tracking && streaming)
	{
		mWhatsMissing = "<font color=green>Ready to record!</font><br>";
		if (!tool || !tool->getVisible())
		{
			mWhatsMissing += "<font color=orange>Probe not visible</font><br>";
		}
	}
	else
	{
		if(!tracking)
			mWhatsMissing.append("<font color=red>Need to start tracking.</font><br>");
		if(!streaming)
			mWhatsMissing.append("<font color=red>Need to start streaming.</font><br>");
	}

	int saving = mSaveThreads.size();

	if (saving!=0)
		mWhatsMissing.append(QString("<font color=orange>Saving %1 acquisition data.</font><br>").arg(saving));

	// remove redundant line breaks
	QStringList list = mWhatsMissing.split("<br>", QString::SkipEmptyParts);
	mWhatsMissing = list.join("<br>");

	//Make sure we have at least 2 lines to avoid "bouncing buttons"
	if (list.size() < 2)
		mWhatsMissing.append("<br>");

	// do not require tracking to be present in order to perform an acquisition.
	mBase->setReady(streaming, mWhatsMissing);
}

ssc::TimedTransformMap USAcquisition::getRecording(RecordSessionPtr session)
{
	ssc::TimedTransformMap retval;

	if(mRecordingTool)
		retval = mRecordingTool->getSessionHistory(session->getStartTime(), session->getStopTime());

	if(retval.empty())
	{
		ssc::messageManager()->sendError("Could not find any tracking data from session "+session->getUid()+". Volume data only will be written.");
	}

	return retval;
}

void USAcquisition::saveSession()
{
	//get session data
	RecordSessionPtr session = mBase->getLatestSession();
	if (!session)
		return;

	ssc::TimedTransformMap trackerRecordedData = this->getRecording(session);
	ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();

	for (unsigned i=0; i<mVideoRecorder.size(); ++i)
	{
		CachedImageDataContainerPtr imageData = mVideoRecorder[i]->getImageData();
		std::vector<double> imageTimestamps = mVideoRecorder[i]->getTimestamps();
		QString streamSessionName = session->getDescription()+"_"+mVideoRecorder[i]->getSource()->getUid();

		// complete writing of images to temporary storage. Do this before using the image data.
		mVideoRecorder[i]->completeSave();
		mVideoRecorder[i].reset();
		std::cout << QString("completed save of cached video stream %1").arg(i) << std::endl;

		UsReconstructionFileMakerPtr fileMaker;
		fileMaker.reset(new UsReconstructionFileMaker(streamSessionName));

		ssc::USReconstructInputData reconstructData = fileMaker->getReconstructData(imageData,
		                                                                            imageTimestamps,
		                                                                            trackerRecordedData,
		                                                                            mRecordingTool,
		                                                                            this->getWriteColor(),
		                                                                            rMpr);
		fileMaker->setReconstructData(reconstructData);

		mBase->getPluginData()->getReconstructer()->selectData(reconstructData);
		emit acquisitionDataReady();

		QString saveFolder = UsReconstructionFileMaker::createFolder(patientService()->getPatientData()->getActivePatientFolder(), session->getDescription());

		// now start saving of data to the patient folder, compressed version:
		QFuture<QString> fileMakerFuture =
				QtConcurrent::run(boost::bind(
									  &UsReconstructionFileMaker::writeToNewFolder,
									  fileMaker,
									  saveFolder,
									  settings()->value("Ultrasound/CompressAcquisition", true).toBool()
									  ));
		QFutureWatcher<QString>* fileMakerFutureWatcher = new QFutureWatcher<QString>();
		fileMakerFutureWatcher->setFuture(fileMakerFuture);
		connect(fileMakerFutureWatcher, SIGNAL(finished()), this, SLOT(fileMakerWriteFinished()));
		mSaveThreads.push_back(fileMakerFutureWatcher);
		fileMaker.reset(); // filemaker is now stored in the mSaveThreads queue, clear as current.
	}

	mVideoRecorder.clear();

	this->checkIfReadySlot();
}

void USAcquisition::fileMakerWriteFinished()
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
	this->checkIfReadySlot();
}

std::vector<ssc::VideoSourcePtr> USAcquisition::getRecordingVideoSources()
{
	std::vector<ssc::VideoSourcePtr> retval;

	if (mRecordingTool && mRecordingTool->getProbe())
	{
		ssc::ProbePtr probe = mRecordingTool->getProbe();
		QStringList sources = probe->getAvailableVideoSources();
		for (unsigned i=0; i<sources.size(); ++i)
			retval.push_back(probe->getRTSource(sources[i]));
	}
	else
	{
		retval = videoService()->getVideoConnection()->getVideoSources();
	}

	return retval;
}

bool USAcquisition::getWriteColor() const
{
	bool writeColor = mBase->getPluginData()->getReconstructer()->getParams()->mAngioAdapter->getValue()
	        ||  !settings()->value("Ultrasound/8bitAcquisitionData").toBool();
	return writeColor;
}

void USAcquisition::recordStarted()
{
	// assert that previous recording have been cleared
	SSC_ASSERT(!mRecordingTool);
	SSC_ASSERT(mVideoRecorder.empty());

	mRecordingTool = ToolManager::getInstance()->findFirstProbe();
	std::vector<ssc::VideoSourcePtr> sources = this->getRecordingVideoSources();
	RecordSessionPtr session = mBase->getLatestSession();

	QString tempBaseFolder = DataLocations::getCachePath()+"/usacq/"+QDateTime::currentDateTime().toString(ssc::timestampSecondsFormat());
	QString cacheFolder = UsReconstructionFileMaker::createUniqueFolder(tempBaseFolder, session->getDescription());

	mBase->getPluginData()->getReconstructer()->selectData(ssc::USReconstructInputData()); // clear old data in reconstructeer

	for (unsigned i=0; i<sources.size(); ++i)
	{
		SavingVideoRecorderPtr videoRecorder;
		videoRecorder.reset(new SavingVideoRecorder(
		                         sources[i],
								 cacheFolder,
		                         QString("%1_%2").arg(session->getDescription()).arg(sources[i]->getUid()),
								 false, // no compression when saving to cache
								 this->getWriteColor()));
		videoRecorder->startRecord();
		mVideoRecorder.push_back(videoRecorder);
	}

	ssc::messageManager()->sendSuccess("Ultrasound acquisition started.", true);
}

void USAcquisition::recordStopped()
{
	for (unsigned i=0; i<mVideoRecorder.size(); ++i)
		mVideoRecorder[i]->stopRecord();
	ssc::messageManager()->sendSuccess("Ultrasound acquisition stopped.", true);
	this->saveSession();
	mRecordingTool.reset();
}

void USAcquisition::recordCancelled()
{
	mRecordingTool.reset();
	for (unsigned i=0; i<mVideoRecorder.size(); ++i)
	{
		mVideoRecorder[i]->stopRecord();
		mVideoRecorder[i]->cancel();
		mVideoRecorder[i].reset();
	}
	mVideoRecorder.clear();
	ssc::messageManager()->sendInfo("Ultrasound acquisition cancelled.");
}


}
