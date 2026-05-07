/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxFilterCPDPluginActivator.h"

#include <QtPlugin>

#include "cxCPDFilter.h"
#include "cxRegisteredService.h"
#include "cxVisServices.h"

namespace cx
{

FilterCPDPluginActivator::FilterCPDPluginActivator()
{
}

FilterCPDPluginActivator::~FilterCPDPluginActivator()
{
}

void FilterCPDPluginActivator::start(ctkPluginContext* context)
{
	VisServicesPtr services = VisServices::create(context);
	CPDFilter* cpdFilter = new CPDFilter(services);
	mRegistration = RegisteredService::create(context, cpdFilter, FilterService_iid);
}

void FilterCPDPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx
