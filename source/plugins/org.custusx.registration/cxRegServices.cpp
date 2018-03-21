/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxRegServices.h"

#include <ctkPluginContext.h>
#include "cxRegistrationServiceProxy.h"
#include "cxViewServiceProxy.h"
#include "cxAcquisitionServiceProxy.h"

namespace cx {

RegServicesPtr RegServices::create(ctkPluginContext* context)
{
	return RegServicesPtr(new RegServices(context));
}

RegServices::RegServices(ctkPluginContext* context) :
	VisServices(context)
{
	registrationService	 = RegistrationServicePtr(new RegistrationServiceProxy(context));
	acquisitionService	 = AcquisitionServicePtr(new AcquisitionServiceProxy(context));
}

RegServicesPtr RegServices::getNullObjects()
{
	return RegServicesPtr(new RegServices());
}

RegServices::RegServices()
{
	registrationService		= RegistrationService::getNullObject();
	acquisitionService		= AcquisitionService::getNullObject();
}
} // cx
