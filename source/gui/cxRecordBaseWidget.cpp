#include "cxRecordBaseWidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <vtkPolyData.h>
#include "sscToolManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscMeshHelpers.h"
#include "sscTransform3D.h"
#include "sscToolRep3D.h"
#include "sscToolTracer.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"
#include "cxSegmentation.h"
#include "cxRecordSessionWidget.h"
#include "cxTrackingDataToVolume.h"
#include "cxRepManager.h"
#include "cxViewManager.h"
#include "cxView3D.h"
#include "cxUsReconstructionFileMaker.h"

namespace cx
{
//----------------------------------------------------------------------------------------------------------------------
RecordBaseWidget::RecordBaseWidget(QWidget* parent, QString description):
    QWidget(parent),
    mLayout(new QVBoxLayout(this)),
    mInfoLabel(new QLabel("Will never be ready... Derive from this class!")),
    mRecordSessionWidget(new RecordSessionWidget(this, description))
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
  ssc::SessionToolHistoryMap toolTransformMap = session->getSessionHistory();

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
    TrackedRecordWidget(parent, "US Acquisition")
{
  this->setObjectName("USAcqusitionWidget");
  this->setWindowTitle("US Acquisition");

  mRTSourceDataAdapter = SelectRTSourceStringDataAdapterPtr(new SelectRTSourceStringDataAdapter());

  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(checkIfReadySlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(checkIfReadySlot()));

  RecordBaseWidget::mLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mRTSourceDataAdapter));

  connect(mRTSourceDataAdapter.get(), SIGNAL(rtSourceChanged()), this, SLOT(rtSourceChangedSlot()));

  this->checkIfReadySlot();
  this->rtSourceChangedSlot();
}

USAcqusitionWidget::~USAcqusitionWidget()
{}

void USAcqusitionWidget::checkIfReadySlot()
{
  std::cout << "void USAcqusitionWidget::checkIfReadySlot()" << std::endl;
  if(ssc::toolManager()->isTracking() && mRTSource && mRTSource->isStreaming() && mRTRecorder)
  {
    RecordBaseWidget::setWhatsMissingInfo("<font color=green>Ready to record!</font><br>");
    emit ready(true);
  }
  else
  {
    QString whatsMissing("");
    if(!ssc::toolManager()->isTracking())
      whatsMissing.append("<font color=red>Need to start tracking.</font><br>");
    if(mRTSource)
    {
      if(!mRTSource->isStreaming())
        whatsMissing.append("<font color=red>Need to start streaming.</font><br>");
    }else
    {
      whatsMissing.append("<font color=red>Need to get a stream.</font><br>");
    }
    if(!mRTRecorder)
       whatsMissing.append("<font color=red>Need connect to a recorder.</font><br>");

    RecordBaseWidget::setWhatsMissingInfo(whatsMissing);
    emit ready(false);
  }
}

void USAcqusitionWidget::rtSourceChangedSlot()
{
  if(mRTSource)
  {
    disconnect(mRTSource.get(), SIGNAL(streaming(bool)), this, SLOT(checkIfReadySlot()));
  }

  mRTSource = mRTSourceDataAdapter->getRTSource();
  if(mRTSource)
  {
    ssc::messageManager()->sendDebug("New real time source is "+mRTSource->getName());
    connect(mRTSource.get(), SIGNAL(streaming(bool)), this, SLOT(checkIfReadySlot()));
    mRTRecorder = ssc::RealTimeStreamSourceRecorderPtr(new ssc::RealTimeStreamSourceRecorder(mRTSource));
  }
  this->checkIfReadySlot();
}

void USAcqusitionWidget::postProcessingSlot(QString sessionId)
{
  //get session data
  RecordSessionPtr session = stateManager()->getRecordSession(sessionId);
  ssc::RealTimeStreamSourceRecorder::DataType streamRecordedData = mRTRecorder->getRecording(session->getStartTime(), session->getStopTime());
  ssc::TimedTransformMap trackerRecordedData = TrackedRecordWidget::getRecording(session);
  if(trackerRecordedData.empty())
  {
    ssc::messageManager()->sendError("Could not find any tracking data from session "+sessionId+". Aborting volume tracking data generation.");
    return;
  }

  ToolPtr probe = TrackedRecordWidget::getTool();
  UsReconstructionFileMaker filemaker(trackerRecordedData, streamRecordedData, session, stateManager()->getPatientData()->getActivePatientFolder(), probe);
  filemaker.write();
}

void USAcqusitionWidget::startedSlot()
{
  mRTRecorder->startRecord();
}

void USAcqusitionWidget::stoppedSlot()
{
  mRTRecorder->stopRecord();
}
//----------------------------------------------------------------------------------------------------------------------
}//namespace cx
