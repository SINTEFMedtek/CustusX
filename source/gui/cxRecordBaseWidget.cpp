#include "cxRecordBaseWidget.h"

#include <QPushButton>
#include <QFont>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <vtkPolyData.h>
#include "boost/bind.hpp"
#include "sscToolManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscMeshHelpers.h"
#include "sscTransform3D.h"
#include "sscToolRep3D.h"
#include "sscToolTracer.h"
#include "sscReconstructer.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"
#include "cxSegmentation.h"
#include "cxRecordSessionWidget.h"
#include "cxTrackingDataToVolume.h"
#include "cxRepManager.h"
#include "cxViewManager.h"
#include "cxView3D.h"
#include "cxUsReconstructionFileMaker.h"
#include "cxToolPropertiesWidget.h"
#include "RTSource/cxRTSourceManager.h"
#include "cxDataLocations.h"
#include "cxProbe.h"
#include "cxSoundSpeedConversionWidget.h"

namespace cx
{

DoubleDataAdapterTimeCalibration::DoubleDataAdapterTimeCalibration()
{
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChanged()));
  this->dominantToolChanged();
}

void DoubleDataAdapterTimeCalibration::dominantToolChanged()
{
//  std::cout << "DoubleDataAdapterTimeCalibration::dominantToolChanged()" << std::endl;

  // ignore tool changes to something non-probeish.
  // This gives the user a chance to use the widget without having to show the probe.
  ssc::ToolPtr newTool = ssc::toolManager()->getDominantTool();
  if (!newTool || newTool->getProbeSector().mType==ssc::ProbeData::tNONE)
    return;

  if (mTool)
    disconnect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

  mTool = ssc::toolManager()->getDominantTool();

  if (mTool)
    connect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

//  std::cout << "DoubleDataAdapterTimeCalibration::dominantToolChanged() .. " << mTool.get() << std::endl;
  emit changed();
}

ssc::DoubleDataAdapterPtr DoubleDataAdapterTimeCalibration::New()
{
  return ssc::DoubleDataAdapterPtr(new DoubleDataAdapterTimeCalibration());
}

double DoubleDataAdapterTimeCalibration::getValue() const
{
//  std::cout << "DoubleDataAdapterTimeCalibration::getValue()" << std::endl;
  if (!mTool)
    return 0;
//  std::cout << "mTool->getProbe()->getData().mTemporalCalibration " << mTool->getProbe()->getData().mTemporalCalibration << std::endl;
  return mTool->getProbe()->getData().mTemporalCalibration;
}

QString DoubleDataAdapterTimeCalibration::getHelp() const
{
  return "Set a temporal shift to add to input probe frames. Will NOT be saved.";
}

bool DoubleDataAdapterTimeCalibration::setValue(double val)
{
  if (!mTool)
    return 0;
  mTool->getProbe()->setTemporalCalibration(val);
  return true;
}

ssc::DoubleRange DoubleDataAdapterTimeCalibration::getValueRange() const
{
  return ssc::DoubleRange(-1000,1000,1);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
RecordBaseWidget::RecordBaseWidget(QWidget* parent, QString description):
    QWidget(parent),
    mLayout(new QVBoxLayout(this)),
    mRecordSessionWidget(new RecordSessionWidget(this, description)),
    mInfoLabel(new QLabel("Will never be ready... Derive from this class!"))
{
  this->setObjectName("RecordBaseWidget");
  this->setWindowTitle("Record Base");

  connect(this, SIGNAL(ready(bool)), mRecordSessionWidget, SLOT(setEnabled(bool)));
  connect(mRecordSessionWidget, SIGNAL(newSession(QString)), this, SLOT(postProcessingSlot(QString)));
  connect(mRecordSessionWidget, SIGNAL(started()), this, SLOT(startedSlot()));
  connect(mRecordSessionWidget, SIGNAL(stopped()), this, SLOT(stoppedSlot()));

  mLayout->addWidget(mInfoLabel);
  mLayout->addWidget(mRecordSessionWidget);
}

RecordBaseWidget::~RecordBaseWidget()
{}

void RecordBaseWidget::setWhatsMissingInfo(QString info)
{
  mInfoLabel->setText(info);
}
//----------------------------------------------------------------------------------------------------------------------

TrackedRecordWidget::TrackedRecordWidget(QWidget* parent, QString description) :
  RecordBaseWidget(parent, description)
{}

TrackedRecordWidget::~TrackedRecordWidget()
{}

void TrackedRecordWidget::setTool(ToolPtr tool)
{
  if(mTool && tool && (mTool->getUid() == tool->getUid()))
    return;

  mTool = tool;
  emit toolChanged();
}

ToolPtr TrackedRecordWidget::getTool()
{
  return mTool;
}

//----------------------------------------------------------------------------------------------------------------------
TrackedCenterlineWidget::TrackedCenterlineWidget(QWidget* parent) :
    TrackedRecordWidget(parent, "Tracked centerline")
{
  this->setObjectName("TrackedCenterlineWidget");
  this->setWindowTitle("Tracked Centerline");

  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(checkIfReadySlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(checkIfReadySlot()));
  mLayout->addStretch();

  this->checkIfReadySlot();
}

TrackedCenterlineWidget::~TrackedCenterlineWidget()
{}

void TrackedCenterlineWidget::checkIfReadySlot()
{
  if(ssc::toolManager()->isTracking())
  {
    RecordBaseWidget::setWhatsMissingInfo("<font color=green>Ready to record!</font>\n");
    emit ready(true);
  }
  else
  {
    RecordBaseWidget::setWhatsMissingInfo("<font color=red>Need to start tracking.</font>\n");
    emit ready(false);
  }
}

void TrackedCenterlineWidget::postProcessingSlot(QString sessionId)
{
  RecordSessionPtr session = stateManager()->getRecordSession(sessionId);

  //get the transforms from the session
  ssc::TimedTransformMap transforms_prMt = this->getRecording(session);
  if(transforms_prMt.empty())
  {
    ssc::messageManager()->sendError("Could not find any tracking data from session "+sessionId+". Aborting volume tracking data generation.");
    return;
  }

  //visualize the tracked data as a mesh
  ssc::loadMeshFromToolTransforms(transforms_prMt);

  //convert the transforms into a binary image
  TrackingDataToVolume converter;
  int padding = 10;
  converter.setInput(transforms_prMt, padding);
  ssc::ImagePtr image_d = converter.getOutput();

  //extract the centerline
  QString savepath = stateManager()->getPatientData()->getActivePatientFolder();
  Segmentation segmentation;
  ssc::ImagePtr centerLineImage_d = segmentation.centerline(image_d, savepath);
}

void TrackedCenterlineWidget::startedSlot()
{
  //show preview of tool path
  ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();
  ssc::ToolManager::ToolMap::iterator toolIt = tools->begin();

  View3D* view = viewManager()->get3DView(0,0);
  ssc::ToolRep3DPtr activeRep3D;
  for(; toolIt != tools->end(); ++toolIt)
  {
    activeRep3D = repManager()->findFirstRep<ssc::ToolRep3D>(view->getReps(), toolIt->second);
    if(!activeRep3D)
      continue;
    activeRep3D->getTracer()->clear();
    activeRep3D->getTracer()->start();
  }
}

void TrackedCenterlineWidget::stoppedSlot()
{
  //hide preview of tool path
  ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();
  ssc::ToolManager::ToolMap::iterator toolIt = tools->begin();

  View3D* view = viewManager()->get3DView(0,0);
  ssc::ToolRep3DPtr activeRep3D;
  for(; toolIt != tools->end(); ++toolIt)
  {
    activeRep3D = repManager()->findFirstRep<ssc::ToolRep3D>(view->getReps(), toolIt->second);
    if(!activeRep3D)
      continue;
    if (activeRep3D->getTracer()->isRunning())
    {
      activeRep3D->getTracer()->stop();
      activeRep3D->getTracer()->clear();
    }
  }
}
ssc::TimedTransformMap TrackedCenterlineWidget::getRecording(RecordSessionPtr session)
{
  ssc::TimedTransformMap retval;

  double startTime = session->getStartTime();
  double stopTime = session->getStopTime();

  ToolPtr tool = this->findTool(startTime, stopTime);
  if(!tool)
  {
    ssc::messageManager()->sendWarning("Found no tool with tracking data from the given session.");
    return retval;
  }
  this->setTool(tool);
  retval = tool->getSessionHistory(startTime, stopTime);

  return retval;
}

ToolPtr TrackedCenterlineWidget::findTool(double startTime, double stopTime)
{
  ToolPtr retval;

  ssc::SessionToolHistoryMap toolTransformMap = ssc::toolManager()->getSessionHistory(startTime, stopTime);
  if(toolTransformMap.size() == 1)
  {
    ssc::messageManager()->sendInfo("Found one tool("+toolTransformMap.begin()->first->getName()+") with relevant data.");
    retval = boost::dynamic_pointer_cast<Tool>(toolTransformMap.begin()->first);
  }
  else if(toolTransformMap.size() > 1)
  {
    ssc::messageManager()->sendWarning("Found more than one tool with relevant data, user needs to choose which one to use for tracked centerline extraction.");
    //TODO make the user select which tool they wanna use!!! Pop-up???
    retval = boost::dynamic_pointer_cast<Tool>(toolTransformMap.begin()->first);
    //TODO
  }else if(toolTransformMap.empty())
  {
    ssc::messageManager()->sendWarning("Could not find any session history for given session.");
  }
  return retval;
}
//----------------------------------------------------------------------------------------------------------------------
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

  mRTRecorder.reset(new ssc::RTSourceRecorder(mRTSource)); // reclaim memory

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
//----------------------------------------------------------------------------------------------------------------------
}//namespace cx
