#include "cxRecordSessionWidget.h"

#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include "sscTime.h"
#include "sscMessageManager.h"
#include "cxAudio.h"
#include "cxRecordSession.h"
#include "cxToolManager.h"
#include "cxStateMachineManager.h"
#include "cxDataInterface.h"

namespace cx
{
RecordSessionWidget::RecordSessionWidget(QWidget* parent, QString defaultDescription) :
    QWidget(parent),
    mStartStopButton(new QPushButton(QIcon(":/icons/open_icon_library/png/64x64/actions/media-record-3.png"), "Start")),
    mCancelButton(new QPushButton(QIcon(":/icons/open_icon_library/png/64x64/actions/process-stop-7.png"), "Cancel")),
    mDescriptionLine(new QLineEdit(defaultDescription)),
    mStartTimeMSec(-1),
    mStopTimeMSec(-1),
    mPostProcessing(false)
{
  this->setObjectName("RecordSessionWidget");
  this->setWindowTitle("Record Tracking");

  QVBoxLayout* layout = new QVBoxLayout(this);
  mDescriptionLabel = new QLabel("Description:");
  layout->addWidget(mDescriptionLabel);
  layout->addWidget(mDescriptionLine);
  layout->addWidget(mStartStopButton);
  layout->addWidget(mCancelButton);

  mStartStopButton->setCheckable(true);
  connect(mStartStopButton, SIGNAL(clicked(bool)), this, SLOT(startStopSlot(bool)));
  connect(mCancelButton, SIGNAL(clicked(bool)), this, SLOT(cancelSlot()));
  mCancelButton->setEnabled(false);
}

void RecordSessionWidget::setDescriptionVisibility(bool value)
{
    mDescriptionLine->setVisible(value);
    mDescriptionLabel->setVisible(value);
}

RecordSessionWidget::~RecordSessionWidget()
{}

void RecordSessionWidget::setDescription(QString text)
{
  mDescriptionLine->setText(text);
}

void RecordSessionWidget::changeEvent(QEvent* event)
{
  QWidget::changeEvent(event);
  if(event->type() != QEvent::EnabledChange)
    return;

  if(!this->isEnabled())
    this->stopRecording();
}

void RecordSessionWidget::startPostProcessing(QString description)
{
  mPostProcessing = true;

  mCancelButton->setEnabled(true);
  mStartStopButton->setText(description);
  mStartStopButton->setEnabled(false);
}

void RecordSessionWidget::stopPostProcessing()
{
  this->reset();
}


void RecordSessionWidget::startStopSlot(bool checked)
{
  if(checked)
    this->startRecording();
  else
    this->stopRecording();
}

void RecordSessionWidget::cancelSlot()
{
  if(!this->isRecording() && !mPostProcessing)
    return;

  this->reset();
  Audio::playCancelSound();
  emit stopped();
}

void RecordSessionWidget::startRecording()
{
  if(this->isRecording())
  {
    ssc::messageManager()->sendInfo("Already recording a session, stop before trying to start a new record session.");
    return;
  }

  mStartTimeMSec = ssc::getMilliSecondsSinceEpoch();
  mStartStopButton->setText("Stop");
  mStartStopButton->setIcon(QIcon(":/icons/open_icon_library/png/64x64/actions/media-playback-stop.png"));
  mCancelButton->setEnabled(true);

  Audio::playStartSound();
  emit started();
}

void RecordSessionWidget::stopRecording()
{
  if(!this->isRecording())
    return;

  mStopTimeMSec = ssc::getMilliSecondsSinceEpoch();

  RecordSessionPtr session = RecordSessionPtr(new RecordSession(mStartTimeMSec, mStopTimeMSec, mDescriptionLine->text()));
  stateManager()->addRecordSession(session);

  ToolManager::getInstance()->saveToolsSlot(); //asks all the tools to save their transforms and timestamps

  this->reset();
  Audio::playStopSound();
  emit stopped();

  emit newSession(session->getUid());
}

bool RecordSessionWidget::isRecording()
{
  return (mStartTimeMSec > -1);
}

void RecordSessionWidget::reset()
{
  mPostProcessing = false;

  mStartStopButton->blockSignals(true);
  mStartStopButton->setChecked(false);
  mStartStopButton->blockSignals(false);
  mStartStopButton->setText("Start");
  mStartStopButton->setIcon(QIcon(":/icons/open_icon_library/png/64x64/actions/media-record-3.png"));
  mStartStopButton->setEnabled(true);

  mCancelButton->setEnabled(false);

  mStartTimeMSec = -1;
  mStopTimeMSec = -1;
}
}
