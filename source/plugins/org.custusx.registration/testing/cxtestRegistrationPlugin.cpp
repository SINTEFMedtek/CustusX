/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
