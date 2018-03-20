/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPNNReconstructionPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxPNNReconstructionMethodService.h"
#include "cxRegisteredService.h"

namespace cx
{

PNNReconstructionPluginActivator::PNNReconstructionPluginActivator()
{
}

PNNReconstructionPluginActivator::~PNNReconstructionPluginActivator()
{
}

void PNNReconstructionPluginActivator::start(ctkPluginContext* context)
{
	mRegistration = RegisteredService::create<PNNReconstructionMethodService>(context, ReconstructionMethodService_iid);
}

void PNNReconstructionPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx



