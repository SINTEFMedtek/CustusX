/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxLogicManager.h"
#include "cxDataLocations.h"
#include "cxLogger.h"

#include "cxTrackingImplService.h"
#include "cxTool.h"
#include "cxTrackerConfiguration.h"

namespace cxtest
{

TEST_CASE("TrackingPlugin: Check nothing", "[unit][plugins][org.custusx.core.tracking][hide]")
{
	CHECK(true);
}

TEST_CASE("TrackingImplService: Init", "[unit][plugins][org.custusx.core.tracking]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
	ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginContext();
	cx::TrackingImplServicePtr tracking(new cx::TrackingImplService(context));

	CHECK(tracking->getState() == cx::Tool::tsNONE);

	std::vector<cx::TrackerConfigurationPtr> configs = tracking->getConfigurations();
	REQUIRE(configs.size() > 0);
	//CX_LOG_DEBUG() << "Num configs: " << configs.size();

	cx::TrackerConfigurationPtr config = configs[0];
	REQUIRE(config);
	CHECK(config->getAllTools().size() > 0);

	//CX_LOG_DEBUG() << "Tools: " << config->getAllTools().size();
	//CX_LOG_DEBUG() << "CurrentTrackingSystemImplementation: " << tracking->getCurrentTrackingSystemImplementation();

	cx::LogicManager::shutdown();
}

TEST_CASE("TrackingImplService: CurrentTrackingSystemImplementation", "[unit][plugins][org.custusx.core.tracking]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
	ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginContext();
	cx::TrackingImplServicePtr tracking(new cx::TrackingImplService(context));

	CHECK(tracking->getCurrentTrackingSystemImplementation().isEmpty());

	std::vector<cx::TrackerConfigurationPtr> configs = tracking->getConfigurations();
	REQUIRE(configs.size() > 1);

	cx::TrackerConfigurationPtr config = tracking->getConfiguration();
	CHECK(config);
	//CX_LOG_DEBUG() << "config impl: " << config->getTrackingSystemImplementation();
	CHECK(config->getTrackingSystemImplementation() == configs[0]->getTrackingSystemImplementation());//Tracking returns first system at init

	tracking->setCurrentTrackingSystemImplementation(configs[1]->getTrackingSystemImplementation());
	CHECK(tracking->getCurrentTrackingSystemImplementation() == configs[1]->getTrackingSystemImplementation());
	CHECK(tracking->getCurrentTrackingSystemImplementation() != configs[0]->getTrackingSystemImplementation());

	config = tracking->getConfiguration();
	CHECK(config->getTrackingSystemImplementation() == configs[1]->getTrackingSystemImplementation());

	cx::LogicManager::shutdown();
}
}//cxtest
