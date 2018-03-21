/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationGUIPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxRegistrationGUIExtenderService.h"
#include "cxRegistrationHistoryGUIExtenderService.h"
#include "cxRegisteredService.h"
#include "cxRegServices.h"

namespace cx
{

RegistrationGUIPluginActivator::RegistrationGUIPluginActivator()
{
}

RegistrationGUIPluginActivator::~RegistrationGUIPluginActivator()
{
}

void RegistrationGUIPluginActivator::start(ctkPluginContext* context)
{
	mRegistration = RegisteredService::create<RegistrationGUIExtenderService>(context, GUIExtenderService_iid);

	RegServicesPtr services = RegServices::create(context);
	RegistrationHistoryGUIExtenderService *history = new RegistrationHistoryGUIExtenderService(services);
	mRegistrationHistory = RegisteredServicePtr(new RegisteredService(context, history, GUIExtenderService_iid));
}

void RegistrationGUIPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	mRegistrationHistory.reset();
	Q_UNUSED(context);
}

} // namespace cx


