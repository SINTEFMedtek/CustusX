/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxCPDFilter.h"
#include "cxDataLocations.h"
#include "cxLogicManager.h"
#include "cxVisServices.h"
#include <QFileInfo>

namespace cxtest {

TEST_CASE("CPDFilter: instantiate and check script exists", "[unit][org.custusx.filter.cpd]")
{
	cx::LogicManager::initialize();

	cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());
	cx::CPDFilterPtr filter = cx::CPDFilterPtr(new cx::CPDFilter(services));

	REQUIRE(filter);
	CHECK(filter->getType() == "cpd_filter");
	CHECK(!filter->getName().isEmpty());

	QString scriptPath = cx::DataLocations::getFilterScriptsPath() + "scripts/python_cpd/cpd_registration.py";
	INFO("Expected script at: " << scriptPath.toStdString());
	CHECK(QFileInfo::exists(scriptPath));

	cx::LogicManager::shutdown();
}

} // end cxtest namespace
