/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxRegistrationServiceProxy.h"
#include "cxLogicManager.h"
#include "cxPluginFramework.h"

namespace
{

void init()
{
	cx::LogicManager::initialize();
}
void shutdown()
{
	cx::LogicManager::shutdown();
}

ctkPluginContext* getPluginContext()
{
	cx::LogicManager::getInstance()->getPluginFramework()->start();
	ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginContext();
	return context;
}
} // namespace

namespace cxtest
{

TEST_CASE("RegistrationServiceProxy works", "[unit][resource][core]")
{
	init();
	ctkPluginContext* context = getPluginContext();
	cx::RegistrationServicePtr registrationService;
	registrationService.reset(new cx::RegistrationServiceProxy(context));
	REQUIRE(registrationService);
	REQUIRE_FALSE(registrationService->isNull());
	registrationService.reset();
	shutdown();
}

} //cxtest
