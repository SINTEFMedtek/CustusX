#include "cxFastOrientationRegistrationWidget.h"

#include <cmath>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include "sscMessageManager.h"
#include "sscToolManager.h"
#include "cxDataManager.h"
#include "cxRegistrationManager.h"

namespace cx
{
FastOrientationRegistrationWidget::FastOrientationRegistrationWidget(QWidget* parent) :
    QWidget(parent),
    mSetOrientationButton(new QPushButton("Get Orientation")),
    mInvertButton(new QCheckBox("Back face"))
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mInvertButton);
  layout->addWidget(mSetOrientationButton);
  layout->addStretch();

  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(dominantToolChangedSlot(const std::string&)));

  ssc::ToolPtr dominantTool = ssc::toolManager()->getDominantTool();
  if(dominantTool)
    this->dominantToolChangedSlot(dominantTool->getUid());
}

FastOrientationRegistrationWidget::~FastOrientationRegistrationWidget()
{}

void FastOrientationRegistrationWidget::showEvent(QShowEvent* event)
{
  connect(mSetOrientationButton, SIGNAL(clicked()), this, SLOT(setOrientationSlot()));
}

void FastOrientationRegistrationWidget::hideEvent(QHideEvent* event)
{
  disconnect(mSetOrientationButton, SIGNAL(clicked()), this, SLOT(setOrientationSlot()));
}

void FastOrientationRegistrationWidget::setOrientationSlot()
{
  ssc::Transform3D tMtm;

  if (mInvertButton->isChecked())
  {
    tMtm = ssc::createTransformRotateY(M_PI) * ssc::createTransformRotateZ(M_PI_2); //?
  }
  else
  {
    tMtm = ssc::createTransformRotateY(M_PI) * ssc::createTransformRotateZ(M_PI_2); //?
  }

  registrationManager()->doFastRegistration_Orientation(tMtm);
}

void FastOrientationRegistrationWidget::enableToolSampleButtonSlot()
{
  mToolToSample = ssc::toolManager()->getDominantTool();
  bool enabled = false;
  enabled = mToolToSample &&
      mToolToSample->getVisible() &&
      (mToolToSample->getType()!=ssc::Tool::TOOL_MANUAL || DataManager::getInstance()->getDebugMode()); // enable only for non-manual tools. ignore this in debug mode.
  mSetOrientationButton->setEnabled(enabled);
}

void FastOrientationRegistrationWidget::dominantToolChangedSlot(const std::string& uid)
{
  if(mToolToSample && mToolToSample->getUid() == uid)
    return;

  ssc::ToolPtr dominantTool = ssc::toolManager()->getDominantTool();

  if(mToolToSample)
    disconnect(mToolToSample.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));

  mToolToSample = dominantTool;

  if(mToolToSample)
    connect(mToolToSample.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));

  //update button
  //mSetOrientationButton->setText("Sample ("+qstring_cast(mToolToSample->getName())+")");
  connect(DataManager::getInstance(), SIGNAL(debugModeChanged(bool)), this, SLOT(enableToolSampleButtonSlot()));
  this->enableToolSampleButtonSlot();
}

void FastOrientationRegistrationWidget::toolVisibleSlot(bool visible)
{
  this->enableToolSampleButtonSlot();
}

}//namespace cx
