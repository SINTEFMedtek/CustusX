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
//#include "sscReconstructer.h"
#include "cxRecordSessionWidget.h"
#include "cxUsReconstructionFileMaker.h"
#include "cxVideoConnection.h"
#include "cxProbe.h"
#include "cxSoundSpeedConversionWidget.h"

namespace cx
{

RecordBaseWidget::RecordBaseWidget(AcquisitionDataPtr pluginData, QWidget* parent, QString description):
    BaseWidget(parent, "RecordBaseWidget", "Record Base"),
    mPluginData(pluginData),
    mLayout(new QVBoxLayout(this)),
    mRecordSessionWidget(new RecordSessionWidget(pluginData, this, description))
{
  this->setObjectName("RecordBaseWidget");
  this->setWindowTitle("Record Base");

  connect(mRecordSessionWidget, SIGNAL(newSession(QString)), this, SLOT(postProcessingSlot(QString)));
  connect(mRecordSessionWidget, SIGNAL(started(QString)), this, SLOT(startedSlot(QString)));
  connect(mRecordSessionWidget, SIGNAL(stopped()), this, SLOT(stoppedSlot()));

  mLayout->addWidget(mRecordSessionWidget);
}

RecordBaseWidget::~RecordBaseWidget()
{}

////----------------------------------------------------------------------------------------------------------------------
////----------------------------------------------------------------------------------------------------------------------
////----------------------------------------------------------------------------------------------------------------------

TrackedRecordWidget::TrackedRecordWidget(AcquisitionDataPtr pluginData, QWidget* parent, QString description) :
  RecordBaseWidget(pluginData, parent, description)
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
