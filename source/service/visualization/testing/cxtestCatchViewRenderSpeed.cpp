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
#include "cxTestRenderSpeed.h"
#include "cxtestJenkinsMeasurement.h"

namespace cxtest
{

TEST_CASE("Speed: Render 10 empty viewWidgets", "[speed][gui][integration]")
{
    TestRenderSpeed helper;
    helper.testSeveralViews();

    REQUIRE(helper.getRenderFPS() > 10);

		JenkinsMeasurement jenkins;
    jenkins.createOutput("FPS", QString::number(helper.getRenderFPS()));
//    helper.printResult();
}

TEST_CASE("Speed: Render 10 empty vtkRenderWindows", "[speed][gui][integration]")
{
    TestRenderSpeed helper;
    helper.testSeveralVtkRenderWindows();

    REQUIRE(helper.getRenderFPS() > 10);

		JenkinsMeasurement jenkins;
    jenkins.createOutput("FPS", QString::number(helper.getRenderFPS()));
//    helper.printResult();
}

} //namespace cxtest
