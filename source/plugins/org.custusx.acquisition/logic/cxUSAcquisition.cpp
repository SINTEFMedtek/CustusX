/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxUSAcquisition.h"

#include "cxBoolProperty.h"

#include "cxSettings.h"
#include "cxVideoService.h"
#include "cxTrackingService.h"
#include "cxUSSavingRecorder.h"
#include "cxAcquisitionData.h"
#include "cxUsReconstructionService.h"
#include "cxUSReconstructInputData.h"
#include "cxPatientModelService.h"
#include "cxVideoSource.h"
#include "cxAcquisitionService.h"
#include "cxUsReconstructionService.h"
#include "cxVisServices.h"


namespace cx
{

USAcquisition::USAcquisition(AcquisitionPtr base, QObject* parent) :
	QObject(parent),
	mBase(base),
	mReady(true),
	mInfoText("")
{
	mCore.reset(new USSavingRecorder());
	connect(mCore.get(), SIGNAL(saveDataCompleted(QString)), this, SLOT(checkIfReadySlot()));
	connect(mCore.get(), SIGNAL(saveDataCompleted(QString)), this, SIGNAL(saveDataCompleted(QString)));


	connect(this->getServices()->tracking().get(), &TrackingService::stateChanged, this, &USAcquisition::checkIfReadySlot);
	connect(this->getServices()->tracking().get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(checkIfReadySlot()));
	connect(this->getServices()->video().get(), SIGNAL(activeVideoSourceChanged()), this, SLOT(checkIfReadySlot()));
	connect(this->getServices()->video().get(), &VideoService::connected, this, &USAcquisition::checkIfReadySlot);

	connect(mBase.get(), SIGNAL(started()), this, SLOT(recordStarted()));
	connect(mBase.get(), SIGNAL(acquisitionStopped()), this, SLOT(recordStopped()), Qt::QueuedConnection);
	connect(mBase.get(), SIGNAL(cancelled()), this, SLOT(recordCancelled()));

	this->checkIfReadySlot();
}

USAcquisition::~USAcquisition()
{

}

VisServicesPtr USAcquisition::getServices()
{
	return mBase->getPluginData()->getServices();
}

UsReconstructionServicePtr USAcquisition::getReconstructer()
{
	return mBase->getPluginData()->getReconstructer();
}

bool USAcquisition::isReady(AcquisitionService::TYPES context) const
{
	if (!context.testFlag(AcquisitionService::tUS))
		return true;
	return mReady;
}

QString USAcquisition::getInfoText(AcquisitionService::TYPES context) const
{
	if (!context.testFlag(AcquisitionService::tUS))
		return "";
	return mInfoText;
}

void USAcquisition::checkIfReadySlot()
{
	bool tracking = this->getServices()->tracking()->getState()>=Tool::tsTRACKING;
	bool streaming = this->getServices()->video()->isConnected();
	ToolPtr tool = this->getServices()->tracking()->getFirstProbe();
    ToolPtr reference_frame = this->getServices()->tracking()->getReferenceTool();
    if(tracking && reference_frame)
    {
        QObject::connect(reference_frame.get(), &Tool::toolVisible, this, &USAcquisition::checkIfReadySlot, Qt::UniqueConnection);
    }

	QString mWhatsMissing;
	mWhatsMissing.clear();

	if(tracking && streaming)
	{
		mWhatsMissing = "<font color=green>Ready to record!</font><br>";
		if (!tool || !tool->getVisible())
		{
            mWhatsMissing += "<font color=orange>Probe not visible.</font><br>";
        }
        if(!reference_frame || !reference_frame->getVisible())
        {
            mWhatsMissing += "<font color=orange>Reference frame not visible.</font><br>";
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
	this->setReady(streaming, mWhatsMissing);
}

void USAcquisition::setReady(bool val, QString text)
{
	mReady = val;
	mInfoText = text;

	emit readinessChanged();
}

int USAcquisition::getNumberOfSavingThreads() const
{
	return mCore->getNumberOfSavingThreads();
}

void USAcquisition::recordStarted()
{
	if (!mBase->getCurrentContext().testFlag(AcquisitionService::tUS))
		return;

	this->getReconstructer()->selectData(USReconstructInputData()); // clear old data in reconstructeer

	ToolPtr tool = this->getServices()->tracking()->getFirstProbe();
	mCore->setWriteColor(this->getWriteColor());
	mCore->startRecord(mBase->getLatestSession(),
										 tool,
										 this->getServices()->tracking()->getReferenceTool(),
										 this->getRecordingVideoSources(tool),
										 this->getServices()->file());
}

void USAcquisition::recordStopped()
{
	if (!mBase->getCurrentContext().testFlag(AcquisitionService::tUS))
		return;

	mCore->stopRecord();

	this->sendAcquisitionDataToReconstructer();

	mCore->set_rMpr(this->getServices()->patient()->get_rMpr());
	bool compress = settings()->value("Ultrasound/CompressAcquisition", true).toBool();
	QString baseFolder = this->getServices()->patient()->getActivePatientFolder();
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
	mCore->set_rMpr(this->getServices()->patient()->get_rMpr());

	VideoSourcePtr activeVideoSource = this->getServices()->video()->getActiveVideoSource();
	if (activeVideoSource)
	{
		USReconstructInputData data = mCore->getDataForStream(activeVideoSource->getUid());
		this->getReconstructer()->selectData(data);
		emit acquisitionDataReady();
	}
}

std::vector<VideoSourcePtr> USAcquisition::getRecordingVideoSources(ToolPtr tool)
{
	std::vector<VideoSourcePtr> retval;

	if (tool && tool->getProbe())
	{
		ProbePtr probe = tool->getProbe();
		QStringList sources = probe->getAvailableVideoSources();
		for (unsigned i=0; i<sources.size(); ++i)
			retval.push_back(probe->getRTSource(sources[i]));
	}
	else
	{
		retval = this->getServices()->video()->getVideoSources();
	}

	return retval;
}

bool USAcquisition::getWriteColor()
{
	PropertyPtr angio = this->getReconstructer()->getParam("Angio data");
	bool writeColor = angio->getValueAsVariant().toBool()
	        ||  !settings()->value("Ultrasound/8bitAcquisitionData").toBool();
	return writeColor;
}

}
