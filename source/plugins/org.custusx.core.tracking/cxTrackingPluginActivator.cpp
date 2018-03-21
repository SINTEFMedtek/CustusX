/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTrackingPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxTrackingImplService.h"
#include "cxRegisteredService.h"

namespace cx
{

TrackingPluginActivator::TrackingPluginActivator()
{
}

TrackingPluginActivator::~TrackingPluginActivator()
{
}

void TrackingPluginActivator::start(ctkPluginContext* context)
{
	mRegistration = RegisteredService::create<TrackingImplService>(context, TrackingService_iid);
}

void TrackingPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx



