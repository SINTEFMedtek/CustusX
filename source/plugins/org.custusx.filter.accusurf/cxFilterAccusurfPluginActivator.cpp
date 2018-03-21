/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFilterAccusurfPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxAccusurfFilterService.h"
#include "cxRegisteredService.h"
#include "cxVisServices.h"

namespace cx
{

FilterAccusurfPluginActivator::FilterAccusurfPluginActivator()
{
}

FilterAccusurfPluginActivator::~FilterAccusurfPluginActivator()
{
}

void FilterAccusurfPluginActivator::start(ctkPluginContext* context)
{
	VisServicesPtr services = VisServices::create(context);
	AccusurfFilter *accusurfFilter = new AccusurfFilter(services);
	mRegistration = RegisteredService::create(context, accusurfFilter, FilterService_iid);
}

void FilterAccusurfPluginActivator::stop(ctkPluginContext* context)
{
    mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx



