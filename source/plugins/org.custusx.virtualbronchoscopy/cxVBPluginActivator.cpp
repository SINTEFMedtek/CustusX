/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxVBPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxVBService.h"
#include "cxRegisteredService.h"
#include "cxVisServices.h"

namespace cx
{

VBPluginActivator::VBPluginActivator()
{
}

VBPluginActivator::~VBPluginActivator()
{
}

void VBPluginActivator::start(ctkPluginContext* context)
{
	VisServicesPtr services = VisServices::create(context);
	mVBregistration = RegisteredServicePtr(new RegisteredService(context, new VBGUIExtenderService(services), GUIExtenderService_iid));
}

void VBPluginActivator::stop(ctkPluginContext* context)
{
	mVBregistration.reset();
    Q_UNUSED(context);
}

} // namespace cx
