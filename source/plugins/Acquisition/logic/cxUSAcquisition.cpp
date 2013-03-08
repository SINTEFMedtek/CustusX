/*
 * cxUSAcquisition.cpp
 *
 *  \date May 12, 2011
 *      \author christiana
 */

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

namespace cx
{

USAcquisition::USAcquisition(AcquisitionPtr base, QObject* parent) : QObject(parent), mBase(base)
{
	connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(checkIfReadySlot()));
//	connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(c4learSlot()));
	connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(checkIfReadySlot()));
	connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(checkIfReadySlot()));
	connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(checkIfReadySlot()));
	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(checkIfReadySlot()));
//	connect(this, SIGNAL(toolChanged()), this, SLOT(probeChangedSlot()));

	connect(mBase.get(), SIGNAL(started()), this, SLOT(recordStarted()));
	connect(mBase.get(), SIGNAL(stopped()), this, SLOT(recordStopped()));
	connect(mBase.get(), SIGNAL(cancelled()), this, SLOT(recordCancelled()));

//	this->dominantToolChangedSlot();
//	this->probeChangedSlot();
	this->checkIfReadySlot();
//	this->connectToPureVideo();
}

USAcquisition::~USAcquisition()
{

}

//void USAcquisition::clearSlot()
//{
////	if (mTool)
////		disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this,
////		           SLOT(checkIfReadySlot()));
//	mRecordingTool.reset();
//}

void USAcquisition::checkIfReadySlot()
{
	bool tracking = ssc::toolManager()->isTracking();
	bool streaming = videoService()->getVideoConnection()->isConnected();
//	bool streaming = mRTSource && mRTSource->isStreaming();
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
//		if(mRTSource)
//		{
//			if(!streaming)
//				mWhatsMissing.append("<font color=red>Need to start streaming.</font><br>");
//		}
//		else
//		{
//			mWhatsMissing.append("<font color=red>Need to get a stream.</font><br>");
//		}
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

//void USAcquisition::setTool(ssc::ToolPtr tool) {
//	if (mTool && tool && (mTool->getUid() == tool->getUid()))
//		return;

//	if (mTool)
//		disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(checkIfReadySlot()));
//	mTool = tool;
//	if (mTool)
//		connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(checkIfReadySlot()));
//	emit toolChanged();
//}

//ssc::ToolPtr USAcquisition::getTool()
//{
//	return mTool;
//}

//void USAcquisition::probeChangedSlot()
//{
//	ssc::ToolPtr tool = this->getTool();
//	if(!tool)
//		return;
//	ssc::ProbePtr probe = tool->getProbe();
//	if(!probe)
//		return;
//	if(!probe->getRTSource())
//		return;

//	this->connectVideoSource(probe->getRTSource());
//}

//void USAcquisition::connectToPureVideo()
//{
//	if (mRTSource)
//		return;

//	this->connectVideoSource(videoService()->getActiveVideoSource());
//}

//void USAcquisition::connectVideoSource(ssc::VideoSourcePtr source)
//{
//	//Don't change source if it is the same as earlier
//	if (mRTSource == source)
//		return;

////	if (source)
////		std::cout << "USAcquisition::connectVideoSource " << source->getUid() << std::endl;

//	if(mRTSource)
//	{
//		disconnect(mRTSource.get(), SIGNAL(streaming(bool)), this, SLOT(checkIfReadySlot()));
//	}

//	mRTSource = source;

//	if(mRTSource)
//	{
//		connect(mRTSource.get(), SIGNAL(streaming(bool)), this, SLOT(checkIfReadySlot()));
//	}

//	this->checkIfReadySlot();
//}

ssc::TimedTransformMap USAcquisition::getRecording(RecordSessionPtr session)
{
	ssc::TimedTransformMap retval;

//	ssc::ToolPtr tool = this->getTool();
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

//	ssc::ToolPtr probe = this->getTool();

	for (unsigned i=0; i<mVideoRecorder.size(); ++i)
	{
		UsReconstructionFileMakerPtr fileMaker;
//		fileMaker.reset(new UsReconstructionFileMaker(session->getDescription()+mVideoRecorder[i]->getSource()->getUid()));
		fileMaker.reset(new UsReconstructionFileMaker(session->getDescription()+"_"+mVideoRecorder[i]->getSource()->getUid()));

		ssc::USReconstructInputData reconstructData = fileMaker->getReconstructData(mVideoRecorder[i], trackerRecordedData,
																									mRecordingTool,
																									this->getWriteColor());
		fileMaker->setReconstructData(reconstructData);

		// Use instead of filemaker->write(), this writes only images, other stuff kept in memory.
		mVideoRecorder[i]->completeSave();
		mVideoRecorder[i].reset();

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
//		QFutureWatcher<QString>* watcher = *iter;
//		emit saveDataCompleted(watcher->future().result());
		delete *iter;
		iter = mSaveThreads.erase(iter);
		emit saveDataCompleted(result);
	}
	this->checkIfReadySlot();
}

//void USAcquisition::dominantToolChangedSlot()
//{
//	ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();

//	ssc::ProbePtr probe = tool->getProbe();
//	if(!probe)
//		return;

//	this->setTool(tool);
//	this->probeChangedSlot();
//}

std::vector<ssc::VideoSourcePtr> USAcquisition::getRecordingVideoSources()
{
	std::vector<ssc::VideoSourcePtr> retval;

	if (mRecordingTool && mRecordingTool->getProbe())
	{
		ssc::ProbePtr probe = mRecordingTool->getProbe();
		QStringList sources = probe->getAvailableVideoSources();
//		std::cout << "no of sources " << sources.size() << std::endl;
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
}

void USAcquisition::recordCancelled()
{
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
