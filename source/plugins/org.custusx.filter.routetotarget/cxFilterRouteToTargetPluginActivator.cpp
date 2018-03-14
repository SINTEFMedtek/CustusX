/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFilterRouteToTargetPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxRouteToTargetFilterService.h"
#include "cxRegisteredService.h"
#include "cxVisServices.h"

namespace cx
{

FilterRouteToTargetPluginActivator::FilterRouteToTargetPluginActivator()
{
}

FilterRouteToTargetPluginActivator::~FilterRouteToTargetPluginActivator()
{
}

void FilterRouteToTargetPluginActivator::start(ctkPluginContext* context)
{
	VisServicesPtr services = VisServices::create(context);
	RouteToTargetFilter *routeToTargetFilter = new RouteToTargetFilter(services);
	mRegistration = RegisteredService::create(context, routeToTargetFilter, FilterService_iid);
}

void FilterRouteToTargetPluginActivator::stop(ctkPluginContext* context)
{
    mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx



