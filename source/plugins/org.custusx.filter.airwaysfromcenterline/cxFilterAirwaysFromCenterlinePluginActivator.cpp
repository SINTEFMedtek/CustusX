/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFilterAirwaysFromCenterlinePluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxAirwaysFromCenterlineFilterService.h"
#include "cxRegisteredService.h"
#include "cxVisServices.h"

namespace cx
{

FilterAirwaysFromCenterlinePluginActivator::FilterAirwaysFromCenterlinePluginActivator()
{
}

FilterAirwaysFromCenterlinePluginActivator::~FilterAirwaysFromCenterlinePluginActivator()
{
}

void FilterAirwaysFromCenterlinePluginActivator::start(ctkPluginContext* context)
{
	VisServicesPtr services = VisServices::create(context);
        AirwaysFromCenterlineFilter *airwaysFromCenterlineFilter = new AirwaysFromCenterlineFilter(services);
        mRegistration = RegisteredService::create(context, airwaysFromCenterlineFilter, FilterService_iid);
}

void FilterAirwaysFromCenterlinePluginActivator::stop(ctkPluginContext* context)
{
    mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx



