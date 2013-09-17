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



TEST_CASE("DistanceMetric composed of two points gives a correct distance", "[unit]")
{
    cxtest::MetricFixture fixture;
    double distance = 2;
//    PointMetricWithInput p0 = fixture.getPointMetricWithInput(Vector3D(0,0,0));
//    PointMetricWithInput p1 = fixture.getPointMetricWithInput(Vector3D(distance,0,0));
//    DistanceMetricWithInput testData = fixture.getDistanceMetricWithInput(distance, p0, p1);
	cxtest::DistanceMetricWithInput testData = fixture.getDistanceMetricWithInput(distance);

	CHECK(fixture.inputEqualsMetric(testData));
}

// not supported
//TEST_CASE("DistanceMetric composed of two planes gives a correct distance", "[unit]")
//{
//    cxtest::MetricFixture fixture;
//    double distance = 2;
//    Vector3D normal(0,0,1);
//    cxtest::PlaneMetricWithInput p0 = fixture.getPlaneMetricWithInput(Vector3D(0,0,0), normal);
//    cxtest::PlaneMetricWithInput p1 = fixture.getPlaneMetricWithInput(Vector3D(distance,0,0), normal);
//    cxtest::DistanceMetricWithInput testData = fixture.getDistanceMetricWithInput(distance, p0.mMetric, p1.mMetric);

//    CHECK(fixture.inputEqualsMetric(testData));
//}

TEST_CASE("DistanceMetric composed of a point and plane gives a correct distance", "[unit]")
{
    cxtest::MetricFixture fixture;
    double distance = 2;
	cx::Vector3D normal(0,0,1);
	cxtest::PointMetricWithInput p0 = fixture.getPointMetricWithInput(cx::Vector3D(0,0, distance));
	cxtest::PlaneMetricWithInput p1 = fixture.getPlaneMetricWithInput(cx::Vector3D(0,0,0), normal);

	cxtest::DistanceMetricWithInput testData = fixture.getDistanceMetricWithInput(distance, p0.mMetric, p1.mMetric);
	CHECK(fixture.inputEqualsMetric(testData));

	testData = fixture.getDistanceMetricWithInput(distance, p1.mMetric, p0.mMetric);
	CHECK(fixture.inputEqualsMetric(testData));
}

TEST_CASE("DistanceMetric can save/load XML", "[unit]")
{
    cxtest::MetricFixture fixture;

    double distance = 2;
	cxtest::DistanceMetricWithInput testData = fixture.getDistanceMetricWithInput(distance);

    CHECK(fixture.saveLoadXmlGivesEqualTransform(testData));
}

TEST_CASE("DistanceMetric can convert values to single line string", "[unit]")
{
    cxtest::MetricFixture fixture;
    double distance = 2;
	cxtest::DistanceMetricWithInput testData = fixture.getDistanceMetricWithInput(distance);

	QStringList list = fixture.getSingleLineDataList(testData.mMetric);
	CHECK(fixture.verifySingleLineHeader(list, testData.mMetric));
	INFO(list.join("\n"));
	REQUIRE(list.length() > 2);
    CHECK(list[2].toDouble() == Approx(distance));
}

TEST_CASE("DistanceMetric can get a valid reference coordinate", "[unit]")
{
}

