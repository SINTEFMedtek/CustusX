/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxViewPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxViewImplService.h"
#include "cxRegisteredService.h"

namespace cx
{

VisualizationPluginActivator::VisualizationPluginActivator()
{
}

VisualizationPluginActivator::~VisualizationPluginActivator()
{
}

void VisualizationPluginActivator::start(ctkPluginContext* context)
{
	mRegistration = RegisteredService::create<ViewImplService>(context, ViewService_iid);
}

void VisualizationPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx
