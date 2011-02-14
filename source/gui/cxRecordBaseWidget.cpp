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
}

void DoubleDataAdapterTimeCalibration::dominantToolChanged()
{
  // ignore tool changes to something non-probeish.
  // This gives the user a chance to use the widget without having to show the probe.
  ssc::ToolPtr newTool = boost::shared_dynamic_cast<Tool>(ssc::toolManager()->getDominantTool());
  if (!newTool || newTool->getProbeSector().mType==ssc::ProbeData::tNONE)
    return;

  if (mTool)
    disconnect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

  mTool = boost::shared_dynamic_cast<Tool>(ssc::toolManager()->getDominantTool());

  if (mTool)
    connect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

  emit changed();
}

ssc::DoubleDataAdapterPtr DoubleDataAdapterTimeCalibration::New()
{
  return ssc::DoubleDataAdapterPtr(new DoubleDataAdapterTimeCalibration());
}

double DoubleDataAdapterTimeCalibration::getValue() const
{
  if (!mTool)
    return 0;
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

ssc::TimedTransformMap TrackedRecordWidget::getRecording(RecordSessionPtr session)
{
  ssc::TimedTransformMap retval;
  //ssc::SessionToolHistoryMap toolTransformMap = session->getSessionHistory();
  ssc::SessionToolHistoryMap toolTransformMap = ssc::toolManager()->getSessionHistory(session->getStartTime(), session->getStopTime());;

  if(toolTransformMap.size() == 1)
  {
    ssc::messageManager()->sendInfo("Found one tool("+toolTransformMap.begin()->first->getName()+") with relevant data.");
    mTool = boost::dynamic_pointer_cast<Tool>(toolTransformMap.begin()->first);
    retval = toolTransformMap.begin()->second;
  }
  else if(toolTransformMap.size() > 1)
  {
    ssc::messageManager()->sendWarning("Found more than one tool with relevant data, user needs to choose which one to use for tracked centerline extraction.");
    //TODO make the user select which tool they wanna use!!! Pop-up???
    mTool = boost::dynamic_pointer_cast<Tool>(toolTransformMap.begin()->first);
    retval = toolTransformMap.begin()->second;
    //TODO
  }else if(toolTransformMap.empty())
  {
    ssc::messageManager()->sendWarning("Could not find any session history for given session.");
  }
  return retval;
}

ToolPtr TrackedRecordWidget::getTool()
{
  if(!mTool)
    ssc::messageManager()->sendWarning("No tool has been set for the session yet, try calling getRecording() before getTool() in TrackedRecordWidget.");
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
  ssc::TimedTransformMap transforms_prMt = TrackedRecordWidget::getRecording(session);
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
//----------------------------------------------------------------------------------------------------------------------
USAcqusitionWidget::USAcqusitionWidget(QWidget* parent) :
    TrackedRecordWidget(parent, DataLocations::getSettings()->value("Ultrasound/acquisitionName").toString())
{
  this->setObjectName("USAcqusitionWidget");
  this->setWindowTitle("US Acquisition");

  connect(&mFileMakerFutureWatcher, SIGNAL(finished()), this, SLOT(fileMakerWriteFinished()));

  mRecordSessionWidget->setDescriptionVisibility(false);

//  mRTSourceDataAdapter = SelectRTSourceStringDataAdapterPtr(new SelectRTSourceStringDataAdapter());

  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(checkIfReadySlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(checkIfReadySlot()));

  RecordBaseWidget::mLayout->addWidget(new ssc::LabeledComboBoxWidget(this, ActiveToolConfigurationStringDataAdapter::New()));

//  mUSSectorConfigBox = new ssc::LabeledComboBoxWidget(this, ActiveToolConfigurationStringDataAdapter::New());
//  mToptopLayout->addWidget(mUSSectorConfigBox);

//  RecordBaseWidget::mLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mRTSourceDataAdapter));


  SoundSpeedConverterWidget* soundSpeedWidget = new SoundSpeedConverterWidget(this);
  mLayout->addWidget(soundSpeedWidget);
  mLayout->addStretch();
  RecordBaseWidget::mLayout->addWidget(new ssc::SpinBoxGroupWidget(this, DoubleDataAdapterTimeCalibration::New()));

//  connect(mRTSourceDataAdapter.get(), SIGNAL(changed()), this, SLOT(rtSourceChangedSlot()));

  this->checkIfReadySlot();
  this->rtSourceChangedSlot();
}

USAcqusitionWidget::~USAcqusitionWidget()
{}

void USAcqusitionWidget::checkIfReadySlot()
{
  bool tracking = ssc::toolManager()->isTracking();
  bool streaming = mRTSource && mRTSource->isStreaming();

  //std::cout << "void USAcqusitionWidget::checkIfReadySlot()" << std::endl;
  if(tracking && streaming && mRTRecorder)
  {
    RecordBaseWidget::setWhatsMissingInfo("<font color=green>Ready to record!</font><br>");
//    emit ready(true);
  }
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
//    emit ready(false);
  }

  // do not require tracking to be present in order to perform an acquisition.
  emit ready(streaming && mRTRecorder);
}

void USAcqusitionWidget::rtSourceChangedSlot()
{
  if(mRTSource)
  {
    disconnect(mRTSource.get(), SIGNAL(streaming(bool)), this, SLOT(checkIfReadySlot()));
  }

//  mRTSource = mRTSourceDataAdapter->getRTSource();
  mRTSource = stateManager()->getIGTLinkConnection()->getRTSource();

  if(mRTSource)
  {
    //ssc::messageManager()->sendDebug("New real time source is "+mRTSource->getName());
    connect(mRTSource.get(), SIGNAL(streaming(bool)), this, SLOT(checkIfReadySlot()));
    mRTRecorder.reset(new ssc::RealTimeStreamSourceRecorder(mRTSource));
  }
  this->checkIfReadySlot();
}

void USAcqusitionWidget::postProcessingSlot(QString sessionId)
{
//  std::cout << "post processing" << std::endl;

  //get session data
  RecordSessionPtr session = stateManager()->getRecordSession(sessionId);
  ssc::RealTimeStreamSourceRecorder::DataType streamRecordedData = mRTRecorder->getRecording(session->getStartTime(), session->getStopTime());
  ssc::TimedTransformMap trackerRecordedData = TrackedRecordWidget::getRecording(session);
  if(trackerRecordedData.empty())
  {
    ssc::messageManager()->sendError("Could not find any tracking data from session "+sessionId+". Volume data only will be written.");
//    return;
  }

//  std::cout << "pre save" << std::endl;
  ToolPtr probe = TrackedRecordWidget::getTool();
  mFileMaker.reset(new UsReconstructionFileMaker(trackerRecordedData, streamRecordedData, session->getDescription(), stateManager()->getPatientData()->getActivePatientFolder(), probe));

  mFileMakerFuture = QtConcurrent::run(boost::bind(&UsReconstructionFileMaker::write, mFileMaker));
  mFileMakerFutureWatcher.setFuture(mFileMakerFuture);
//  std::cout << "save started" << std::endl;
//  QString targetFolder = filemaker.write();
}

void USAcqusitionWidget::fileMakerWriteFinished()
{
  QString targetFolder = mFileMakerFutureWatcher.future().result();

//  std::cout << "select data" << std::endl;
  stateManager()->getReconstructer()->selectData(mFileMaker->getMhdFilename(targetFolder));
//  std::cout << "selected data" << std::endl;

  mRTRecorder.reset(new ssc::RealTimeStreamSourceRecorder(mRTSource));

  if (DataLocations::getSettings()->value("Automation/autoReconstruct").toBool())
  {
//    std::cout << "start threaded reconstruct" << std::endl;
    mThreadedReconstructer.reset(new ssc::ThreadedReconstructer(stateManager()->getReconstructer()));
    connect(mThreadedReconstructer.get(), SIGNAL(finished()), this, SLOT(reconstructFinishedSlot()));
    mThreadedReconstructer->start();
    mRecordSessionWidget->startPostProcessing("Reconstructing");
//    std::cout << "started threaded reconstruct" << std::endl;

//    stateManager()->getReconstructer()->reconstruct();
  }
}

void USAcqusitionWidget::reconstructFinishedSlot()
{
//  std::cout << "finished threaded reconstruct" << std::endl;
  mRecordSessionWidget->stopPostProcessing();
  mThreadedReconstructer.reset();

  // show data in view from here if applicable.
}

void USAcqusitionWidget::startedSlot()
{
  mRecordSessionWidget->setDescription(DataLocations::getSettings()->value("Ultrasound/acquisitionName").toString());

  mRTRecorder->startRecord();

  //sleep(120);
}

//void USAcqusitionWidget::cancelledSlot()
//{
//}

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
