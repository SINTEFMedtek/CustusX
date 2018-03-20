/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxToolTipCalibrationWidget.h"

#include <QPushButton>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxTrackingService.h"
#include "cxVector3D.h"
#include "cxDefinitionStrings.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxTool.h"
#include "cxVisServices.h"
#include "cxStringPropertySelectTool.h"
#include "cxSpaceProvider.h"

namespace cx
{

//------------------------------------------------------------------------------
ToolTipCalibrateWidget::ToolTipCalibrateWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "tool_tip_calibrate_widget", "ToolTip Calibrate"),
	mServices(services),
    mCalibrateButton(new QPushButton("Calibrate")),
    mReferencePointLabel(new QLabel("Ref. point:")),
    mTestButton(new QPushButton("Test calibration")),
    mCalibrationLabel(new QLabel("Calibration: \n")),
    mDeltaLabel(new QLabel("Delta:"))
{
  QVBoxLayout* toplayout = new QVBoxLayout(this);

  mTools = StringPropertySelectTool::New(mServices->tracking());
  mTools->setValueName("Reference tool");
  mTools->setHelp("Select a tool with a known reference point");
  mCalibrateToolComboBox = new LabeledComboBoxWidget(this, mTools);
  this->setToolTip("Calibrate tool position part of sMt matrix");

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
  connect(mServices->tracking().get(), &TrackingService::stateChanged, this, &ToolTipCalibrateWidget::onTrackingSystemStateChanged);

  //setting default state
  this->toolSelectedSlot();
}

ToolTipCalibrateWidget::~ToolTipCalibrateWidget()
{}

void ToolTipCalibrateWidget::onTrackingSystemStateChanged()
{
	if (mServices->tracking()->getTool(mTools->getValue()))
		return;
	if (!mServices->tracking()->getReferenceTool())
		return;

	mTools->setValue(mServices->tracking()->getReferenceTool()->getUid());
}

void ToolTipCalibrateWidget::calibrateSlot()
{
  ToolPtr refTool = mTools->getTool();
  //Todo, we only allow the reference point with id 1 to be used to calibrate
  //this could be done more dynamic.
  if(!refTool || !refTool->hasReferencePointWithId(1))
    return;

  ToolPtr tool = mServices->tracking()->getActiveTool();
  CoordinateSystem to = mServices->spaceProvider()->getT(tool);
  Vector3D P_t = mServices->spaceProvider()->getActiveToolTipPoint(to);

  ToolTipCalibrationCalculator calc(mServices->spaceProvider(), tool, refTool, P_t);
  Transform3D calibration = calc.get_calibration_sMt();

  QMessageBox msgBox;
  msgBox.setText("Do you want to overwrite "+tool->getName()+"s calibration file?");
  msgBox.setInformativeText("This cannot be undone.");
  msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Ok);
  int ret = msgBox.exec();

  if(ret == QMessageBox::Ok)
  {
      try
      {
          tool->setCalibration_sMt(calibration);
      }
      catch(std::exception& e)
      {
          QMessageBox msgBox2;
          msgBox2.setText("Unknown error, could not calibrate the tool: "+tool->getName()+".");
          msgBox2.setInformativeText(QString(e.what()));
          msgBox2.setStandardButtons(QMessageBox::Ok);
          msgBox2.setDefaultButton(QMessageBox::Ok);
          int ret2 = msgBox2.exec();
          return;
      }
      mCalibrationLabel->setText("Calibration:\n"+qstring_cast(calibration));
  }
}

void ToolTipCalibrateWidget::testCalibrationSlot()
{
  ToolPtr selectedTool = mTools->getTool();
  if(!selectedTool || !selectedTool->hasReferencePointWithId(1))
    return;

  CoordinateSystem to = mServices->spaceProvider()->getT(mServices->tracking()->getActiveTool());
  Vector3D sampledPoint = mServices->spaceProvider()->getActiveToolTipPoint(to);

  ToolTipCalibrationCalculator calc(mServices->spaceProvider(), mServices->tracking()->getActiveTool(), selectedTool, sampledPoint);
  Vector3D delta_selectedTool = calc.get_delta_ref();

  mDeltaLabel->setText("<b>Delta:</b> "+qstring_cast(delta_selectedTool)+" <br> <b>Length:</b>  "+qstring_cast(delta_selectedTool.length()));

  report("Delta: "+qstring_cast(delta_selectedTool)+" Length:   "+qstring_cast(delta_selectedTool.length()));
}

void ToolTipCalibrateWidget::toolSelectedSlot()
{
  QString text("Ref. point: <UNDEFINED POINT>");
  mCalibrateButton->setEnabled(false);

  if(mTools->getTool())
  {
	ToolPtr tool = mTools->getTool();
    if(tool && tool->hasReferencePointWithId(1))
    {
      text = "Ref. point: "+qstring_cast(tool->getReferencePoints()[1]);
      mCalibrateButton->setEnabled(true);
    }
    else
		reportWarning("Selected tool have no known reference point");
    if(tool)
    {
      mCalibrationLabel->setText("Calibration:\n"+qstring_cast(tool->getCalibration_sMt()));
    }
  }

  mReferencePointLabel->setText(text);
}
  //------------------------------------------------------------------------------



ToolTipCalibrationCalculator::ToolTipCalibrationCalculator(SpaceProviderPtr spaces, ToolPtr tool, ToolPtr ref, Vector3D p_t) :
	mTool(tool), mRef(ref), mP_t(p_t), mSpaces(spaces)
{}

ToolTipCalibrationCalculator::~ToolTipCalibrationCalculator()
{}

Vector3D ToolTipCalibrationCalculator::get_delta_ref()
{
  return get_referencePoint_ref() - get_sampledPoint_ref();
}

Transform3D ToolTipCalibrationCalculator::get_calibration_sMt()
{
  return this->get_sMt_new();
}

Vector3D ToolTipCalibrationCalculator::get_sampledPoint_t()
{
  return mP_t;
}

Vector3D ToolTipCalibrationCalculator::get_sampledPoint_ref()
{
  CoordinateSystem csT = mSpaces->getT(mTool); //from
  CoordinateSystem csRef = mSpaces->getT(mRef); //to

  Transform3D refMt = mSpaces->get_toMfrom(csT, csRef);

  Vector3D P_ref = refMt.coord(mP_t);

  return P_ref;
}

Vector3D ToolTipCalibrationCalculator::get_referencePoint_ref()
{
  return mRef->getReferencePoints()[1];
}

Transform3D ToolTipCalibrationCalculator::get_sMt_new()
{
  Transform3D sMt_old = mTool->getCalibration_sMt();

  CoordinateSystem csT = mSpaces->getT(mTool); //to
  CoordinateSystem csRef = mSpaces->getT(mRef); //from
  Transform3D tMref = mSpaces->get_toMfrom(csRef, csT);

  Vector3D delta_t = tMref.vector(this->get_delta_ref());
  Transform3D T_delta_t = createTransformTranslate(delta_t);

  return sMt_old * T_delta_t;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
}//namespace cx
