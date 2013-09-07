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

#include "sscTypeConversions.h"
#include "cxtestMetricFixture.h"


TEST_CASE("cxFrameMetric can set/get transform", "[unit]")
{
    cxtest::MetricFixture fixture;
    cxtest::FrameMetricWithInput testData = fixture.getFrameMetricWithInput();

	CHECK(fixture.inputEqualsMetric(testData));
}

TEST_CASE("cxFrameMetric can save/load XML", "[unit]")
{
    cxtest::MetricFixture fixture;
    cxtest::FrameMetricWithInput testData = fixture.getFrameMetricWithInput();

    CHECK(fixture.saveLoadXmlGivesEqualTransform(testData));
}

TEST_CASE("cxFrameMetric can convert transform to single line string", "[unit]")
{
    cxtest::MetricFixture fixture;
    cxtest::FrameMetricWithInput testData = fixture.getFrameMetricWithInput();

	QStringList list = fixture.getSingleLineDataList(testData.mMetric);
	REQUIRE(fixture.verifySingleLineHeader(list, testData.mMetric));

	REQUIRE(list[2]=="reference");
	INFO(list.join("\n"));
	bool transformStringOk = false;
	ssc::Transform3D readTransform = ssc::Transform3D::fromString(QStringList(list.mid(3, 16)).join(" "), &transformStringOk);
    REQUIRE(transformStringOk);
    REQUIRE(ssc::similar(testData.m_qMt, readTransform));
}

TEST_CASE("cxFrameMetric can set space correctly", "[unit]")
{
    cxtest::MetricFixture fixture;
    cxtest::FrameMetricWithInput testData = fixture.getFrameMetricWithInput();

    fixture.setPatientRegistration();

    testData.mMetric->setSpace(ssc::CoordinateSystemHelpers::getPr());
    CHECK_FALSE(fixture.inputEqualsMetric(testData));

    testData.mMetric->setSpace(testData.mSpace);
    CHECK(fixture.inputEqualsMetric(testData));
}
