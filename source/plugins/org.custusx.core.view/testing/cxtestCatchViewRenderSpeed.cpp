/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxTestRenderSpeed.h"
#include "cxtestJenkinsMeasurement.h"
#include "cxDataLocations.h"
#include "cxSettings.h"

namespace cxtest
{

TEST_CASE("Speed: Render 10 empty viewWidgets, optimizedViews on", "[speed][gui][integration]")
{
	cx::DataLocations::setTestMode();
	cx::settings()->setValue("optimizedViews", true);
	TestRenderSpeed helper;
    helper.testSeveralViews();

    REQUIRE(helper.getRenderFPS() > 10);

	JenkinsMeasurement jenkins;
    jenkins.createOutput("FPS", QString::number(helper.getRenderFPS()));
}

TEST_CASE("Speed: Render 10 empty viewWidgets, optimizedViews off", "[speed][gui][integration]")
{
	cx::DataLocations::setTestMode();
	cx::settings()->setValue("optimizedViews", false);
	TestRenderSpeed helper;
	helper.testSeveralViews();

	REQUIRE(helper.getRenderFPS() > 1);

	JenkinsMeasurement jenkins;
	jenkins.createOutput("FPS", QString::number(helper.getRenderFPS()));
}

} //namespace cxtest
