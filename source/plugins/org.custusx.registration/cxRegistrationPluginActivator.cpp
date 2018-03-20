/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxRegistrationImplService.h"
#include "cxRegisteredService.h"

namespace cx
{

RegistrationPluginActivator::RegistrationPluginActivator()
{
}

RegistrationPluginActivator::~RegistrationPluginActivator()
{
}

void RegistrationPluginActivator::start(ctkPluginContext* context)
{
	mRegistration = RegisteredService::create<RegistrationImplService>(context, RegistrationService_iid);
}

void RegistrationPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx



