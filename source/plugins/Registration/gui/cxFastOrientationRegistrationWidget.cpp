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
    mInvertButton(new QCheckBox("Back face"))
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mInvertButton);
  layout->addWidget(mSetOrientationButton);
  layout->addStretch();

  mSetOrientationButton->setToolTip(this->defaultWhatsThis());

  connect(dataManager(), SIGNAL(debugModeChanged(bool)), this, SLOT(enableToolSampleButtonSlot()));

  mDominantToolProxy =  DominantToolProxy::New(trackingService());
  connect(mDominantToolProxy.get(), SIGNAL(toolVisible(bool)), this, SLOT(enableToolSampleButtonSlot()));
  connect(mDominantToolProxy.get(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(enableToolSampleButtonSlot()));
  this->enableToolSampleButtonSlot();

}

FastOrientationRegistrationWidget::~FastOrientationRegistrationWidget()
{}

QString FastOrientationRegistrationWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3>Fast orientation registration.</h3>"
			"<p><b>Prerequisite:</b> Correctly oriented DICOM axes.</p>"
			"<p>Fast and approximate method for orienting the data to the patient.</p>"
			"<p><i>"
			"Align the Polaris tool so that the tools tip points towards the patients feet and the "
			"markers face the same way as the patients nose. Click the Get Orientation button."
			"</i></p>"
			"<p>"
			"<b>Tip:</b> If the patient is orientated with the nose down towards the table, try using <i>back face</i>."
			"</p>"
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

Transform3D FastOrientationRegistrationWidget::get_tMtm() const
{
	Transform3D tMtm;

	if (mInvertButton->isChecked())
	{
		tMtm = createTransformRotateY(M_PI) * createTransformRotateZ(-M_PI / 2);
	}
	else
	{
		tMtm = createTransformRotateY(M_PI) * createTransformRotateZ(M_PI / 2);
	}

	return tMtm;
}

void FastOrientationRegistrationWidget::enableToolSampleButtonSlot()
{
  ToolPtr tool = toolManager()->getDominantTool();
  bool enabled = false;
  enabled = tool &&
	  tool->getVisible() &&
	  (!tool->hasType(Tool::TOOL_MANUAL) || dataManager()->getDebugMode()); // enable only for non-manual tools. ignore this in debug mode.

  mSetOrientationButton->setEnabled(enabled);
}

}//namespace cx
