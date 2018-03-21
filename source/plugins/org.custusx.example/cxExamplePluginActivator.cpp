/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxExamplePluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxExampleGUIExtenderService.h"
#include "cxRegisteredService.h"

namespace cx
{

ExamplePluginActivator::ExamplePluginActivator()
{
	std::cout << "Created ExamplePluginActivator" << std::endl;
}

ExamplePluginActivator::~ExamplePluginActivator()
{}

void ExamplePluginActivator::start(ctkPluginContext* context)
{
	mRegistration = RegisteredService::create<ExampleGUIExtenderService>(context, GUIExtenderService_iid);
}

void ExamplePluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx



