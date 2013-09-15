/*
 * cxToolManualCalibrationWidget.cpp
 *
 *  \date May 4, 2011
 *      \author christiana
 */

#include <cxToolManualCalibrationWidget.h>
#include "cxActiveToolWidget.h"
#include "sscToolManager.h"

namespace cx
{

ToolManualCalibrationWidget::ToolManualCalibrationWidget(QWidget* parent) :
    BaseWidget(parent, "ToolManualCalibrationWidget", "Tool Manual Calibrate")
{
  //layout
  QVBoxLayout* mToptopLayout = new QVBoxLayout(this);
  //toptopLayout->setMargin(0);

  mToptopLayout->addWidget(new ActiveToolWidget(this));

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
  connect(toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(toolCalibrationChanged()));

  this->toolCalibrationChanged();
  mMatrixWidget->setEditable(true);

  mToptopLayout->addStretch();

  connect(toolManager(), SIGNAL(configured()), this, SLOT(toolCalibrationChanged()));
  connect(toolManager(), SIGNAL(initialized()), this, SLOT(toolCalibrationChanged()));
  connect(toolManager(), SIGNAL(trackingStarted()), this, SLOT(toolCalibrationChanged()));
  connect(toolManager(), SIGNAL(trackingStopped()), this, SLOT(toolCalibrationChanged()));
}


QString ToolManualCalibrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Tool Manual Calibration.</h3>"
      "<p><i>Manipulate the tool calibration matrix sMt directly, using the matrix manipulation interface.</i></br>"
      "</html>";
}


void ToolManualCalibrationWidget::toolCalibrationChanged()
{
  ToolPtr tool = toolManager()->getDominantTool();
  if (!tool)
    return;

//  mManualGroup->setVisible(tool->getVisible());
  mMatrixWidget->blockSignals(true);
  mMatrixWidget->setMatrix(tool->getCalibration_sMt());
  mMatrixWidget->blockSignals(false);
}

void ToolManualCalibrationWidget::matrixWidgetChanged()
{
  ToolPtr tool = toolManager()->getDominantTool();
    if (!tool)
      return;

  Transform3D M = mMatrixWidget->getMatrix();
  tool->setCalibration_sMt(M);
}



}
