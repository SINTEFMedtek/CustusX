/*
 * cxUSAcquisition.cpp
 *
 *  \date May 12, 2011
 *      \author christiana
 */

#include <cxUSAcquisition.h>
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

namespace cx
{

USAcquisition::USAcquisition(AcquisitionPtr base, QObject* parent) : QObject(parent), mBase(base)
{
	connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(checkIfReadySlot()));
	connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(clearSlot()));
	connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(checkIfReadySlot()));
	connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(dominantToolChangedSlot()));
	connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(dominantToolChangedSlot()));
	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
	connect(this, SIGNAL(toolChanged()), this, SLOT(probeChangedSlot()));

//	connect(mBase.get(), SIGNAL(newSession()), this, SLOT(saveSession()));
	connect(mBase.get(), SIGNAL(started()), this, SLOT(recordStarted()));
	connect(mBase.get(), SIGNAL(stopped()), this, SLOT(recordStopped()));
	connect(mBase.get(), SIGNAL(cancelled()), this, SLOT(recordCancelled()));
//	connect(this, SIGNAL(acquisitionDataReady()), this, SLOT(startReconstruction()));

	this->dominantToolChangedSlot();
	this->probeChangedSlot();
	this->checkIfReadySlot();
	this->connectToPureVideo();
}

void USAcquisition::clearSlot()
{
	if (mTool)
		disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this,
		           SLOT(checkIfReadySlot()));
	mTool.reset();
}

void USAcquisition::checkIfReadySlot()
{
	bool tracking = ssc::toolManager()->isTracking();
	bool streaming = mRTSource && mRTSource->isStreaming();

	mWhatsMissing.clear();

//	if(tracking && streaming && mRTRecorder)
	if(tracking && streaming)
	{
		mWhatsMissing = "<font color=green>Ready to record!</font><br>";
		if (!mTool || !mTool->getVisible())
		{
			mWhatsMissing += "<font color=orange>Probe not visible</font><br>";
		}
	}
	else
	{
		if(!tracking)
			mWhatsMissing.append("<font color=red>Need to start tracking.</font><br>");
		if(mRTSource)
		{
			if(!streaming)
				mWhatsMissing.append("<font color=red>Need to start streaming.</font><br>");
		}
		else
		{
			mWhatsMissing.append("<font color=red>Need to get a stream.</font><br>");
		}
//		if(!mRTRecorder)
//			mWhatsMissing.append("<font color=red>Need connect to a recorder.</font><br>");
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
//	emit ready(streaming, mWhatsMissing);
	mBase->setReady(streaming, mWhatsMissing);
}

void USAcquisition::setTool(ssc::ToolPtr tool) {
	if (mTool && tool && (mTool->getUid() == tool->getUid()))
		return;

	if (mTool)
		disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(checkIfReadySlot()));
	mTool = tool;
	if (mTool)
		connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(checkIfReadySlot()));
	emit toolChanged();
}

ssc::ToolPtr USAcquisition::getTool()
{
	return mTool;
}

void USAcquisition::probeChangedSlot()
{
	ssc::ToolPtr tool = this->getTool();
	if(!tool)
		return;
	ssc::ProbePtr probe = tool->getProbe();
	if(!probe)
		return;
	if(!probe->getRTSource())
		return;

	this->connectVideoSource(probe->getRTSource());
}

void USAcquisition::connectToPureVideo()
{
	if (mRTSource)
		return;

	this->connectVideoSource(videoService()->getActiveVideoSource());
}

void USAcquisition::connectVideoSource(ssc::VideoSourcePtr source)
{
	//Don't change source if it is the same as earlier
	if (mRTSource == source)
		return;

	if(mRTSource)
	{
		disconnect(mRTSource.get(), SIGNAL(streaming(bool)), this, SLOT(checkIfReadySlot()));
	}

	mRTSource = source;

	if(mRTSource)
	{
		connect(mRTSource.get(), SIGNAL(streaming(bool)), this, SLOT(checkIfReadySlot()));
	}

	this->checkIfReadySlot();
}


ssc::TimedTransformMap USAcquisition::getRecording(RecordSessionPtr session)
{
	ssc::TimedTransformMap retval;

	ssc::ToolPtr tool = this->getTool();
	if(tool)
		retval = tool->getSessionHistory(session->getStartTime(), session->getStopTime());

	return retval;
}

//void USAcquisition::acquisitionStateChangedSlot()
//{
//	Acquisition::STATE state = mBase->getState();

//	switch (state)
//	{
//	case Acquisition::sRUNNING :
//		break;
//	case Acquisition::sNOT_RUNNING :
//		break;
//	case Acquisition::sPOST_PROCESSING :
//		break;
//	}
//}

void USAcquisition::saveSession()
{
	//get session data
	RecordSessionPtr session = mBase->getLatestSession();
	if (!session)
		return;

	ssc::TimedTransformMap trackerRecordedData = this->getRecording(session);
	if(trackerRecordedData.empty())
	{
		ssc::messageManager()->sendError("Could not find any tracking data from session "+session->getUid()+". Volume data only will be written.");
	}

	ssc::ToolPtr probe = this->getTool();

	QString calibFileName;
	ToolPtr cxTool = boost::dynamic_pointer_cast<Tool>(probe);
	if (cxTool)
		calibFileName = cxTool->getCalibrationFileName();

	bool writeColor = mBase->getPluginData()->getReconstructer()->getParams()->mAngioAdapter->getValue();

	mCurrentSessionFileMaker->setData(trackerRecordedData, mVideoRecorder,
                                      probe, calibFileName,
                                      writeColor);
	mCurrentSessionFileMaker->setDeleteFilesOnRelease(true);

	///TODO this forces write of images to disk, but also writes other crap we dont need.
	//mCurrentSessionFileMaker->write();
	// Use instead of filemaker->write(), this writes only images, other stuff kept in memory.
	mVideoRecorder->completeSave();
	mVideoRecorder.reset();

	ssc::USReconstructInputData reconstructData = mCurrentSessionFileMaker->getReconstructData();
	mBase->getPluginData()->getReconstructer()->selectData(reconstructData);
	emit acquisitionDataReady();

	// now start saving of data to the patient folder, compressed version:
	QFuture<QString> fileMakerFuture =
	        QtConcurrent::run(boost::bind(
	                              &UsReconstructionFileMaker::writeToNewFolder,
	                              mCurrentSessionFileMaker,
	                              patientService()->getPatientData()->getActivePatientFolder(),
	                              settings()->value("Ultrasound/CompressAcquisition", true).toBool()
	                              ));
	QFutureWatcher<QString>* fileMakerFutureWatcher = new QFutureWatcher<QString>();
	fileMakerFutureWatcher->setFuture(fileMakerFuture);
	connect(fileMakerFutureWatcher, SIGNAL(finished()), this, SLOT(fileMakerWriteFinished()));
	mSaveThreads.push_back(fileMakerFutureWatcher);
	mCurrentSessionFileMaker.reset(); // filemaker is now stored in the mSaveThreads queue, clear as current.

	this->checkIfReadySlot();
}

void USAcquisition::fileMakerWriteFinished()
{
	std::list<QFutureWatcher<QString>*>::iterator iter;
	for (iter=mSaveThreads.begin(); iter!=mSaveThreads.end(); ++iter)
	{
		if (!(*iter)->isFinished())
			continue;
		QFutureWatcher<QString>* watcher = *iter;
		emit saveDataCompleted(watcher->future().result());
		delete *iter;
		iter = mSaveThreads.erase(iter);
	}
	this->checkIfReadySlot();
}

void USAcquisition::dominantToolChangedSlot()
{
	ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();

	ssc::ProbePtr probe = tool->getProbe();
	if(!probe)
		return;

	this->setTool(tool);
	this->probeChangedSlot();
}

void USAcquisition::recordStarted()
{
	RecordSessionPtr session = mBase->getLatestSession();

	QString tempFolder = DataLocations::getCachePath()+"/usacq/"+QDateTime::currentDateTime().toString(ssc::timestampSecondsFormat());
	mCurrentSessionFileMaker.reset(new UsReconstructionFileMaker(
	                                   session->getDescription(),
	                                   tempFolder));

	mBase->getPluginData()->getReconstructer()->selectData(ssc::USReconstructInputData()); // clear old data in reconstructeer
	bool writeColor = mBase->getPluginData()->getReconstructer()->getParams()->mAngioAdapter->getValue()
	        ||  !settings()->value("Ultrasound/8bitAcquisitionData").toBool();

	mVideoRecorder.reset(new SavingVideoRecorder(
	                         mRTSource,
	                         mCurrentSessionFileMaker->getFolderName(),
	                         mCurrentSessionFileMaker->getSessionName(),
	                         false, // no compression when saving to cache
	                         writeColor));
	mVideoRecorder->startRecord();

	ssc::messageManager()->sendSuccess("Ultrasound acquisition started.", true);
}

void USAcquisition::recordStopped()
{
	mVideoRecorder->stopRecord();
	ssc::messageManager()->sendSuccess("Ultrasound acquisition stopped.", true);
	this->saveSession();
}

void USAcquisition::recordCancelled()
{
	mVideoRecorder->stopRecord();
	mVideoRecorder->cancel();
	mVideoRecorder.reset();
	ssc::messageManager()->sendInfo("Ultrasound acquisition stopped.");
}


}
