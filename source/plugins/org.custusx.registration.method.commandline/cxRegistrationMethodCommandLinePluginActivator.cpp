/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationMethodCommandLinePluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxRegistrationMethodCommandLineService.h"
#include "cxRegisteredService.h"

namespace cx
{

RegistrationMethodCommandLinePluginActivator::RegistrationMethodCommandLinePluginActivator()
{
}

RegistrationMethodCommandLinePluginActivator::~RegistrationMethodCommandLinePluginActivator()
{}

void RegistrationMethodCommandLinePluginActivator::start(ctkPluginContext* context)
{
	RegServicesPtr services = RegServices::create(context);
	RegistrationMethodCommandLineService* service = new RegistrationMethodCommandLineService(services);
	mRegistrationCommandLine = RegisteredService::create<RegistrationMethodCommandLineService>(context, service, RegistrationMethodService_iid);
}

void RegistrationMethodCommandLinePluginActivator::stop(ctkPluginContext* context)
{
	mRegistrationCommandLine.reset();
	Q_UNUSED(context);
}

} // namespace cx
