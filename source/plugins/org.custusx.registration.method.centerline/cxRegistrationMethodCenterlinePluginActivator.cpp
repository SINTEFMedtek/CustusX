/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationMethodCenterlinePluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxRegistrationMethodCenterlineService.h"
#include "cxRegisteredService.h"

namespace cx
{

RegistrationMethodCenterlinePluginActivator::RegistrationMethodCenterlinePluginActivator()
{
//	std::cout << "Created RegistrationMethodCenterlinePluginActivator" << std::endl;
}

RegistrationMethodCenterlinePluginActivator::~RegistrationMethodCenterlinePluginActivator()
{}

void RegistrationMethodCenterlinePluginActivator::start(ctkPluginContext* context)
{
	RegServicesPtr services = RegServices::create(context);
    RegistrationMethodCenterlineImageToPatientService* image2patientService = new RegistrationMethodCenterlineImageToPatientService(services);

	mRegistrationImageToPatient = RegisteredServicePtr(new RegisteredService(context, image2patientService, RegistrationMethodService_iid));
}

void RegistrationMethodCenterlinePluginActivator::stop(ctkPluginContext* context)
{
	mRegistrationImageToPatient.reset();
	Q_UNUSED(context);
}

} // namespace cx
