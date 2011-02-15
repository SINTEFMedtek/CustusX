#include "cxUSAcqusitionWidget.h"

#include <QtGui>
#include <QSettings>
#include <QVBoxLayout>
#include "boost/bind.hpp"
#include "sscToolManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxPatientData.h"
#include "cxStateMachineManager.h"
#include "cxSoundSpeedConversionWidget.h"
#include "cxRecordSessionWidget.h"
#include "cxDataLocations.h"
#include "cxToolPropertiesWidget.h"

namespace cx
{

USAcqusitionWidget::USAcqusitionWidget(QWidget* parent) :
    TrackedRecordWidget(parent, DataLocations::getSettings()->value("Ultrasound/acquisitionName").toString())
{
  this->setObjectName("USAcqusitionWidget");
  this->setWindowTitle("US Acquisition");

  mRecordSessionWidget->setDescriptionVisibility(false);

  connect(&mFileMakerFutureWatcher, SIGNAL(finished()), this, SLOT(fileMakerWriteFinished()));
  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(checkIfReadySlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(checkIfReadySlot()));
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
  connect(this, SIGNAL(toolChanged()), this, SLOT(probeChangedSlot()));

  //for testing sound speed converting - BEGIN
  SoundSpeedConverterWidget* soundSpeedWidget = new SoundSpeedConverterWidget(this);
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), soundSpeedWidget, SLOT(setToolSlot(const QString&)));
  //for testing sound speed converting - END

  RecordBaseWidget::mLayout->addWidget(new ssc::LabeledComboBoxWidget(this, ActiveToolConfigurationStringDataAdapter::New()));
  mLayout->addStretch();
  mLayout->addWidget(soundSpeedWidget);
  RecordBaseWidget::mLayout->addWidget(new ssc::SpinBoxGroupWidget(this, DoubleDataAdapterTimeCalibration::New()));

  this->probeChangedSlot();
  this->checkIfReadySlot();
}

USAcqusitionWidget::~USAcqusitionWidget()
{}

void USAcqusitionWidget::checkIfReadySlot()
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

void USAcqusitionWidget::probeChangedSlot()
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

ssc::TimedTransformMap USAcqusitionWidget::getRecording(RecordSessionPtr session)
{
  ssc::TimedTransformMap retval;

  ToolPtr tool = this->getTool();
  if(tool)
    retval = tool->getSessionHistory(session->getStartTime(), session->getStopTime());

  return retval;
}

void USAcqusitionWidget::postProcessingSlot(QString sessionId)
{
  //get session data
  RecordSessionPtr session = stateManager()->getRecordSession(sessionId);
  ssc::RTSourceRecorder::DataType streamRecordedData = mRTRecorder->getRecording(session->getStartTime(), session->getStopTime());

  ssc::TimedTransformMap trackerRecordedData = this->getRecording(session);
  if(trackerRecordedData.empty())
  {
    ssc::messageManager()->sendError("Could not find any tracking data from session "+sessionId+". Volume data only will be written.");
  }

  ToolPtr probe = this->getTool();
  mFileMaker.reset(new UsReconstructionFileMaker(trackerRecordedData, streamRecordedData, session->getDescription(), stateManager()->getPatientData()->getActivePatientFolder(), probe));

  mFileMakerFuture = QtConcurrent::run(boost::bind(&UsReconstructionFileMaker::write, mFileMaker));
  mFileMakerFutureWatcher.setFuture(mFileMakerFuture);
}

void USAcqusitionWidget::fileMakerWriteFinished()
{
  QString targetFolder = mFileMakerFutureWatcher.future().result();
  stateManager()->getReconstructer()->selectData(mFileMaker->getMhdFilename(targetFolder));

  mRTRecorder.reset(new ssc::RTSourceRecorder(mRTSource));

  if (DataLocations::getSettings()->value("Automation/autoReconstruct").toBool())
  {
    mThreadedReconstructer.reset(new ssc::ThreadedReconstructer(stateManager()->getReconstructer()));
    connect(mThreadedReconstructer.get(), SIGNAL(finished()), this, SLOT(reconstructFinishedSlot()));
    mThreadedReconstructer->start();
    mRecordSessionWidget->startPostProcessing("Reconstructing");
  }
}

void USAcqusitionWidget::dominantToolChangedSlot()
{
  ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();

  ssc::ProbePtr probe = tool->getProbe();
  if(!probe)
    return;

  ToolPtr cxTool = boost::dynamic_pointer_cast<Tool>(tool);
  if(!cxTool)
    return;

  this->setTool(cxTool);
}

void USAcqusitionWidget::reconstructFinishedSlot()
{
  mRecordSessionWidget->stopPostProcessing();
  mThreadedReconstructer.reset();
}

void USAcqusitionWidget::startedSlot()
{
  mRecordSessionWidget->setDescription(DataLocations::getSettings()->value("Ultrasound/acquisitionName").toString());
  mRTRecorder->startRecord();
}

void USAcqusitionWidget::stoppedSlot()
{
  if (mThreadedReconstructer)
  {
    mThreadedReconstructer->terminate();
    mThreadedReconstructer->wait();
    stateManager()->getReconstructer()->selectData(stateManager()->getReconstructer()->getSelectedData());
    // TODO perform cleanup of all resources connected to this recording.
  }

  mRTRecorder->stopRecord();
}
}//namespace cx
