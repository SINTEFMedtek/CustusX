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
#include "cxPluginFramework.h"
#include <ctkServiceTracker.h>
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxMessageListener.h"
#include "cxReporter.h"

/** Test that one plugin can be sucessfully loaded, both in the unit (build folder)
  * and the integration (install folder) step.
  */
TEST_CASE("LogicManager: Load one core plugin (PatientModelService)", "[integration][unit][plugins]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();

	ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginContext();
	ctkServiceTracker<cx::PatientModelService*> tracker(context);
	tracker.open();
	cx::PatientModelService* service = tracker.getService(); // get arbitrary instance of this type

	CHECK(service);

	cx::LogicManager::shutdown();
}

TEST_CASE("LogicManager: init and shutdown without it posting the warning: QObject::killTimer: timers cannot be stopped from another thread", "[integration][unit][plugins]")
{
    cx::reporter()->initialize();
    cx::MessageListenerPtr messageListener = cx::MessageListener::createWithQueue();

    cx::DataLocations::setTestMode();
    cx::LogicManager::initialize();

    cx::LogicManager::shutdown();
    REQUIRE(!messageListener->containsText("QObject::killTimer: timers cannot be stopped from another thread"));
}

