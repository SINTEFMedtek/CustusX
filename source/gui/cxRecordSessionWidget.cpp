#include "cxRecordSessionWidget.h"

#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include "sscTime.h"
#include "sscLabeledComboBoxWidget.h"
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
    mStartTime(-1),
    mStopTime(-1)
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

void RecordSessionWidget::startStopSlot(bool checked)
{
  if(checked)
    this->startRecording();
  else
    this->stopRecording();
}

void RecordSessionWidget::startRecording()
{
  mStartTime = ssc::getMilliSecondsSinceEpoch();
  mStartStopButton->setText("Stop");
}

void RecordSessionWidget::stopRecording()
{
  mStopTime = ssc::getMilliSecondsSinceEpoch();
  mStartStopButton->setText("Start");

  RecordSessionPtr session = RecordSessionPtr(new RecordSession(mStartTime, mStopTime, mDescriptionLine->text()));
  stateManager()->addRecordSession(session);

  ToolManager::getInstance()->saveToolsSlot(); //asks all the tools to save their transforms and timestamp
}
}
