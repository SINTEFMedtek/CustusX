/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFilterAirwaysPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxAirwaysFilterService.h"
#include "cxRegisteredService.h"
#include "cxVisServices.h"

namespace cx
{

FilterAirwaysPluginActivator::FilterAirwaysPluginActivator()
{
}

FilterAirwaysPluginActivator::~FilterAirwaysPluginActivator()
{
}

void FilterAirwaysPluginActivator::start(ctkPluginContext* context)
{
	VisServicesPtr services = VisServices::create(context);
	AirwaysFilter *airwaysFilter = new AirwaysFilter(services);
	mRegistration = RegisteredService::create(context, airwaysFilter, FilterService_iid);
}

void FilterAirwaysPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx



