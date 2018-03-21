/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationMethodVesselPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxRegistrationMethodVesselService.h"
#include "cxRegisteredService.h"

namespace cx
{

RegistrationMethodVesselPluginActivator::RegistrationMethodVesselPluginActivator()
{
//	std::cout << "Created RegistrationMethodVesselPluginActivator" << std::endl;
}

RegistrationMethodVesselPluginActivator::~RegistrationMethodVesselPluginActivator()
{}

void RegistrationMethodVesselPluginActivator::start(ctkPluginContext* context)
{
	RegServicesPtr services = RegServices::create(context);
	RegistrationMethodVesselImageToImageService* image2imageService = new RegistrationMethodVesselImageToImageService(services);

	mRegistrationImageToImage = RegisteredServicePtr(new RegisteredService(context, image2imageService, RegistrationMethodService_iid));
}

void RegistrationMethodVesselPluginActivator::stop(ctkPluginContext* context)
{
	mRegistrationImageToImage.reset();
	Q_UNUSED(context);
}

} // namespace cx
