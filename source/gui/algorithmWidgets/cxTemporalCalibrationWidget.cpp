/*
 * cxTemporalCalibrationWidget.cpp
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#include <cxTemporalCalibrationWidget.h>


#include <QtGui>
#include <QVBoxLayout>
#include "boost/bind.hpp"
#include "sscToolManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscDoubleWidgets.h"
#include "sscTypeConversions.h"
#include "cxPatientData.h"
#include "cxStateMachineManager.h"
#include "cxRecordSessionWidget.h"
#include "cxSettings.h"
#include "cxToolDataAdapters.h"
#include "cxDoubleDataAdapterTemporalCalibration.h"

#include "vtkImageCorrelation.h"
typedef vtkSmartPointer<vtkImageCorrelation> vtkImageCorrelationPtr;

namespace cx
{


TemporalCalibrationWidget::TemporalCalibrationWidget(QWidget* parent) :
    TrackedRecordWidget(parent, "temporal_cal")
{
  this->setObjectName("TemporalCalibrationWidget");
  this->setWindowTitle("Temporal Calibration");

  mRecordSessionWidget->setDescriptionVisibility(false);

  connect(&mFileMakerFutureWatcher, SIGNAL(finished()), this, SLOT(fileMakerWriteFinished()));
  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(checkIfReadySlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(checkIfReadySlot()));
  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(dominantToolChangedSlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(dominantToolChangedSlot()));
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
  connect(this, SIGNAL(toolChanged()), this, SLOT(probeChangedSlot()));

  RecordBaseWidget::mLayout->addWidget(new ssc::LabeledComboBoxWidget(this, ActiveToolConfigurationStringDataAdapter::New()));
  mLayout->addStretch();

  this->probeChangedSlot();
  this->checkIfReadySlot();
}

TemporalCalibrationWidget::~TemporalCalibrationWidget()
{}

QString TemporalCalibrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Temporal Calibration.</h3>"
      "<p><i>Calibrate the time shift between the tracking system and the video acquisition source.</i></br>"
      "</html>";
}

void TemporalCalibrationWidget::checkIfReadySlot()
{
  bool tracking = ssc::toolManager()->isTracking();
  bool streaming = mRTSource && mRTSource->isStreaming();

  if(tracking && streaming && mRTRecorder)
    RecordBaseWidget::setWhatsMissingInfo("<font color=green>Ready to record!</font><br>");
  else
  {
    QString whatsMissing("");
    if(!tracking)
      whatsMissing.append("<font color=red>Need to start tracking.</font><br>");
    if(mRTSource)
    {
      if(!streaming)
        whatsMissing.append("<font color=red>Need to start streaming.</font><br>");
    }else
    {
      whatsMissing.append("<font color=red>Need to get a stream.</font><br>");
    }
    if(!mRTRecorder)
       whatsMissing.append("<font color=red>Need connect to a recorder.</font><br>");

    RecordBaseWidget::setWhatsMissingInfo(whatsMissing);
  }

  // do not require tracking to be present in order to perform an acquisition.
  emit ready(streaming && mRTRecorder);
}

void TemporalCalibrationWidget::probeChangedSlot()
{
  ssc::ToolPtr tool = this->getTool();
  if(!tool)
    return;

  ssc::ProbePtr probe = tool->getProbe();
  if(!probe)
    return;

  if(mRTSource)
  {
    disconnect(mRTSource.get(), SIGNAL(streaming(bool)), this, SLOT(checkIfReadySlot()));
  }
  mRTSource = probe->getRTSource();

  if(mRTSource)
  {
    connect(mRTSource.get(), SIGNAL(streaming(bool)), this, SLOT(checkIfReadySlot()));
    mRTRecorder.reset(new ssc::RTSourceRecorder(mRTSource));
  }
  this->checkIfReadySlot();
}

ssc::TimedTransformMap TemporalCalibrationWidget::getRecording(RecordSessionPtr session)
{
  ssc::TimedTransformMap retval;

  ssc::ToolPtr tool = this->getTool();
  if(tool)
    retval = tool->getSessionHistory(session->getStartTime(), session->getStopTime());

  return retval;
}


void TemporalCalibrationWidget::postProcessingSlot(QString sessionId)
{
  //get session data
  mLastSession = sessionId;
  RecordSessionPtr session = stateManager()->getRecordSession(sessionId);
  ssc::RTSourceRecorder::DataType streamRecordedData = mRTRecorder->getRecording(session->getStartTime(), session->getStopTime());

  ssc::TimedTransformMap trackerRecordedData = this->getRecording(session);
  if(trackerRecordedData.empty())
  {
    ssc::messageManager()->sendError("Could not find any tracking data from session "+sessionId+". Volume data only will be written.");
  }

  ssc::ToolPtr probe = this->getTool();
  mFileMaker.reset(new UsReconstructionFileMaker(trackerRecordedData, streamRecordedData, session->getDescription(), stateManager()->getPatientData()->getActivePatientFolder(), probe));

  mFileMakerFuture = QtConcurrent::run(boost::bind(&UsReconstructionFileMaker::write, mFileMaker));
  mFileMakerFutureWatcher.setFuture(mFileMakerFuture);
}

void TemporalCalibrationWidget::fileMakerWriteFinished()
{
  QString targetFolder = mFileMakerFutureWatcher.future().result();
  //stateManager()->getReconstructer()->selectData(mFileMaker->getMhdFilename(targetFolder));

  mRTRecorder.reset(new ssc::RTSourceRecorder(mRTSource));

  RecordSessionPtr session = stateManager()->getRecordSession(mLastSession);
  ssc::RTSourceRecorder::DataType streamRecordedData = mRTRecorder->getRecording(session->getStartTime(), session->getStopTime());
  ssc::TimedTransformMap trackerRecordedData = this->getRecording(session);
  if(trackerRecordedData.empty())
  {
    ssc::messageManager()->sendError("Could not find any tracking data from session "+mLastSession+". Ignoring.");
    return;
  }
  ssc::ToolPtr probe = this->getTool();

  this->computeTemporalCalibration(streamRecordedData, trackerRecordedData, probe);

//
//
//
//  if (settings()->value("Automation/autoReconstruct").toBool())
//  {
//    mThreadedReconstructer.reset(new ssc::ThreadedReconstructer(stateManager()->getReconstructer()));
//    connect(mThreadedReconstructer.get(), SIGNAL(finished()), this, SLOT(reconstructFinishedSlot()));
//    mThreadedReconstructer->start();
//    mRecordSessionWidget->startPostProcessing("Reconstructing");
//  }
}

void TemporalCalibrationWidget::computeTemporalCalibration(ssc::RTSourceRecorder::DataType volumes, ssc::TimedTransformMap tracking, ssc::ToolPtr probe)
{
  // - use correlation or convolution to find shifts between volumes
  // - compute the component of the shift in e_z in t space, s(t)
  // - compute tracking data along e_z in t space, q(t).
  // - use correlation to find shift between the two sequences.
  // - the shift is the difference between frame timestamps and tracking timestamps -> temporal calibration.
  // - add this as a delta to the current cal.
  ssc::RTSourceRecorder::DataType corrTemp;
  ssc::RTSourceRecorder::DataType::iterator i = volumes.begin();
  ssc::RTSourceRecorder::DataType::iterator j = i;
  ++j;
  for ( ; j!=volumes.end(); ++j, ++i)
  {
    vtkImageCorrelationPtr correlator = vtkImageCorrelationPtr::New();
    correlator->SetInput1(i->second);
    correlator->SetInput2(j->second);
    correlator->Update();
    vtkImageDataPtr result = correlator->GetOutput();
    corrTemp[j->first] = result;
  }

  mFileMaker.reset(new UsReconstructionFileMaker(tracking, corrTemp, "corr_test", stateManager()->getPatientData()->getActivePatientFolder(), probe));
  mFileMaker->write();
  std::cout << "completed write of correlation results" << std::endl;
}


void TemporalCalibrationWidget::dominantToolChangedSlot()
{
  ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();

  ssc::ProbePtr probe = tool->getProbe();
  if(!probe)
    return;

  if (this->getTool() && this->getTool()->getProbe())
    disconnect(this->getTool()->getProbe().get(), SIGNAL(sectorChanged()), this, SLOT(probeChangedSlot()));

  connect(probe.get(), SIGNAL(sectorChanged()), this, SLOT(probeChangedSlot()));

  this->setTool(tool);

  this->probeChangedSlot();
}

void TemporalCalibrationWidget::reconstructFinishedSlot()
{
  mRecordSessionWidget->stopPostProcessing();
//  mThreadedReconstructer.reset();
}

void TemporalCalibrationWidget::startedSlot()
{
  mRecordSessionWidget->setDescription("temporal_cal");
  mRTRecorder->startRecord();
  ssc::messageManager()->sendSuccess("Ultrasound acquisition started.", true);
}

void TemporalCalibrationWidget::stoppedSlot()
{
//  if (mThreadedReconstructer)
//  {
//    mThreadedReconstructer->terminate();
//    mThreadedReconstructer->wait();
//    stateManager()->getReconstructer()->selectData(stateManager()->getReconstructer()->getSelectedData());
//    // TODO perform cleanup of all resources connected to this recording.
//  }

  mRTRecorder->stopRecord();
  ssc::messageManager()->sendSuccess("Ultrasound acquisition stopped.", true);
}

}//namespace cx


