/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxNDITrackingPluginActivator.h"

#include <QtPlugin>

#include "cxTrackingSystemNDIService.h"
#include "cxRegisteredService.h"

namespace cx
{

NDITrackingPluginActivator::NDITrackingPluginActivator()
{
}

NDITrackingPluginActivator::~NDITrackingPluginActivator()
{
}

void NDITrackingPluginActivator::start(ctkPluginContext* context)
{
	TrackingSystemNDIService* ndi = new TrackingSystemNDIService();
	mRegistration = RegisteredService::create<TrackingSystemNDIService>(context, ndi, TrackingSystemService_iid);
}

void NDITrackingPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx
