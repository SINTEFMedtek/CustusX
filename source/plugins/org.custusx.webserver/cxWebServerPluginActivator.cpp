/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxWebServerPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxRegisteredService.h"
#include "cxScreenVideoProvider.h"

#include "cxPatientModelService.h"
#include "cxLogger.h"

#include "cxWebServerGUIExtenderService.h"

namespace cx
{

NetworkPluginActivator::NetworkPluginActivator()
{
    std::cout << "Created NetworkPluginActivator" << std::endl;
}

NetworkPluginActivator::~NetworkPluginActivator()
{}

void NetworkPluginActivator::start(ctkPluginContext* context)
{
	mRegistration = RegisteredService::create<WebServerGUIExtenderService>(context, new WebServerGUIExtenderService(context), GUIExtenderService_iid);
}

void NetworkPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx
