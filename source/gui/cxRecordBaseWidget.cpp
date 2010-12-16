#include "cxRecordBaseWidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include "sscToolManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscMessageManager.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"
#include "cxSegmentation.h"
#include "cxRecordSessionWidget.h"
#include "cxTrackingDataToVolume.h"

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
TrackedCenterlineWidget::TrackedCenterlineWidget(QWidget* parent) :
    RecordBaseWidget(parent, "Tracked centerline")
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
    RecordBaseWidget::setWhatsMissingInfo("<font color=green>Ready to record!</font>");
    emit ready(true);
  }
  else
  {
    RecordBaseWidget::setWhatsMissingInfo("<font color=red>Need to start tracking.</font>");
    emit ready(false);
  }
}

void TrackedCenterlineWidget::postProcessingSlot(QString sessionId)
{
  RecordSessionPtr session = stateManager()->getRecordSession(sessionId);

  //get the transforms from the session
  ssc::TimedTransformMap transforms_prMt = this->getSessionTrackingData(session);
  if(transforms_prMt.empty())
  {
    ssc::messageManager()->sendError("Could not find any tracking data from session "+sessionId+". Aborting centerline extraction.");
    return;
  }

  //convert the transforms into a binary image
  TrackingDataToVolume converter;
  converter.setInput(transforms_prMt);
  ssc::ImagePtr image_d = converter.getOutput();

  //extract the centerline
  QString savepath = stateManager()->getPatientData()->getActivePatientFolder();
  Segmentation segmentation;
  ssc::ImagePtr centerLineImage_d = segmentation.centerline(image_d, savepath);
}

ssc::TimedTransformMap TrackedCenterlineWidget::getSessionTrackingData(RecordSessionPtr session)
{
  ssc::TimedTransformMap retval;
  ssc::SessionToolHistoryMap toolTransformMap = session->getSessionHistory();

  if(toolTransformMap.size() == 1)
  {
    ssc::messageManager()->sendInfo("Found one tool("+toolTransformMap.begin()->first->getName()+") with relevant data.");
    retval = toolTransformMap.begin()->second;
  }
  else if(toolTransformMap.size() > 1)
  {
    ssc::messageManager()->sendWarning("Found more than one tool with relevant data, user needs to choose which one to use for tracked centerline extraction.");
    retval = toolTransformMap.begin()->second; //TODO make the user select which tool they wanna use.
  }else if(toolTransformMap.empty())
  {
    ssc::messageManager()->sendWarning("Could not find any session history for given session.");
  }
  return retval;
}
//----------------------------------------------------------------------------------------------------------------------
USAcqusitionWidget::USAcqusitionWidget(QWidget* parent) :
    RecordBaseWidget(parent, "US Acquisition")
{
  mRTSourceDataAdapter = SelectRTSourceStringDataAdapterPtr(new SelectRTSourceStringDataAdapter());

  this->setObjectName("USAcqusitionWidget");
  this->setWindowTitle("US Acquisition");

  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(checkIfReadySlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(checkIfReadySlot()));

  RecordBaseWidget::mLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mRTSourceDataAdapter));

  connect(mRTSourceDataAdapter.get(), SIGNAL(rtSourceChanged()), this, SLOT(rtSourceChangedSlot()));

  this->checkIfReadySlot();
}

USAcqusitionWidget::~USAcqusitionWidget()
{}

void USAcqusitionWidget::checkIfReadySlot()
{
  ssc::messageManager()->sendDebug("TODO: implement USAcqusitionWidget::checkIfReadySlot()");
  if(ssc::toolManager()->isTracking() /*&& mRTSource && mRTSource->isStreaming()*/) //TODO
  {
    RecordBaseWidget::setWhatsMissingInfo("<font color=green>Ready to record!</font>");
    emit ready(true);
  }
  else
  {
    QString whatsMissing("");
    if(!ssc::toolManager()->isTracking())
      whatsMissing.append("<font color=red>Need to start tracking.</font> ");
    if(mRTSource /*&& !mRTSource->isStreaming()*/) //TODO
      whatsMissing.append("<font color=red>Need to start streaming.</font> ");

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
  connect(mRTSource.get(), SIGNAL(streaming(bool)), this, SLOT(checkIfReadySlot()));
}

void USAcqusitionWidget::postProcessingSlot(QString sessionId)
{
  //TODO
  //generate the files needed for reconstruction
}
//----------------------------------------------------------------------------------------------------------------------
}//namespace cx
