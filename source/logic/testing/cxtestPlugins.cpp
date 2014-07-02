// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "catch.hpp"
#include "cxLogicManager.h"
#include "cxDataLocations.h"
#include "cxPluginFramework.h"
#include <ctkServiceTracker.h>
#include "cxPatientModelService.h"

/** Test that one plugin can be sucessfully loaded, both in the unit (build folder)
  * and the integration (install folder) step.
  */
TEST_CASE("LogicManager: Load one core plugin (PatientModelService)", "[integration][unit][plugins]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();

	ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginFramework()->getPluginContext();
	ctkServiceTracker<cx::PatientModelService*> tracker(context);
	tracker.open();
	cx::PatientModelService* service = tracker.getService(); // get arbitrary instance of this type

	CHECK(service);

	cx::LogicManager::shutdown();
}

