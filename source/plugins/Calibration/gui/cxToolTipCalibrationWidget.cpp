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

#include "cxToolTipCalibrationWidget.h"

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
//#include "cxStateService.h"
#include "cxPatientData.h"

namespace cx
{

//------------------------------------------------------------------------------
ToolTipCalibrateWidget::ToolTipCalibrateWidget(QWidget* parent) :
    BaseWidget(parent, "ToolTipCalibrateWidget", "ToolTip Calibrate"),
    mCalibrateButton(new QPushButton("Calibrate")),
    mReferencePointLabel(new QLabel("Ref. point:")),
    mTestButton(new QPushButton("Test calibration")),
    mCalibrationLabel(new QLabel("Calibration: \n")),
    mDeltaLabel(new QLabel("Delta:"))
{
  QVBoxLayout* toplayout = new QVBoxLayout(this);

  mTools = SelectToolStringDataAdapter::New();
  mTools->setValueName("Reference tool");
  mTools->setHelp("Select a tool with a known reference point");
  mCalibrateToolComboBox = new ssc::LabeledComboBoxWidget(this, mTools);
  this->setToolTip(this->defaultWhatsThis());

  //toplayout->addWidget(new QLabel("<b>Select a tool with a known reference point:</b>"));
  toplayout->addWidget(mCalibrateToolComboBox);
  toplayout->addWidget(mReferencePointLabel);
  toplayout->addWidget(mCalibrateButton);
  toplayout->addWidget(mCalibrationLabel);
  toplayout->addWidget(this->createHorizontalLine());
  toplayout->addWidget(mTestButton);
  toplayout->addWidget(mDeltaLabel);
  toplayout->addStretch();

  connect(mCalibrateButton, SIGNAL(clicked()), this, SLOT(calibrateSlot()));
  connect(mTestButton, SIGNAL(clicked()), this, SLOT(testCalibrationSlot()));

  connect(mTools.get(), SIGNAL(changed()), this, SLOT(toolSelectedSlot()));

  //setting default state
  this->toolSelectedSlot();
}

ToolTipCalibrateWidget::~ToolTipCalibrateWidget()
{}

QString ToolTipCalibrateWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Tool tip calibration.</h3>"
      "<p><i>Calibrates a tool by sampling it when pointing at a known point on another frame.</i></br>"
      "By using the test button you can test your calibration by pointing at a known reference point.</br></p>"
      "</html>";
}

void ToolTipCalibrateWidget::calibrateSlot()
{
  ssc::ToolPtr refTool = mTools->getTool();
  //Todo, we only allow the reference point with id 1 to be used to calibrate
  //this could be done more dynamic.
  if(!refTool || !refTool->hasReferencePointWithId(1))
    return;

  ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();
  ssc::CoordinateSystem to = ssc::CoordinateSystemHelpers::getT(tool);
  ssc::Vector3D P_t = ssc::CoordinateSystemHelpers::getDominantToolTipPoint(to);

  ToolTipCalibrationCalculator calc(tool, refTool, P_t);
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

void ToolTipCalibrateWidget::testCalibrationSlot()
{
  ssc::ToolPtr selectedTool = mTools->getTool();
  if(!selectedTool || !selectedTool->hasReferencePointWithId(1))
    return;

  ssc::CoordinateSystem to = ssc::CoordinateSystemHelpers::getT(ssc::toolManager()->getDominantTool());
  ssc::Vector3D sampledPoint = ssc::CoordinateSystemHelpers::getDominantToolTipPoint(to);

  ToolTipCalibrationCalculator calc(ssc::toolManager()->getDominantTool(), selectedTool, sampledPoint);
  ssc::Vector3D delta_selectedTool = calc.get_delta_ref();

  mDeltaLabel->setText("<b>Delta:</b> "+qstring_cast(delta_selectedTool)+" <br> <b>Length:</b>  "+qstring_cast(delta_selectedTool.length()));

  ssc::messageManager()->sendInfo("Delta: "+qstring_cast(delta_selectedTool)+" Length:   "+qstring_cast(delta_selectedTool.length()));
}

void ToolTipCalibrateWidget::toolSelectedSlot()
{
  QString text("Ref. point: <UNDEFINED POINT>");
  mCalibrateButton->setEnabled(false);
//  mTestButton->setEnabled(false);

  if(mTools->getTool())
  {
    ToolPtr tool = boost::dynamic_pointer_cast<Tool>(mTools->getTool());
    if(tool && tool->hasReferencePointWithId(1))
    {
      text = "Ref. point: "+qstring_cast(tool->getReferencePoints()[1]);
      mCalibrateButton->setEnabled(true);
//      mTestButton->setEnabled(true);
    }
    else
    	ssc::messageManager()->sendWarning("Selected tool have no known reference point");
    if(tool)
    {
      mCalibrationLabel->setText("Calibration:\n"+qstring_cast(tool->getCalibration_sMt()));
    }
  }

  mReferencePointLabel->setText(text);
}
  //------------------------------------------------------------------------------



ToolTipCalibrationCalculator::ToolTipCalibrationCalculator(ssc::ToolPtr tool, ssc::ToolPtr ref, ssc::Vector3D p_t) :
    mTool(tool), mRef(ref), mP_t(p_t)
{}

ToolTipCalibrationCalculator::~ToolTipCalibrationCalculator()
{}

ssc::Vector3D ToolTipCalibrationCalculator::get_delta_ref()
{
  return get_referencePoint_ref() - get_sampledPoint_ref();
}

ssc::Transform3D ToolTipCalibrationCalculator::get_calibration_sMt()
{
  return this->get_sMt_new();
}

ssc::Vector3D ToolTipCalibrationCalculator::get_sampledPoint_t()
{
  return mP_t;
}

ssc::Vector3D ToolTipCalibrationCalculator::get_sampledPoint_ref()
{
  ssc::CoordinateSystem csT = ssc::CoordinateSystemHelpers::getT(mTool); //from
  ssc::CoordinateSystem csRef = ssc::CoordinateSystemHelpers::getT(mRef); //to

  ssc::Transform3D refMt = ssc::CoordinateSystemHelpers::get_toMfrom(csT, csRef);

  ssc::Vector3D P_ref = refMt.coord(mP_t);

  return P_ref;
}

ssc::Vector3D ToolTipCalibrationCalculator::get_referencePoint_ref()
{
  return mRef->getReferencePoints()[1];
}

ssc::Transform3D ToolTipCalibrationCalculator::get_sMt_new()
{
  ssc::Transform3D sMt_old = mTool->getCalibration_sMt();

  ssc::CoordinateSystem csT = ssc::CoordinateSystemHelpers::getT(mTool); //to
  ssc::CoordinateSystem csRef = ssc::CoordinateSystemHelpers::getT(mRef); //from
  ssc::Transform3D tMref = ssc::CoordinateSystemHelpers::get_toMfrom(csRef, csT);

  ssc::Vector3D delta_t = tMref.vector(this->get_delta_ref());
  ssc::Transform3D T_delta_t = ssc::createTransformTranslate(delta_t);

  return sMt_old * T_delta_t;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
}//namespace cx
