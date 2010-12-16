#include "cxRecordSessionWidget.h"

#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include "sscTime.h"
//#include "sscLabeledComboBoxWidget.h"
#include "sscMessageManager.h"
#include "cxRecordSession.h"
#include "cxToolManager.h"
#include "cxStateMachineManager.h"
#include "cxDataInterface.h"

namespace cx
{
RecordSessionWidget::RecordSessionWidget(QWidget* parent, QString defaultDescription) :
    QWidget(parent),
    mStartStopButton(new QPushButton("Start")),
    mDescriptionLine(new QLineEdit(defaultDescription)),
    mStartTimeMSec(-1),
    mStopTimeMSec(-1)
{
  this->setObjectName("RecordSessionWidget");
  this->setWindowTitle("Record Tracking");

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(new QLabel("Desciption:"));
  layout->addWidget(mDescriptionLine);
  layout->addWidget(mStartStopButton);
  //layout->addWidget(new ssc::LabeledComboBoxWidget(this, SelectRecordSessionStringDataAdapterPtr(new SelectRecordSessionStringDataAdapter())));

  mStartStopButton->setCheckable(true);
  connect(mStartStopButton, SIGNAL(clicked(bool)), this, SLOT(startStopSlot(bool)));
}

RecordSessionWidget::~RecordSessionWidget()
{}

void RecordSessionWidget::changeEvent(QEvent* event)
{
  QWidget::changeEvent(event);
  if(event->type() != QEvent::EnabledChange)
    return;

  if(!this->isEnabled())
    this->stopRecording();
}

void RecordSessionWidget::startStopSlot(bool checked)
{
  if(checked)
    this->startRecording();
  else
    this->stopRecording();
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
}

void RecordSessionWidget::stopRecording()
{
  if(!this->isRecording())
    return;

  mStopTimeMSec = ssc::getMilliSecondsSinceEpoch();
  mStartStopButton->setText("Start");

  RecordSessionPtr session = RecordSessionPtr(new RecordSession(mStartTimeMSec, mStopTimeMSec, mDescriptionLine->text()));
  stateManager()->addRecordSession(session);

  ToolManager::getInstance()->saveToolsSlot(); //asks all the tools to save their transforms and timestamps

  emit newSession(session->getUid());

  this->reset();
}

bool RecordSessionWidget::isRecording()
{
  return (mStartTimeMSec > -1);
}

void RecordSessionWidget::reset()
{
  mStartTimeMSec = -1;
  mStopTimeMSec = -1;
}
}
