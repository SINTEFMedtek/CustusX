/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxTypeConversions.h"
#include "cxtestMetricFixture.h"


TEST_CASE("cxFrameMetric can set/get transform", "[unit]")
{
    cxtest::MetricFixture fixture;
    cxtest::FrameMetricWithInput testData = fixture.getFrameMetricWithInput();

	CHECK(fixture.inputEqualsMetric(testData));
	CHECK_FALSE(fixture.messageListenerContainErrors());
}

TEST_CASE("cxFrameMetric can save/load XML", "[unit]")
{
    cxtest::MetricFixture fixture;
    cxtest::FrameMetricWithInput testData = fixture.getFrameMetricWithInput();

    CHECK(fixture.saveLoadXmlGivesEqualTransform(testData));
		CHECK_FALSE(fixture.messageListenerContainErrors());
}

TEST_CASE("cxFrameMetric can set space correctly", "[unit]")
{
    cxtest::MetricFixture fixture;
	cxtest::FrameMetricWithInput testData = fixture.getFrameMetricWithInput();

	fixture.setPatientRegistration();

	testData.mMetric->setSpace(cx::CoordinateSystem::patientReference());
	CHECK_FALSE(fixture.inputEqualsMetric(testData));

	testData.mMetric->setSpace(testData.mSpace);
	CHECK(fixture.inputEqualsMetric(testData));

	testData.mMetric.reset();
	CHECK_FALSE(fixture.messageListenerContainErrors());
}
