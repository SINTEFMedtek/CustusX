/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationMethodBronchoscopyPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxRegistrationMethodBronchoscopyService.h"
#include "cxRegisteredService.h"

namespace cx
{

RegistrationMethodBronchoscopyPluginActivator::RegistrationMethodBronchoscopyPluginActivator()
{
}

RegistrationMethodBronchoscopyPluginActivator::~RegistrationMethodBronchoscopyPluginActivator()
{}

void RegistrationMethodBronchoscopyPluginActivator::start(ctkPluginContext* context)
{
	RegServicesPtr services = RegServices::create(context);
	RegistrationMethodBronchoscopyImageToPatientService* image2patientService = new RegistrationMethodBronchoscopyImageToPatientService(services);
    RegistrationMethodBronchoscopyImageToImageService* image2imageService = new RegistrationMethodBronchoscopyImageToImageService(services);

	mRegistrationImageToPatient = RegisteredServicePtr(new RegisteredService(context, image2patientService, RegistrationMethodService_iid));
    mRegistrationImageToImage = RegisteredServicePtr(new RegisteredService(context, image2imageService, RegistrationMethodService_iid));
}

void RegistrationMethodBronchoscopyPluginActivator::stop(ctkPluginContext* context)
{
	mRegistrationImageToPatient.reset();
    mRegistrationImageToImage.reset();
	Q_UNUSED(context);
}

} // namespace cx
