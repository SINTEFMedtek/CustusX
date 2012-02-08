// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.


#include "cxLapFrameToolCalibrationWidget.h"

#include <QPushButton>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscToolManager.h"
#include "sscDataManager.h"
#include "sscVector3D.h"
#include "sscDefinitionStrings.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxDataLocations.h"
#include "cxTool.h"
#include "sscToolManager.h"
#include "cxPatientData.h"
#include <cxActiveToolWidget.h>

namespace cx
{

//------------------------------------------------------------------------------
LapFrameToolCalibrationWidget::LapFrameToolCalibrationWidget(QWidget* parent) :
    BaseWidget(parent, "LapFrameToolCalibrationWidget", "LapFrame Calibrate"),
    mCalibrateButton(new QPushButton("Calibrate")),
    mReferencePointLabel(new QLabel("Ref. point:")),
    mTestButton(new QPushButton("Test calibration")),
    mCalibrationLabel(new QLabel("Calibration: \n")),
    mDeltaLabel(new QLabel("Delta:"))
{
  QVBoxLayout* toplayout = new QVBoxLayout(this);

  mTools = SelectToolStringDataAdapter::New();
  mTools->setValueName("Calibration Frame");
  mTools->setHelp("Select Calibration Reference Frame");
  mCalibrateToolComboBox = new ssc::LabeledComboBoxWidget(this, mTools);
  this->setToolTip(this->defaultWhatsThis());

//  toplayout->addWidget(new QLabel("<b>Select a tool with a known reference point:</b>"));
  toplayout->addWidget(mCalibrateToolComboBox);
  toplayout->addWidget(mReferencePointLabel);
  toplayout->addWidget(new ActiveToolWidget(this));
  toplayout->addWidget(mCalibrateButton);
  toplayout->addWidget(mCalibrationLabel);
  toplayout->addWidget(this->createHorizontalLine());
  toplayout->addWidget(mTestButton);
  toplayout->addWidget(mDeltaLabel);
  toplayout->addStretch();

  mReferencePointLabel->setText("<i> Use only with the special Laparascopic <br>"
	                                "calibration frame as reference. </i>");

  connect(mCalibrateButton, SIGNAL(clicked()), this, SLOT(calibrateSlot()));
  connect(mTestButton, SIGNAL(clicked()), this, SLOT(testCalibrationSlot()));

  connect(mTools.get(), SIGNAL(changed()), this, SLOT(toolSelectedSlot()));

  //setting default state
  this->toolSelectedSlot();

  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(trackingStartedSlot()));
}

LapFrameToolCalibrationWidget::~LapFrameToolCalibrationWidget()
{}

QString LapFrameToolCalibrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Laparascopic tool calibration</h3>"
      "<p><i>Calibrates a tool by sampling it when it is inserted into the slot of the custom-made calibration frame.</i></br>"
      "<p><i>The calibration frame position is set to a fixed value by CustusX, thus you are responsible for using "
      "the correct frame.</i></br>"
      "<p>By using the test button you can test your calibration by pointing at a known reference point.</br></p>"
      "</html>";
}

void LapFrameToolCalibrationWidget::calibrateSlot()
{
  ssc::ToolPtr refTool = mTools->getTool();
  //Todo, we only allow the reference point with id 1 to be used to calibrate
  //this could be done more dynamic.
  if(!refTool || !refTool->hasReferencePointWithId(1))
    return;

  ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();

  LapFrameToolCalibrationCalculator calc(tool, refTool);
  ssc::Transform3D calibration = calc.get_calibration_sMt();

  QMessageBox msgBox;
  msgBox.setText("Do you want to overwrite "+tool->getName()+"s calibration file?");
  msgBox.setInformativeText("This cannot be undone.");
  msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Ok);
  int ret = msgBox.exec();

  if(ret == QMessageBox::Ok)
  {
    tool->setCalibration_sMt(calibration);
    mCalibrationLabel->setText("Calibration:\n"+qstring_cast(calibration));
  }
}

void LapFrameToolCalibrationWidget::testCalibrationSlot()
{
  ssc::ToolPtr selectedTool = mTools->getTool();
  if(!selectedTool || !selectedTool->hasReferencePointWithId(1))
    return;

  LapFrameToolCalibrationCalculator calc(ssc::toolManager()->getDominantTool(), selectedTool);
  ssc::Vector3D delta_selectedTool = calc.get_delta_ref();

  QString delta = QString("%1 mm").arg(delta_selectedTool.length(), 6, 'g', 1);
  mDeltaLabel->setText("<b>Delta:</b> "+qstring_cast(delta_selectedTool)+" <br> <b>Accuracy:</b>  " + delta);

  ssc::messageManager()->sendInfo("Delta: "+qstring_cast(delta_selectedTool)+" Length:   "+ delta);


}

void LapFrameToolCalibrationWidget::toolSelectedSlot()
{
	//  QString text("Ref. point: <UNDEFINED POINT>");
	mCalibrateButton->setEnabled(false);
	mTestButton->setEnabled(false);

	if (mTools->getTool())
	{
		mCalibrationLabel->setText("Calibration:\n" + qstring_cast(mTools->getTool()->getCalibration_sMt()));
		mCalibrateButton->setEnabled(true);
		mTestButton->setEnabled(true);
	}

	//  mReferencePointLabel->setText(text);
}
//------------------------------------------------------------------------------

void LapFrameToolCalibrationWidget::trackingStartedSlot()
{
	ssc::ToolPtr ref = ssc::toolManager()->getReferenceTool();
	if (ref)
		mTools->setValueName(ref->getUid());
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------




LapFrameToolCalibrationCalculator::LapFrameToolCalibrationCalculator(ssc::ToolPtr tool, ssc::ToolPtr calRef) :
    mTool(tool), mCalibrationRef(calRef)
{
	m_sMpr = mTool->getCalibration_sMt() * mTool->get_prMt().inv();

	m_qMcr = ssc::Transform3D::fromString(" 0.0,  0.0, -1.0, -71.5,"
		                                  " 0.0, -1.0,  0.0,  -8.0,"
	                                      "-1.0,  0.0,  0.0,  -8.8,"
	                                      " 0.0,  0.0,  0.0,   1.0");

	m_qMpr = m_qMcr * mCalibrationRef->get_prMt().inv();
}

ssc::Vector3D LapFrameToolCalibrationCalculator::get_delta_ref()
{
	ssc::Vector3D p(0,0,0);
	ssc::Transform3D qMpr = m_qMcr * mCalibrationRef->get_prMt().inv();

	ssc::Vector3D calibPoint_pr =  qMpr.inv().coord(p);
	ssc::Vector3D toolPoint_pr =  mTool->get_prMt().coord(p);
	return calibPoint_pr - toolPoint_pr;
}

ssc::Transform3D LapFrameToolCalibrationCalculator::get_calibration_sMt()
{
	return m_sMpr * m_qMpr.inv();
}


}
