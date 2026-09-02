/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLiverPluginActivator.h"

#include <QtPlugin>

#include "cxLiverGUIExtenderService.h"
#include "cxRegisteredService.h"

namespace cx
{

LiverPluginActivator::LiverPluginActivator()
{
}

LiverPluginActivator::~LiverPluginActivator()
{
}

void LiverPluginActivator::start(ctkPluginContext* context)
{
	mRegistration = RegisteredService::create<LiverGUIExtenderService>(context, GUIExtenderService_iid);
}

void LiverPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx
