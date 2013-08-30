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


TEST_CASE("ToolMetric can set/get tool data", "[unit]")
{
	cxtest::MetricFixture fixture;
	cxtest::ToolMetricData testData = fixture.getToolMetricData();
	CHECK(fixture.metricEqualsData(testData));
}

TEST_CASE("ToolMetric can save/load XML", "[unit]")
{
	cxtest::MetricFixture fixture;
	cxtest::ToolMetricData testData = fixture.getToolMetricData();

	CHECK(fixture.saveLoadXmlGivesEqualTransform(testData));
}

//TEST_CASE("ToolMetric can convert values to single line string", "[unit]")
//{
//	cxtest::MetricFixture fixture;
//	cxtest::ToolMetricData testData = fixture.getToolMetricData();

//	QString singleLine = testData.mMetric->getAsSingleLineString();
//	REQUIRE(!singleLine.isEmpty());
//	INFO("line: " + singleLine);
//	QStringList list = fixture.splitStringLineIntoTextComponents(singleLine);
//	INFO("list: " + list.join("\n"));
//	REQUIRE(list.size()>2);
//	REQUIRE(fixture.verifySingleLineHeader(list, testData.mMetric));

//	CHECK(list[2].toDouble() == Approx(distance));
//}

//TEST_CASE("ToolMetric can set space correctly", "[unit]")
//{
//}

//TEST_CASE("ToolMetric can get a valid reference coordinate", "[unit]")
//{
//    cxtest::MetricFixture fixture;
//    cxtest::ToolMetricData testData = fixture.getToolMetricData();

////    ssc::Vector3D testCoord(0,0,0); // TODO insert correct value
////    ssc::Vector3D refCoord = testData.mMetric->getRefCoord();
////    CHECK(ssc::similar(refCoord, testCoord);

//    fixture.setPatientRegistration();
//    testData.mMetric->setSpace(ssc::CoordinateSystemHelpers::getPr());

//    ssc::Vector3D testCoord(0,0,0); // TODO insert correct value
//    ssc::Vector3D refCoord = testData.mMetric->getRefCoord();
//    CHECK(ssc::similar(refCoord, testCoord);
//}
