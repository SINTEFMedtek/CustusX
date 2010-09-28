#include "cxFastOrientationRegistrationWidget.h"

#include <QPushButton>
#include <QVBoxLayout>
#include "sscMessageManager.h"
#include "sscToolManager.h"
#include "cxDataManager.h"
#include "cxRegistrationManager.h"

namespace cx
{
FastOrientationRegistrationWidget::FastOrientationRegistrationWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mSetOrientationButton(new QPushButton("Get Orientation"))
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mSetOrientationButton);

  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(dominantToolChangedSlot(const std::string&)));

  ssc::ToolPtr dominantTool = ssc::toolManager()->getDominantTool();
  if(dominantTool)
    this->dominantToolChangedSlot(dominantTool->getUid());
}

FastOrientationRegistrationWidget::~FastOrientationRegistrationWidget()
{}

QString FastOrientationRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Fast orientation registration.</h3>"
      "<p>Fast and approximate method for determining the patient registrations orientation.</p>"
      "<p><i>Align the Polaris tool so that the tools tip points towards the patients feet and the markers face the same way as the patients nose. Click the Get Orientation button.</i></p></html>";
}

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
  registrationManager()->doFastRegistration_Orientation();
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
