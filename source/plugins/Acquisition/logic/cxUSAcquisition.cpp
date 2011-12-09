/*
 * cxUSAcquisition.cpp
 *
 *  Created on: May 12, 2011
 *      Author: christiana
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

namespace cx
{

USAcquisition::USAcquisition(AcquisitionDataPtr pluginData, QObject* parent) : QObject(parent), mPluginData(pluginData)
{
  connect(&mFileMakerFutureWatcher, SIGNAL(finished()), this, SLOT(fileMakerWriteFinished()));
  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(checkIfReadySlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(checkIfReadySlot()));
  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(dominantToolChangedSlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(dominantToolChangedSlot()));
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
  connect(this, SIGNAL(toolChanged()), this, SLOT(probeChangedSlot()));

  this->probeChangedSlot();
  this->checkIfReadySlot();
  this->connectToPureVideo();
}

void USAcquisition::checkIfReadySlot()
{
  bool tracking = ssc::toolManager()->isTracking();
  bool streaming = mRTSource && mRTSource->isStreaming();

  mWhatsMissing.clear();

  if(tracking && streaming && mRTRecorder)
  {
  	mWhatsMissing = "<font color=green>Ready to record!</font><br>";
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
    if(!mRTRecorder)
    	mWhatsMissing.append("<font color=red>Need connect to a recorder.</font><br>");
  }

  // do not require tracking to be present in order to perform an acquisition.
  emit ready(streaming && mRTRecorder, mWhatsMissing);
}

void USAcquisition::setTool(ssc::ToolPtr tool)
{
  if(mTool && tool && (mTool->getUid() == tool->getUid()))
    return;

  mTool = tool;
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

  //TODO: Don't connect if same as before

	this->connectVideoSource(probe->getRTSource());
}


void USAcquisition::connectToPureVideo()
{
	if (mRTSource)
		return;

	this->connectVideoSource(videoService()->getVideoConnection()->getVideoSource());
}


void USAcquisition::connectVideoSource(ssc::VideoSourcePtr source)
{
  //TODO: Don't change source if it is the same as earlier
  if(mRTSource)
  {
    disconnect(mRTSource.get(), SIGNAL(streaming(bool)), this, SLOT(checkIfReadySlot()));
  }

  mRTSource = source;

  if(mRTSource)
  {
    std::cout << "USAcquisition::connectVideoSource - connected" << std::endl;
    connect(mRTSource.get(), SIGNAL(streaming(bool)), this, SLOT(checkIfReadySlot()));
    mRTRecorder.reset(new ssc::VideoRecorder(mRTSource));
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

void USAcquisition::saveSession(QString sessionId)
{
  //get session data
  RecordSessionPtr session = mPluginData->getRecordSession(sessionId);
  ssc::VideoRecorder::DataType streamRecordedData = mRTRecorder->getRecording(session->getStartTime(), session->getStopTime());

  ssc::TimedTransformMap trackerRecordedData = this->getRecording(session);
  if(trackerRecordedData.empty())
  {
    ssc::messageManager()->sendError("Could not find any tracking data from session "+sessionId+". Volume data only will be written.");
  }

  ssc::ToolPtr probe = this->getTool();

	QString calibFileName;
	ToolPtr cxTool = boost::dynamic_pointer_cast<Tool>(probe);
	if (cxTool)
		calibFileName = cxTool->getCalibrationFileName();

	// streamRecordedData is empty for ultrasonix? Incorrect time?
  mFileMaker.reset(new UsReconstructionFileMaker(trackerRecordedData, streamRecordedData, session->getDescription(), patientService()->getPatientData()->getActivePatientFolder(), probe, calibFileName));

  mFileMakerFuture = QtConcurrent::run(boost::bind(&UsReconstructionFileMaker::write, mFileMaker));
  mFileMakerFutureWatcher.setFuture(mFileMakerFuture);
}

void USAcquisition::fileMakerWriteFinished()
{
  QString targetFolder = mFileMakerFutureWatcher.future().result();
  mRTRecorder.reset(new ssc::VideoRecorder(mRTSource));
  emit saveDataCompleted(mFileMaker->getMhdFilename(targetFolder));
}

void USAcquisition::dominantToolChangedSlot()
{
//  std::cout << "USAcquisition::dominantToolChangedSlot()" << std::endl;
  ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();

  ssc::ProbePtr probe = tool->getProbe();
  if(!probe)
    return;

  if (this->getTool() && this->getTool()->getProbe())
    disconnect(this->getTool()->getProbe().get(), SIGNAL(sectorChanged()), this, SLOT(probeChangedSlot()));

  //TODO: Check
  connect(probe.get(), SIGNAL(sectorChanged()), this, SLOT(probeChangedSlot()));//Is this necessary?

  this->setTool(tool);

  this->probeChangedSlot();
}

void USAcquisition::startRecord()
{
  mRTRecorder->startRecord();
  ssc::messageManager()->sendSuccess("Ultrasound acquisition started.", true);
}

void USAcquisition::stopRecord()
{
  mRTRecorder->stopRecord();
  ssc::messageManager()->sendSuccess("Ultrasound acquisition stopped.", true);
}


}
