/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxEBUSCalibrationWidget.h"

#include <QPushButton>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxTrackingService.h"
#include "cxVector3D.h"
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
EBUSCalibrationWidget::EBUSCalibrationWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "ebus_calibration_widget", "EBUS Calibrate"),
	mServices(services),
    mCalibrateButton(new QPushButton("Calibrate")),
    mReferencePointLabel(new QLabel("Ref. point:")),
    mCalibrationLabel(new QLabel("Calibration: \n")),
    mDeltaLabel(new QLabel("Delta:"))
{
  QVBoxLayout* toplayout = new QVBoxLayout(this);

  mCalibRefTool = StringPropertySelectTool::New(mServices->tracking());
	mCalibRefTool->setValueName("Calibration Adapter");
	mCalibRefTool->setHelp("Select Calibration Adapter sensor");

  mCalibratingTool = StringPropertySelectTool::New(mServices->tracking());
  mCalibratingTool->setValueName("Tool");
  mCalibratingTool->setHelp("Select which Tool to calibrate");

	//this->setToolTip("Calibrate tool matrix using a custom frame");

  toplayout->addWidget(new LabeledComboBoxWidget(this, mCalibRefTool));
  toplayout->addWidget(mReferencePointLabel);
  toplayout->addWidget(new LabeledComboBoxWidget(this, mCalibratingTool));
  toplayout->addWidget(mCalibrateButton);
  toplayout->addWidget(mCalibrationLabel);
  toplayout->addWidget(this->createHorizontalLine());
	//toplayout->addWidget(mDeltaLabel);
  toplayout->addStretch();

	mReferencePointLabel->setText("<i> Use EBUS adapter for calibration </i>");

  connect(mCalibrateButton, SIGNAL(clicked()), this, SLOT(calibrateSlot()));

  connect(mCalibRefTool.get(), SIGNAL(changed()), this, SLOT(toolSelectedSlot()));

  //setting default state
  this->toolSelectedSlot();

	connect(mServices->tracking().get(), &cx::TrackingService::stateChanged, this, &EBUSCalibrationWidget::trackingStartedSlot);
}

EBUSCalibrationWidget::~EBUSCalibrationWidget()
{}

void EBUSCalibrationWidget::calibrateSlot()
{
  ToolPtr refTool = mCalibRefTool->getTool();
  ToolPtr tool = mCalibratingTool->getTool();
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

	EBUSCalibrationCalculator calc(tool, refTool);
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


void EBUSCalibrationWidget::toolSelectedSlot()
{
	//  QString text("Ref. point: <UNDEFINED POINT>");
	mCalibrateButton->setEnabled(false);

	if (mCalibRefTool->getTool())
	{
//		mCalibrationLabel->setText("Calibration:\n" + qstring_cast(mCalibratingTool->getTool()->getCalibration_sMt()));
//    Transform3D calibration = mCalibratingTool->getTool()->getCalibration_sMt();
//    mCalibrationLabel->setText(QString("Calibration matrix for %1:\n%2").arg(tool->getName(), qstring_cast(calibration)));
		mCalibrateButton->setEnabled(true);
	}

	//  mReferencePointLabel->setText(text);
}
//------------------------------------------------------------------------------

void EBUSCalibrationWidget::trackingStartedSlot()
{
	ToolPtr ref = mServices->tracking()->getTool("calibration_tool");
	if (ref)
		mCalibRefTool->setValue(ref->getUid());
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------




EBUSCalibrationCalculator::EBUSCalibrationCalculator(ToolPtr tool, ToolPtr calRef) :
		mTool(tool), mCalibrationRef(calRef)
{
	m_sMpr = mTool->getCalibration_sMt() * mTool->get_prMt().inv();

//	m_qMcr = Transform3D::fromString(" 0.0,  0.0, -1.0, -71.5,"
//		                                  " 0.0, -1.0,  0.0,  -8.0,"
//	                                      "-1.0,  0.0,  0.0,  -8.8,"
//	                                      " 0.0,  0.0,  0.0,   1.0");

	m_qMcr = Transform3D::Identity();
	m_qMpr = m_qMcr * mCalibrationRef->get_prMt().inv();
}

Vector3D EBUSCalibrationCalculator::get_delta_ref()
{
	Vector3D p(0,0,0);
	Transform3D qMpr = m_qMcr * mCalibrationRef->get_prMt().inv();

	Vector3D calibPoint_pr =  qMpr.inv().coord(p);
	Vector3D toolPoint_pr =  mTool->get_prMt().coord(p);
	return calibPoint_pr - toolPoint_pr;
}

Transform3D EBUSCalibrationCalculator::get_calibration_sMt()
{
	Transform3D calibration = m_sMpr * m_qMpr.inv();
	return calibration;
}

}
