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
FastOrientationRegistrationWidget::FastOrientationRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent) :
    RegistrationBaseWidget(regManager, parent, "FastOrientationRegistrationWidget", "Fast Orientation Registration"),
    mSetOrientationButton(new QPushButton("Define Orientation")),
    mPatientOrientationButton(new QPushButton("Patient Orientation")),
    mInvertButton(new QCheckBox("Back face"))
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mInvertButton);
  layout->addWidget(mSetOrientationButton);
  layout->addSpacing(6);
  layout->addWidget(this->createHorizontalLine());
  layout->addWidget(mPatientOrientationButton);
  layout->addStretch();

  mSetOrientationButton->setToolTip(this->defaultWhatsThis());
  mPatientOrientationButton->setToolTip(""
				  "Set orientation to tool orientation, but keep\n"
				  "the position of the already loaded data objects\n"
				  "relative to physical space.");

  connect(mPatientOrientationButton, SIGNAL(clicked()), this, SLOT(setPatientOrientationSlot()));

  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot(const QString&)));

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
      "<p><i>Align the Polaris tool so that the tools tip points towards the patients feet and the markers face the same way as the patients nose. Click the Get Orientation button.</i></p>"
      "<p><b>Tip:</b> If the patient is orientated with the nose down towards the table, try using <i>back face</i>.</p>"
      "</html>";
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
  mManager->doFastRegistration_Orientation(this->get_tMtm());
}

ssc::Transform3D FastOrientationRegistrationWidget::get_tMtm() const
{
	ssc::Transform3D tMtm;

	if (mInvertButton->isChecked())
	{
		tMtm = ssc::createTransformRotateY(M_PI) * ssc::createTransformRotateZ(-M_PI / 2);
	}
	else
	{
		tMtm = ssc::createTransformRotateY(M_PI) * ssc::createTransformRotateZ(M_PI / 2);
	}

	return tMtm;
}

void FastOrientationRegistrationWidget::setPatientOrientationSlot()
{
  mManager->applyPatientOrientation(this->get_tMtm());
}

void FastOrientationRegistrationWidget::enableToolSampleButtonSlot()
{
  mToolToSample = ssc::toolManager()->getDominantTool();
  bool enabled = false;
  enabled = mToolToSample &&
      mToolToSample->getVisible() &&
      (!mToolToSample->hasType(ssc::Tool::TOOL_MANUAL) || DataManager::getInstance()->getDebugMode()); // enable only for non-manual tools. ignore this in debug mode.

  mSetOrientationButton->setEnabled(enabled);
  mPatientOrientationButton->setEnabled(enabled);
}

void FastOrientationRegistrationWidget::dominantToolChangedSlot(const QString& uid)
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
