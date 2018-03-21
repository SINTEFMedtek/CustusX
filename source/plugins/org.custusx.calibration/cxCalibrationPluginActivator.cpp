/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxCalibrationPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxCalibrationGUIExtenderService.h"
#include "cxRegisteredService.h"

namespace cx
{

CalibrationPluginActivator::CalibrationPluginActivator()
{
}

CalibrationPluginActivator::~CalibrationPluginActivator()
{}

void CalibrationPluginActivator::start(ctkPluginContext* context)
{
	mGUIExtender = RegisteredService::create<CalibrationGUIExtenderService>(context, GUIExtenderService_iid);
}

void CalibrationPluginActivator::stop(ctkPluginContext* context)
{
	mGUIExtender.reset();
	Q_UNUSED(context);
}

} // namespace cx



