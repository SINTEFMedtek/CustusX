#include <cxPatientOrientationWidget.h>

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
PatientOrientationWidget::PatientOrientationWidget(RegistrationManagerPtr regManager, QWidget* parent) :
    RegistrationBaseWidget(regManager, parent, "PatientOrientationWidget", "Patient Orientation"),
    mPatientOrientationButton(new QPushButton("Patient Orientation")),
    mInvertButton(new QCheckBox("Back face"))
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mInvertButton);
  layout->addWidget(mPatientOrientationButton);
  layout->addStretch();

//  QString helpText("<html>"
//        "<h3>Set patient orientation using the navigation tool.</h3>"
//        "<p>"
//		"<p>"
//		"Only orientation of the reference space is changed - data is not moved."
//		"</p>"
//        "Align the Polaris tool so that the tools tip points towards the patients feet and the "
//        "markers face the same way as the patients nose. Click the button. Verify that the"
//        "figure in the upper left corner of the 3D view is correcly aligned."
//        "</p>"
//        "<p>"
//        "<b>Tip:</b> If the patient is orientated with the nose down towards the table, try using <i>back face</i>."
//        "</p>"
//        "</html>");

  mPatientOrientationButton->setToolTip(defaultWhatsThis());
  connect(mPatientOrientationButton, SIGNAL(clicked()), this, SLOT(setPatientOrientationSlot()));


//  mPatientOrientationFrontFaceAction = new QAction(QIcon(":/icons/woman_A_2.png"), "Patient Orientation - Front Face", this);
//  mPatientOrientationFrontFaceAction->setStatusTip(helpText);
//  connect(mPatientOrientationFrontFaceAction, SIGNAL(triggered()), this, SLOT(patientOrientationFrontFaceSlot()));
//  mPatientOrientationFrontFaceButton = new QToolButton();
//  mPatientOrientationFrontFaceButton->setDefaultAction(mPatientOrientationFrontFaceAction);
//  layout->addWidget(mPatientOrientationFrontFaceButton);
//
//  mPatientOrientationBackFaceAction = new QAction(QIcon(":/icons/woman_front_1.png"), "Patient Orientation - Front Face", this);
//  mPatientOrientationBackFaceAction->setStatusTip(helpText);
//  connect(mPatientOrientationBackFaceAction, SIGNAL(triggered()), this, SLOT(patientOrientationBackFaceSlot()));
//  mPatientOrientationBackFaceButton = new QToolButton();
//  mPatientOrientationBackFaceButton->setDefaultAction(mPatientOrientationBackFaceAction);
//  layout->addWidget(mPatientOrientationBackFaceButton);

  connect(cxDataManager::getInstance(), SIGNAL(debugModeChanged(bool)), this, SLOT(enableToolSampleButtonSlot()));

  mDominantToolProxy =  DominantToolProxy::New();
  connect(mDominantToolProxy.get(), SIGNAL(toolVisible(bool)), this, SLOT(enableToolSampleButtonSlot()));
  connect(mDominantToolProxy.get(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(enableToolSampleButtonSlot()));
  this->enableToolSampleButtonSlot();

}

PatientOrientationWidget::~PatientOrientationWidget()
{}

QString PatientOrientationWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3>Set patient orientation using the navigation tool.</h3>"
			"<p><b>Prerequisite:</b> Image registration.</p>"
			"<p>Only orientation of the reference space is changed - data is not moved.</p>"
			"Align the Polaris tool so that the tools tip points towards the patients feet and the "
			"markers face the same way as the patients nose. Click the button. Verify that the "
			"figure in the upper left corner of the 3D view is correcly aligned.</p>"
			"<p><b>Tip:</b> If the patient is orientated with the nose down towards the table, try using <i>back face</i>.</p>"
			"</html>";
}

//void PatientOrientationWidget::patientOrientationFrontFaceSlot()
//{
//	Transform3D tMtm;
//	tMtm = createTransformRotateY(M_PI) * createTransformRotateZ(M_PI / 2);
//	mManager->applyPatientOrientation(tMtm);
//}
//void PatientOrientationWidget::patientOrientationBackFaceSlot()
//{
//	Transform3D tMtm;
//	tMtm = createTransformRotateY(M_PI) * createTransformRotateZ(-M_PI / 2);
//	mManager->applyPatientOrientation(tMtm);
//}

Transform3D PatientOrientationWidget::get_tMtm() const
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

void PatientOrientationWidget::setPatientOrientationSlot()
{
  mManager->applyPatientOrientation(this->get_tMtm());
}

void PatientOrientationWidget::enableToolSampleButtonSlot()
{
  ToolPtr tool = toolManager()->getDominantTool();
  bool enabled = false;
  enabled = tool &&
	  tool->getVisible() &&
      (!tool->hasType(Tool::TOOL_MANUAL) || cxDataManager::getInstance()->getDebugMode()); // enable only for non-manual tools. ignore this in debug mode.

  mPatientOrientationButton->setEnabled(enabled);
//  mPatientOrientationFrontFaceAction->setEnabled(enabled);
//  mPatientOrientationBackFaceAction->setEnabled(enabled);
}

}//namespace cx
