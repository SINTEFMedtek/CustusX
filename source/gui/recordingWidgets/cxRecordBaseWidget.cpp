#include "cxRecordBaseWidget.h"

#include <QPushButton>
#include <QFont>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <vtkPolyData.h>
#include "sscToolManager.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscTransform3D.h"
#include "sscReconstructer.h"
#include "cxRecordSessionWidget.h"
#include "cxUsReconstructionFileMaker.h"
#include "RTSource/cxRTSourceManager.h"
#include "cxProbe.h"
#include "cxSoundSpeedConversionWidget.h"

namespace cx
{



//----------------------------------------------------------------------------------------------------------------------
RecordBaseWidget::RecordBaseWidget(QWidget* parent, QString description):
    BaseWidget(parent, "RecordBaseWidget", "Record Base"),
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

void TrackedRecordWidget::setTool(ssc::ToolPtr tool)
{
  if(mTool && tool && (mTool->getUid() == tool->getUid()))
    return;

  mTool = tool;
  emit toolChanged();
}

ssc::ToolPtr TrackedRecordWidget::getTool()
{
  return mTool;
}
//----------------------------------------------------------------------------------------------------------------------
}//namespace cx
