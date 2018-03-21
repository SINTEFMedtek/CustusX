/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxStatePluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxStateServiceImpl.h"
#include "cxRegisteredService.h"

namespace cx
{

StatePluginActivator::StatePluginActivator()
{
}

StatePluginActivator::~StatePluginActivator()
{
}

void StatePluginActivator::start(ctkPluginContext* context)
{
	mStateService = RegisteredService::create<StateServiceImpl>(context, StateService_iid);
}

void StatePluginActivator::stop(ctkPluginContext* context)
{
	mStateService.reset();
	Q_UNUSED(context);
}

} // namespace cx

