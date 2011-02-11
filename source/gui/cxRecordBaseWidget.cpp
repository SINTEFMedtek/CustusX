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
#include "RTSource/cxOpenIGTLinkConnection.h"
#include "cxDataLocations.h"
#include "cxProbe.h"

namespace cx
{

ssc::DoubleDataAdapterPtr DoubleDataAdapterTimeCalibration::New()
{
  return ssc::DoubleDataAdapterPtr(new DoubleDataAdapterTimeCalibration());
}

DoubleDataAdapterTimeCalibration::DoubleDataAdapterTimeCalibration()
{
  connect(stateManager()->getIGTLinkConnection()->getRTSource().get(), SIGNAL(connected(bool)), this, SIGNAL(changed()));
}

double DoubleDataAdapterTimeCalibration::getValue() const
{
  return stateManager()->getIGTLinkConnection()->getRTSource()->getTimestampCalibration();
}

QString DoubleDataAdapterTimeCalibration::getHelp() const
{
  return "Set a time shift to add to input RT source frames. Will NOT be saved.";
}

bool DoubleDataAdapterTimeCalibration::setValue(double val)
{
  stateManager()->getIGTLinkConnection()->getRTSource()->setTimestampCalibration(val);
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
SoundSpeedConverterWidget::SoundSpeedConverterWidget(QWidget* parent) :
    QWidget(parent),
    mFromSoundSpeed(1540.0),
    mApplyButton(new QPushButton("Apply")),
    mSoundSpeedSpinBox(new QDoubleSpinBox()),
    mWaterDegreeSpinBox(new QDoubleSpinBox())
{
  QVBoxLayout* vLayout = new QVBoxLayout(this);

  connect(mApplyButton, SIGNAL(clicked()), this, SLOT(applySoundSpeedCompensationFactorSlot()));

  mWaterDegreeSpinBox->setRange(0.0, 50.0);
  mWaterDegreeSpinBox->setValue(25.0);
  connect(mWaterDegreeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(waterDegreeChangedSlot()));

  mToSoundSpeed = this->getWaterSoundSpeed();

  mSoundSpeedSpinBox->setRange(1000.0, 2000.0); //what's a suitable range?
  mSoundSpeedSpinBox->setValue(mToSoundSpeed);
  connect(mSoundSpeedSpinBox, SIGNAL(valueChanged(double)), this, SLOT(waterSoundSpeedChangedSlot()));

  QHBoxLayout* speedLayout = new QHBoxLayout();
  speedLayout->addWidget(new QLabel("Water sound speed: "));
  speedLayout->addWidget(mSoundSpeedSpinBox);
  speedLayout->addWidget(new QLabel("m/s, or"));
  speedLayout->addWidget(mWaterDegreeSpinBox);
  speedLayout->addWidget(new QLabel("C"+QString::fromUtf8("\302\260"))); //\302\260 is the degree sign

  vLayout->addLayout(speedLayout);
  vLayout->addWidget(mApplyButton);
}

SoundSpeedConverterWidget::~SoundSpeedConverterWidget()
{}

void SoundSpeedConverterWidget::applySoundSpeedCompensationFactorSlot()
{
  if(!mProbe)
  {
    ssc::messageManager()->sendWarning("Don't know which probe to set the sound speed compensation for...");
    return;
  }

  double factor = this->getSoundSpeedCompensationFactor();
  //stateManager()->getIGTLinkConnection()->setSoundSpeedCompensationFactor(factor);

  ProbePtr probe = boost::dynamic_pointer_cast<Probe>(mProbe->getProbe());
  if(probe)
    probe->setSoundSpeedCompensationFactor(factor);
  else
    ssc::messageManager()->sendDebug("Could not cast probe to a cx probe...");
}

double SoundSpeedConverterWidget::getSoundSpeedCompensationFactor()
{
  return mToSoundSpeed/mFromSoundSpeed;
}

double SoundSpeedConverterWidget::getWaterSoundSpeed()
{
  double waterDegree = mWaterDegreeSpinBox->value();
  double retval = 1402.40 + 5.01*waterDegree - 0.055*pow(waterDegree, 2) + 0.00022*pow(waterDegree, 3);

  return retval;
}

void SoundSpeedConverterWidget::setProbe(ToolPtr probe)
{
  mProbe = probe;
}

void SoundSpeedConverterWidget::waterSoundSpeedChangedSlot()
{
  mToSoundSpeed = mSoundSpeedSpinBox->value();

  QFont font = mWaterDegreeSpinBox->font();
  font.setStrikeOut(true);
  mWaterDegreeSpinBox->setFont(font);
}

void SoundSpeedConverterWidget::waterDegreeChangedSlot()
{
  mToSoundSpeed = this->getWaterSoundSpeed();

  if(mToSoundSpeed != mSoundSpeedSpinBox->value())
    mSoundSpeedSpinBox->setValue(mToSoundSpeed);

  QFont font = mWaterDegreeSpinBox->font();
  font.setStrikeOut(false);
  mWaterDegreeSpinBox->setFont(font);
}
}//namespace cx
