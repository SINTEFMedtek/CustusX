/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxToolManualCalibrationWidget.h"

#include <QLabel>
#include <QGroupBox>
#include "cxActiveToolWidget.h"
#include "cxTrackingService.h"
#include "cxHelperWidgets.h"
#include "cxVisServices.h"
#include "cxStringPropertySelectTool.h"

namespace cx
{

ToolManualCalibrationWidget::ToolManualCalibrationWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "tool_manual_calibration_widget", "Tool Manual Calibrate"),
	mServices(services)
{
  //layout
  QVBoxLayout* mToptopLayout = new QVBoxLayout(this);
  //toptopLayout->setMargin(0);

  this->setToolTip("Edit tool calibration matrix sMt");
  mTool = StringPropertySelectTool::New(mServices->tracking());
  mToptopLayout->addWidget(sscCreateDataWidget(this, mTool));

  mToptopLayout->addWidget(new QLabel("<font color=red>Caution: sMt is changed directly by this control.</font>"));
  mGroup = new QGroupBox(this);
  mGroup->setTitle("Calibration matrix sMt");
  mToptopLayout->addWidget(mGroup);
  QVBoxLayout* groupLayout = new QVBoxLayout;
  mGroup->setLayout(groupLayout);
  groupLayout->setMargin(0);
  mMatrixWidget = new Transform3DWidget(mGroup);
  groupLayout->addWidget(mMatrixWidget);
  connect(mMatrixWidget, SIGNAL(changed()), this, SLOT(matrixWidgetChanged()));
  connect(mTool.get(), SIGNAL(changed()), this, SLOT(toolCalibrationChanged()));

  this->toolCalibrationChanged();
  mMatrixWidget->setEditable(true);

  mToptopLayout->addStretch();

  connect(mServices->tracking().get(), &TrackingService::stateChanged, this, &ToolManualCalibrationWidget::toolCalibrationChanged);
}

void ToolManualCalibrationWidget::toolCalibrationChanged()
{
	ToolPtr tool = mTool->getTool();
  if (!tool)
    return;

  mMatrixWidget->blockSignals(true);
  mMatrixWidget->setMatrix(tool->getCalibration_sMt());
  mMatrixWidget->blockSignals(false);
}

void ToolManualCalibrationWidget::matrixWidgetChanged()
{
	ToolPtr tool = mTool->getTool();
	if (!tool)
      return;

  Transform3D M = mMatrixWidget->getMatrix();
  tool->setCalibration_sMt(M);
}



}
