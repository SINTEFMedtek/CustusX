/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTrackingShapePluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxTrackingShapeGUIExtenderService.h"
#include "cxRegisteredService.h"

namespace cx
{

TrackingShapePluginActivator::TrackingShapePluginActivator()
{
	std::cout << "Created TrackingShapePluginActivator" << std::endl;
}

TrackingShapePluginActivator::~TrackingShapePluginActivator()
{}

void TrackingShapePluginActivator::start(ctkPluginContext* context)
{
	mRegistration = RegisteredService::create<TrackingShapeGUIExtenderService>(context, GUIExtenderService_iid);
}

void TrackingShapePluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx



