/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxLapFrameToolCalibrationWidget.h"

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
#include "cxTrackingService.h"
#include <cxActiveToolWidget.h>
#include "cxDoubleWidgets.h"
#include "cxVisServices.h"
#include "cxStringPropertySelectTool.h"

namespace cx
{

//------------------------------------------------------------------------------
LapFrameToolCalibrationWidget::LapFrameToolCalibrationWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "lap_frame_tool_calibration_widget", "LapFrame Calibrate"),
	mServices(services),
    mCalibrateButton(new QPushButton("Calibrate")),
    mReferencePointLabel(new QLabel("Ref. point:")),
    mTestButton(new QPushButton("Test calibration")),
    mCalibrationLabel(new QLabel("Calibration: \n")),
    mDeltaLabel(new QLabel("Delta:"))
{
  QVBoxLayout* toplayout = new QVBoxLayout(this);

  mCameraAngleAdapter = DoubleProperty::initialize("Camera Angle", "",
      "Additional tilt of calibration around tool y-axis,\nfor use with cameras tilted relative to tool direction",
      0.0, DoubleRange(-M_PI/2, M_PI/2, M_PI/180), 0);
  mCameraAngleAdapter->setInternal2Display(180.0/M_PI);

  mCalibRefTool = StringPropertySelectTool::New(mServices->tracking());
  mCalibRefTool->setValueName("Calibration Frame");
  mCalibRefTool->setHelp("Select Calibration Reference Frame");

  mCalibratingTool = StringPropertySelectTool::New(mServices->tracking());
  mCalibratingTool->setValueName("Tool");
  mCalibratingTool->setHelp("Select which Tool to calibrate");

  this->setToolTip("Calibrate tool matrix using a custom frame");

//  toplayout->addWidget(new QLabel("<b>Select a tool with a known reference point:</b>"));
  toplayout->addWidget(new LabeledComboBoxWidget(this, mCalibRefTool));
  toplayout->addWidget(mReferencePointLabel);
  toplayout->addWidget(new LabeledComboBoxWidget(this, mCalibratingTool));
//  toplayout->addWidget(new ActiveToolWidget(this));
  toplayout->addWidget(new SpinBoxAndSliderGroupWidget(this, mCameraAngleAdapter));
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

  connect(mCalibRefTool.get(), SIGNAL(changed()), this, SLOT(toolSelectedSlot()));

  //setting default state
  this->toolSelectedSlot();

  connect(mServices->tracking().get(), &cx::TrackingService::stateChanged, this, &LapFrameToolCalibrationWidget::trackingStartedSlot);
}

LapFrameToolCalibrationWidget::~LapFrameToolCalibrationWidget()
{}

void LapFrameToolCalibrationWidget::calibrateSlot()
{
  ToolPtr refTool = mCalibRefTool->getTool();
  ToolPtr tool = mCalibratingTool->getTool();
  double cameraAngle = mCameraAngleAdapter->getValue();
  if(!refTool || !tool)
  {
    reportError(QString("Calibration prerequisited not met: calref:%1, tool:%2").arg(refTool!=0).arg(tool!=0) );
    return;
  }
  if(!refTool->getVisible() || !tool->getVisible() || !refTool->hasReferencePointWithId(1))
  {
    reportError(QString("Calibration prerequisited not met: calref vis:%1, tool vis :%2, refpoint:%3").arg(refTool->getVisible()).arg(tool->getVisible()).arg(refTool->hasReferencePointWithId(1)) );
    return;
  }

  LapFrameToolCalibrationCalculator calc(tool, refTool, cameraAngle);
  Transform3D calibration = calc.get_calibration_sMt();

  QMessageBox msgBox;
  msgBox.setText("Do you want to overwrite "+tool->getName()+"'s calibration file?");
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
    mCalibrationLabel->setText(QString("Calibration matrix for %1:\n%2").arg(tool->getName(), qstring_cast(calibration)));
  }
}

void LapFrameToolCalibrationWidget::testCalibrationSlot()
{
  ToolPtr refTool = mCalibRefTool->getTool();
  ToolPtr tool = mCalibratingTool->getTool();
  double cameraAngle = mCameraAngleAdapter->getValue();

  if(!refTool || !tool || !refTool->hasReferencePointWithId(1))
    return;

  LapFrameToolCalibrationCalculator calc(tool, refTool, cameraAngle);
  Vector3D delta_selectedTool = calc.get_delta_ref();

  QString delta = QString("%1 mm").arg(delta_selectedTool.length(), 6, 'g', 1);
  mDeltaLabel->setText("<b>Delta "+tool->getName()+":</b> "+qstring_cast(delta_selectedTool)+" <br> <b>Accuracy:</b>  " + delta);

  report("Delta "+tool->getName()+": "+qstring_cast(delta_selectedTool)+" Length:   "+ delta);


}

void LapFrameToolCalibrationWidget::toolSelectedSlot()
{
	//  QString text("Ref. point: <UNDEFINED POINT>");
	mCalibrateButton->setEnabled(false);
	mTestButton->setEnabled(false);

	if (mCalibRefTool->getTool())
	{
//		mCalibrationLabel->setText("Calibration:\n" + qstring_cast(mCalibratingTool->getTool()->getCalibration_sMt()));
//    Transform3D calibration = mCalibratingTool->getTool()->getCalibration_sMt();
//    mCalibrationLabel->setText(QString("Calibration matrix for %1:\n%2").arg(tool->getName(), qstring_cast(calibration)));
		mCalibrateButton->setEnabled(true);
		mTestButton->setEnabled(true);
	}

	//  mReferencePointLabel->setText(text);
}
//------------------------------------------------------------------------------

void LapFrameToolCalibrationWidget::trackingStartedSlot()
{
	ToolPtr ref = mServices->tracking()->getTool("calibration_tool");
	if (ref)
		mCalibRefTool->setValue(ref->getUid());
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------




LapFrameToolCalibrationCalculator::LapFrameToolCalibrationCalculator(ToolPtr tool, ToolPtr calRef, double cameraAngle) :
    mTool(tool), mCalibrationRef(calRef), mCameraAngle(cameraAngle)
{
	m_sMpr = mTool->getCalibration_sMt() * mTool->get_prMt().inv();

//	m_qMcr = Transform3D::fromString(" 0.0,  0.0, -1.0, -71.5,"
//		                                  " 0.0, -1.0,  0.0,  -8.0,"
//	                                      "-1.0,  0.0,  0.0,  -8.8,"
//	                                      " 0.0,  0.0,  0.0,   1.0");

	m_qMcr = Transform3D::Identity();
	m_qMpr = m_qMcr * mCalibrationRef->get_prMt().inv();
}

Vector3D LapFrameToolCalibrationCalculator::get_delta_ref()
{
	Vector3D p(0,0,0);
	Transform3D qMpr = m_qMcr * mCalibrationRef->get_prMt().inv();

	Vector3D calibPoint_pr =  qMpr.inv().coord(p);
	Vector3D toolPoint_pr =  mTool->get_prMt().coord(p);
	return calibPoint_pr - toolPoint_pr;
}

Transform3D LapFrameToolCalibrationCalculator::get_calibration_sMt()
{
	Transform3D calibration = m_sMpr * m_qMpr.inv() * createTransformRotateY(mCameraAngle);
	this->useOnlyRotationalPart(&calibration);
	return calibration;
}

/**
* @brief LapFrameToolCalibrationCalculator::useOnlyRotationalPart
* @param transform
*
* This function washes a matrix so that only the rotational component
* is kept in the linear (upper left 3x3) part of the matrix.
* The use for it here is particulary because of possible errors in the calibration
* since we read calibration files which might not have the needed precission and so on.
* The noise might show up as shear or scale operations which we don't want.
*/
void LapFrameToolCalibrationCalculator::useOnlyRotationalPart(Transform3D* transform)
{
	Transform3D::LinearMatrixType rotationalPart = transform->rotation();
	transform->linear() = rotationalPart;
}

}
