/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationMethodLandmarkPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxRegistrationMethodLandmarkService.h"
#include "cxRegisteredService.h"

namespace cx
{

RegistrationMethodLandmarkPluginActivator::RegistrationMethodLandmarkPluginActivator()
{
//	std::cout << "Created RegistrationMethodLandmarkPluginActivator" << std::endl;
}

RegistrationMethodLandmarkPluginActivator::~RegistrationMethodLandmarkPluginActivator()
{}

void RegistrationMethodLandmarkPluginActivator::start(ctkPluginContext* context)
{
	RegServicesPtr services = RegServices::create(context);
	RegistrationMethodLandmarkImageToImageService* image2imageService = new RegistrationMethodLandmarkImageToImageService(services);
	RegistrationMethodLandmarkImageToPatientService* image2patientService = new RegistrationMethodLandmarkImageToPatientService(services);
	RegistrationMethodFastLandmarkImageToPatientService* fastImage2patientService = new RegistrationMethodFastLandmarkImageToPatientService(services);

	mRegistrationImageToImage = RegisteredService::create(context, image2imageService, RegistrationMethodService_iid);
	mRegistrationImageToPatient = RegisteredService::create(context, image2patientService, RegistrationMethodService_iid);
	mRegistrationFastImageToPatient = RegisteredService::create(context, fastImage2patientService, RegistrationMethodService_iid);
}

void RegistrationMethodLandmarkPluginActivator::stop(ctkPluginContext* context)
{
	mRegistrationImageToImage.reset();
	mRegistrationImageToPatient.reset();
	mRegistrationFastImageToPatient.reset();
	Q_UNUSED(context);
}

} // namespace cx
