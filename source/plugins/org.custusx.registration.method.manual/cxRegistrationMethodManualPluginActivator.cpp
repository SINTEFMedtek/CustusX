/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationMethodManualPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxRegistrationMethodManualImageToImageService.h"
#include "cxRegistrationMethodManualImageToPatientService.h"
#include "cxRegistrationMethodManualImageTransformService.h"
#include "cxRegistrationMethodManualPatientOrientationService.h"
#include "cxRegisteredService.h"

namespace cx
{

RegistrationMethodManualPluginActivator::RegistrationMethodManualPluginActivator()
{
//	std::cout << "Created RegistrationMethodManualPluginActivator" << std::endl;
}

RegistrationMethodManualPluginActivator::~RegistrationMethodManualPluginActivator()
{}

void RegistrationMethodManualPluginActivator::start(ctkPluginContext* context)
{
	RegServicesPtr services = RegServices::create(context);

	RegistrationMethodManualImageToImageService *i2i = new RegistrationMethodManualImageToImageService(services);
	RegistrationMethodManualImageToPatientService *i2p = new RegistrationMethodManualImageToPatientService(services);
	RegistrationMethodManualImageTransformService *iTransform = new RegistrationMethodManualImageTransformService(services);
	RegistrationMethodManualPatientOrientationService *patientOrientation = new RegistrationMethodManualPatientOrientationService(services);

	mRegistrationImageToImage = RegisteredServicePtr(new RegisteredService(context, i2i, RegistrationMethodService_iid));
	mRegistrationImageToPatient = RegisteredServicePtr(new RegisteredService(context, i2p, RegistrationMethodService_iid));
	mRegistrationImageTransform = RegisteredServicePtr(new RegisteredService(context, iTransform, RegistrationMethodService_iid));
	mRegistrationPatientOrientation = RegisteredServicePtr(new RegisteredService(context, patientOrientation, RegistrationMethodService_iid));
}

void RegistrationMethodManualPluginActivator::stop(ctkPluginContext* context)
{
	mRegistrationImageToImage.reset();
	mRegistrationImageToPatient.reset();
	mRegistrationImageTransform.reset();
	mRegistrationPatientOrientation.reset();
	Q_UNUSED(context);
}

} // namespace cx
