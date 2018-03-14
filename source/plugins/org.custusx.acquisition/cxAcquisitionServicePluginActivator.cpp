/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxAcquisitionServicePluginActivator.h"

#include <QtPlugin>

#include "cxAcquisitionImplService.h"
#include "cxRegisteredService.h"
#include "cxAcquisitionPlugin.h"

namespace cx
{

AcquisitionServicePluginActivator::AcquisitionServicePluginActivator()
{
}

AcquisitionServicePluginActivator::~AcquisitionServicePluginActivator()
{
}

void AcquisitionServicePluginActivator::start(ctkPluginContext* context)
{
	mRegisteredAcquisitionService = RegisteredService::create<AcquisitionImplService>(context, AcquisitionService_iid);
	mRegisteredAcquisitionGUIService = RegisteredService::create<AcquisitionPlugin>(context, GUIExtenderService_iid);
}

void AcquisitionServicePluginActivator::stop(ctkPluginContext* context)
{
	mRegisteredAcquisitionGUIService.reset();
	mRegisteredAcquisitionService.reset();
	Q_UNUSED(context);
}

} // cx
