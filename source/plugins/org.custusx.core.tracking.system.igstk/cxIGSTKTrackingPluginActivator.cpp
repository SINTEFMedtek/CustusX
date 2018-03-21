/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxIGSTKTrackingPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxTrackingSystemIGSTKService.h"
#include "cxRegisteredService.h"

namespace cx
{

IGSTKTrackingPluginActivator::IGSTKTrackingPluginActivator()
{
}

IGSTKTrackingPluginActivator::~IGSTKTrackingPluginActivator()
{
}

void IGSTKTrackingPluginActivator::start(ctkPluginContext* context)
{
    TrackingSystemIGSTKService* igstk = new TrackingSystemIGSTKService();
    mRegistration = RegisteredService::create<TrackingSystemIGSTKService>(context, igstk, TrackingSystemService_iid);
}

void IGSTKTrackingPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx



