/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxVNNclPluginActivator.h"

#include <QtPlugin>
#include <iostream>
#include "cxVNNclReconstructionMethodService.h"
#include "cxRegisteredService.h"

namespace cx
{

VNNclPluginActivator::VNNclPluginActivator()
{
}

VNNclPluginActivator::~VNNclPluginActivator()
{

}

void VNNclPluginActivator::start(ctkPluginContext* context)
{
	mRegistration = RegisteredService::create<VNNclReconstructionMethodService>(context, ReconstructionMethodService_iid);
}

void VNNclPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx



