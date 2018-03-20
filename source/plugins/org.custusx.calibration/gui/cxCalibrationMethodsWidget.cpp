/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include <cxCalibrationMethodsWidget.h>

#include <cxToolTipSampleWidget.h>
#include "cxToolTipCalibrationWidget.h"
#include "cxToolManualCalibrationWidget.h"
#include "cxTemporalCalibrationWidget.h"
#include "cxLapFrameToolCalibrationWidget.h"
#include "cxProbeConfigWidget.h"

namespace cx
{

CalibrationMethodsWidget::CalibrationMethodsWidget(VisServicesPtr services, AcquisitionServicePtr acquisitionService, QWidget* parent, QString objectName, QString windowTitle) :
	TabbedWidget(parent, objectName, windowTitle)
{
	this->setToolTip("Collection of calibration methods");
	this->addTab(new ToolTipCalibrateWidget(services, this), "Tool Tip");
	this->addTab(new LapFrameToolCalibrationWidget(services, this), "Lap Frame");
	this->addTab(new ToolTipSampleWidget(services, this), "Sample");
	this->addTab(new TemporalCalibrationWidget(services, acquisitionService, this), "Temporal");
	this->addTab(new ToolManualCalibrationWidget(services, this), "Tool Manual");
	this->addTab(new ProbeConfigWidget(services, this), "Probe");
}

}
