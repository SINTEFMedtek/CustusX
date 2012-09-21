#include "cxRecordSessionWidget.h"

#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QEvent>
#include "sscTime.h"
#include "sscMessageManager.h"
#include "cxRecordSession.h"
#include "cxToolManager.h"
#include "sscTypeConversions.h"

namespace cx
{
RecordSessionWidget::RecordSessionWidget(AcquisitionDataPtr pluginData, QWidget* parent, QString defaultDescription) :
    BaseWidget(parent, "RecordSessionWidget", "Record Session"),
    mPluginData(pluginData),
    mInfoLabel(new QLabel("")),
    mStartStopButton(new QPushButton(QIcon(":/icons/open_icon_library/png/64x64/actions/media-record-3.png"), "Start")),
    mCancelButton(new QPushButton(QIcon(":/icons/open_icon_library/png/64x64/actions/process-stop-7.png"), "Cancel")),
    mDescriptionLine(new QLineEdit(defaultDescription)),
    mStartTimeMSec(-1),
    mStopTimeMSec(-1),
    mPostProcessing(false)
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);
  mDescriptionLabel = new QLabel("Description:");
  layout->addWidget(mInfoLabel);
  layout->addWidget(mDescriptionLabel);
  layout->addWidget(mDescriptionLine);
  layout->addWidget(mStartStopButton);
  layout->addWidget(mCancelButton);

  mStartStopButton->setCheckable(true);
  connect(mStartStopButton, SIGNAL(clicked(bool)), this, SLOT(startStopSlot(bool)));
  connect(mCancelButton, SIGNAL(clicked(bool)), this, SLOT(cancelSlot()));
  mCancelButton->setEnabled(false);
}

QString RecordSessionWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Record session.</h3>"
    "<p>Lets you record a session of some kind.</p>"
    "<p><i></i></p>"
    "</html>";
}

void RecordSessionWidget::setReady(bool val, QString text)
{
  this->setEnabled(val);
  mInfoLabel->setText(text);
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
  ssc::messageManager()->playCancelSound();
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

  ssc::messageManager()->playStartSound();
  emit started();
}

void RecordSessionWidget::stopRecording()
{
  if(!this->isRecording())
    return;

  mStopTimeMSec = ssc::getMilliSecondsSinceEpoch();

  RecordSessionPtr session = RecordSessionPtr(new RecordSession(mPluginData->getNewUid(), mStartTimeMSec, mStopTimeMSec, mDescriptionLine->text()));
  mPluginData->addRecordSession(session);

  ToolManager::getInstance()->saveToolsSlot(); //asks all the tools to save their transforms and timestamps

  this->reset();
  ssc::messageManager()->playStopSound();
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
