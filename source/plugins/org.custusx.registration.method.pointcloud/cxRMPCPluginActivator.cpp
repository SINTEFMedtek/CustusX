/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRMPCPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxRMPCService.h"
#include "cxRMPCFromPointerService.h"
#include "cxRegisteredService.h"


namespace cx
{

RMPCPluginActivator::RMPCPluginActivator()
{
//	std::cout << "Created RMPCPluginActivator" << std::endl;
}

RMPCPluginActivator::~RMPCPluginActivator()
{
}

void RMPCPluginActivator::start(ctkPluginContext* context)
{	
	mPointerToSurface = RegisteredService::create<RMPCFromPointerImageToPatientService>(context, RegistrationMethodService_iid);
	mPointCloud = RegisteredService::create<RMPCImageToPatientService>(context, RegistrationMethodService_iid);
}

void RMPCPluginActivator::stop(ctkPluginContext* context)
{
	mPointerToSurface.reset();
	mPointCloud.reset();
	Q_UNUSED(context);
}

} // namespace cx


