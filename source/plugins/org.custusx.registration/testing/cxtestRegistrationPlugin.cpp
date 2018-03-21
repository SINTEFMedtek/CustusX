/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"
#include "ctkServiceTracker.h"

#include "cxRegistrationServiceNull.h"
#include "cxRegistrationImplService.h"
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
TEST_CASE("RegistrationPlugin: Check RegistrationServiceNull", "[unit][plugins][org.custusx.registration]")
{
	cx::RegistrationServicePtr service = cx::RegistrationService::getNullObject();
	REQUIRE(service);
	REQUIRE_FALSE(service->getFixedData());
	REQUIRE_FALSE(service->getMovingData());
	REQUIRE(service->isNull());

	cx::RegistrationServicePtr service2 = cx::RegistrationService::getNullObject();
	REQUIRE(service == service2);
	REQUIRE(service.get() == service2.get());
}

TEST_CASE("RegistrationPlugin: Check empty RegistrationImplService", "[unit][plugins][org.custusx.registration]")
{
	init();
	cx::RegistrationServicePtr service = cx::RegistrationService::getNullObject();
	service.reset(new cx::RegistrationImplService(getPluginContext()));
	REQUIRE(service);
	REQUIRE_FALSE(service->getFixedData());
	REQUIRE_FALSE(service->getMovingData());
	REQUIRE_FALSE(service->isNull());
	service.reset();
	shutdown();
}

TEST_CASE("RegistrationPlugin: RegistrationService available", "[unit][plugins][org.custusx.registration]")
{
	init();
	ctkPluginContext* context = getPluginContext();
	REQUIRE(context);
	ctkServiceTracker<cx::RegistrationService*> tracker(context);
	tracker.open();
	cx::RegistrationService* service = tracker.getService();
	REQUIRE(service);
	REQUIRE_FALSE(service->isNull());
	shutdown();
}
} //namespace cxtest
