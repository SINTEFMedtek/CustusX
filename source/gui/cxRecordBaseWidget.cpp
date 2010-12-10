#include "cxRecordBaseWidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include "sscToolManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxRecordSessionWidget.h"

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
  connect(mRecordSessionWidget, SIGNAL(newSession(QString)), this, SLOT(postProcessingSlot()));

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

  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(checkIfReadySlot()));
  connect(ssc::toolManager(), SIGNAL(initialized()), this, SLOT(checkIfReadySlot()));
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
    RecordBaseWidget::setWhatsMissingInfo("<font color=red>Need to start tracking.</font>");
  emit ready(false);
}

void TrackedCenterlineWidget::postProcessingSlot()
{
  //TODO
  //extract the centerline
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
  if(ssc::toolManager()->isTracking() /*&& mRTSource && mRTSource->isStreaming()*/)
  {
    RecordBaseWidget::setWhatsMissingInfo("<font color=green>Ready to record!</font>");
    emit ready(true);
  }
  else
  {
    QString whatsMissing("");
    if(!ssc::toolManager()->isTracking())
      whatsMissing.append("<font color=red>Need to start tracking.</font> ");
    if(mRTSource /*&& !mRTSource->isStreaming()*/)
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

void USAcqusitionWidget::postProcessingSlot()
{
  //TODO
  //generate the files needed for reconstruction
}
//----------------------------------------------------------------------------------------------------------------------
}//namespace cx
