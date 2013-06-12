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

#include "sscReconstructManager.h"
#include "sscReconstructParams.h"
#include "sscBoolDataAdapterXml.h"

#include "cxPatientData.h"
#include "cxSettings.h"
#include "cxPatientService.h"
#include "cxVideoService.h"
#include "cxVideoConnectionManager.h"
#include "cxToolManager.h"
#include "cxUSSavingRecorder.h"
#include "cxAcquisitionData.h"

namespace cx
{

USAcquisition::USAcquisition(AcquisitionPtr base, QObject* parent) : QObject(parent), mBase(base)
{
	mCore.reset(new USSavingRecorder());
	connect(mCore.get(), SIGNAL(saveDataCompleted(QString)), this, SLOT(checkIfReadySlot()));
	connect(mCore.get(), SIGNAL(saveDataCompleted(QString)), this, SIGNAL(saveDataCompleted(QString)));


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

	int saving = mCore->getNumberOfSavingThreads();

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

int USAcquisition::getNumberOfSavingThreads() const
{
	return mCore->getNumberOfSavingThreads();
}

void USAcquisition::recordStarted()
{
	mBase->getPluginData()->getReconstructer()->selectData(ssc::USReconstructInputData()); // clear old data in reconstructeer

	ssc::ToolPtr tool = ToolManager::getInstance()->findFirstProbe();
	mCore->setWriteColor(this->getWriteColor());
	mCore->startRecord(mBase->getLatestSession(), tool, this->getRecordingVideoSources(tool));
}

void USAcquisition::recordStopped()
{
	mCore->stopRecord();

	this->sendAcquisitionDataToReconstructer();

	mCore->set_rMpr(*ssc::toolManager()->get_rMpr());
	bool compress = settings()->value("Ultrasound/CompressAcquisition", true).toBool();
	QString baseFolder = patientService()->getPatientData()->getActivePatientFolder();
	mCore->startSaveData(baseFolder, compress);

	mCore->clearRecording();

	this->checkIfReadySlot();
}

void USAcquisition::recordCancelled()
{
	mCore->cancelRecord();
}

void USAcquisition::sendAcquisitionDataToReconstructer()
{
	mCore->set_rMpr(*ssc::toolManager()->get_rMpr());

	ssc::VideoSourcePtr activeVideoSource = videoService()->getActiveVideoSource();
	if (activeVideoSource)
	{
		ssc::USReconstructInputData data = mCore->getDataForStream(activeVideoSource->getUid());
		mBase->getPluginData()->getReconstructer()->selectData(data);
		emit acquisitionDataReady();
	}
}

std::vector<ssc::VideoSourcePtr> USAcquisition::getRecordingVideoSources(ssc::ToolPtr tool)
{
	std::vector<ssc::VideoSourcePtr> retval;

	if (tool && tool->getProbe())
	{
		ssc::ProbePtr probe = tool->getProbe();
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

}
