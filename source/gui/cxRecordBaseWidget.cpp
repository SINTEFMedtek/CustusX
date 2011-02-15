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

DoubleDataAdapterTimeCalibration::DoubleDataAdapterTimeCalibration()
{
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChanged()));
}

void DoubleDataAdapterTimeCalibration::dominantToolChanged()
{
  // ignore tool changes to something non-probeish.
  // This gives the user a chance to use the widget without having to show the probe.
  ssc::ToolPtr newTool = boost::shared_dynamic_cast<Tool>(ssc::toolManager()->getDominantTool());
  if (!newTool || newTool->getProbeSector().mType==ssc::ProbeData::tNONE)
    return;

  if (mTool)
    disconnect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

  mTool = boost::shared_dynamic_cast<Tool>(ssc::toolManager()->getDominantTool());

  if (mTool)
    connect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

  emit changed();
}

ssc::DoubleDataAdapterPtr DoubleDataAdapterTimeCalibration::New()
{
  return ssc::DoubleDataAdapterPtr(new DoubleDataAdapterTimeCalibration());
}

double DoubleDataAdapterTimeCalibration::getValue() const
{
  if (!mTool)
    return 0;
  return mTool->getProbe()->getData().mTemporalCalibration;
}

QString DoubleDataAdapterTimeCalibration::getHelp() const
{
  return "Set a temporal shift to add to input probe frames. Will NOT be saved.";
}

bool DoubleDataAdapterTimeCalibration::setValue(double val)
{
  if (!mTool)
    return 0;
  mTool->getProbe()->setTemporalCalibration(val);
  return true;
}

ssc::DoubleRange DoubleDataAdapterTimeCalibration::getValueRange() const
{
  return ssc::DoubleRange(-1000,1000,1);
}

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

void TrackedRecordWidget::setTool(ToolPtr tool)
{
  if(mTool && tool && (mTool->getUid() == tool->getUid()))
    return;

  mTool = tool;
  emit toolChanged();
}

ToolPtr TrackedRecordWidget::getTool()
{
  return mTool;
}
//----------------------------------------------------------------------------------------------------------------------
}//namespace cx
