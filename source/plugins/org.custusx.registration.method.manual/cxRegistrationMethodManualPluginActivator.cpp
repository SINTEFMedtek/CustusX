/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxRegistrationMethodManualPluginActivator.h"

#include <QtPlugin>
#include <iostream>

//#include "cxRegistrationMethodManualService.h"
#include "cxRegistrationMethodManualImageToImageService.h"
#include "cxRegistrationMethodManualImageToPatientService.h"
#include "cxRegistrationMethodManualImageTransformService.h"
#include "cxRegisteredService.h"
#include "cxPatientModelServiceProxy.h"
#include "cxRegistrationServiceProxy.h"

namespace cx
{

RegistrationMethodManualPluginActivator::RegistrationMethodManualPluginActivator()
{
//    std::cout << "Created RegistrationMethodManualPluginActivator" << std::endl;
}

RegistrationMethodManualPluginActivator::~RegistrationMethodManualPluginActivator()
{}

void RegistrationMethodManualPluginActivator::start(ctkPluginContext* context)
{
	PatientModelServicePtr patientModelService = PatientModelServicePtr(new PatientModelServiceProxy(context));
	RegistrationServicePtr registrationService = RegistrationServicePtr(new RegistrationServiceProxy(context));

	mRegistrationImageToImage	= RegisteredServicePtr(new RegisteredService(context, new RegistrationMethodManualImageToImageService(registrationService), RegistrationMethodService_iid));
	mRegistrationImageToPatient = RegisteredServicePtr(new RegisteredService(context, new RegistrationMethodManualImageToPatientService(registrationService, patientModelService), RegistrationMethodService_iid));
	mRegistrationImageTransform = RegisteredServicePtr(new RegisteredService(context, new RegistrationMethodManualImageTransformService(registrationService) , RegistrationMethodService_iid));
}

void RegistrationMethodManualPluginActivator::stop(ctkPluginContext* context)
{
	mRegistrationImageToImage.reset();
	mRegistrationImageToPatient.reset();
	mRegistrationImageTransform.reset();
	Q_UNUSED(context);
}

} // namespace cx

Q_EXPORT_PLUGIN2(RegistrationMethodManualPluginActivator_irrelevant_string, cx::RegistrationMethodManualPluginActivator)


