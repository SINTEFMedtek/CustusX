/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationMethodPlatePluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxRegistrationMethodPlateService.h"
#include "cxRegisteredService.h"

namespace cx
{

RegistrationMethodPlatePluginActivator::RegistrationMethodPlatePluginActivator()
{
//	std::cout << "Created RegistrationMethodPlatePluginActivator" << std::endl;
}

RegistrationMethodPlatePluginActivator::~RegistrationMethodPlatePluginActivator()
{}

void RegistrationMethodPlatePluginActivator::start(ctkPluginContext* context)
{
	RegServicesPtr services = RegServices::create(context);
	RegistrationMethodPlateImageToPatientService* image2patientService = new RegistrationMethodPlateImageToPatientService(services);

	mRegistrationImageToPatient = RegisteredServicePtr(new RegisteredService(context, image2patientService, RegistrationMethodService_iid));
}

void RegistrationMethodPlatePluginActivator::stop(ctkPluginContext* context)
{
	mRegistrationImageToPatient.reset();
	Q_UNUSED(context);
}

} // namespace cx
