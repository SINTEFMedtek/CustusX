/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxFilterClipMeshPluginActivator.h"

#include <QtPlugin>

#include "cxClipMeshFilter.h"
#include "cxRegisteredService.h"
#include "cxVisServices.h"

namespace cx
{

FilterClipMeshPluginActivator::FilterClipMeshPluginActivator()
{
}

FilterClipMeshPluginActivator::~FilterClipMeshPluginActivator()
{
}

void FilterClipMeshPluginActivator::start(ctkPluginContext* context)
{
	VisServicesPtr services = VisServices::create(context);
	ClipMeshFilter* filter = new ClipMeshFilter(services);
	mRegistration = RegisteredService::create(context, filter, FilterService_iid);
}

void FilterClipMeshPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx
