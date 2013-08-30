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
//    PointMetricData p0 = fixture.getPointMetricData(ssc::Vector3D(0,0,0));
//    PointMetricData p1 = fixture.getPointMetricData(ssc::Vector3D(distance,0,0));
//    DistanceMetricData testData = fixture.getDistanceMetricData(distance, p0, p1);
    cxtest::DistanceMetricData testData = fixture.getDistanceMetricData(distance);

    CHECK(fixture.metricEqualsData(testData));
}

// not supported
//TEST_CASE("DistanceMetric composed of two planes gives a correct distance", "[unit]")
//{
//    cxtest::MetricFixture fixture;
//    double distance = 2;
//    ssc::Vector3D normal(0,0,1);
//    cxtest::PlaneMetricData p0 = fixture.getPlaneMetricData(ssc::Vector3D(0,0,0), normal);
//    cxtest::PlaneMetricData p1 = fixture.getPlaneMetricData(ssc::Vector3D(distance,0,0), normal);
//    cxtest::DistanceMetricData testData = fixture.getDistanceMetricData(distance, p0.mMetric, p1.mMetric);

//    CHECK(fixture.metricEqualsData(testData));
//}

TEST_CASE("DistanceMetric composed of a point and plane gives a correct distance", "[unit]")
{
    cxtest::MetricFixture fixture;
    double distance = 2;
    ssc::Vector3D normal(0,0,1);
    cxtest::PointMetricData p0 = fixture.getPointMetricData(ssc::Vector3D(0,0, distance));
    cxtest::PlaneMetricData p1 = fixture.getPlaneMetricData(ssc::Vector3D(0,0,0), normal);

    cxtest::DistanceMetricData testData = fixture.getDistanceMetricData(distance, p0.mMetric, p1.mMetric);
    CHECK(fixture.metricEqualsData(testData));

    testData = fixture.getDistanceMetricData(distance, p1.mMetric, p0.mMetric);
    CHECK(fixture.metricEqualsData(testData));
}

TEST_CASE("DistanceMetric can save/load XML", "[unit]")
{
    cxtest::MetricFixture fixture;

    double distance = 2;
    cxtest::DistanceMetricData testData = fixture.getDistanceMetricData(distance);

    CHECK(fixture.saveLoadXmlGivesEqualTransform(testData));
}



TEST_CASE("DistanceMetric can convert values to single line string", "[unit]")
{
    cxtest::MetricFixture fixture;
    double distance = 2;
    cxtest::DistanceMetricData testData = fixture.getDistanceMetricData(distance);

    QString singleLine = testData.mMetric->getAsSingleLineString();
    REQUIRE(!singleLine.isEmpty());
    INFO("line: " + singleLine);
    QStringList list = fixture.splitStringLineIntoTextComponents(singleLine);
    INFO("list: " + list.join("\n"));
    REQUIRE(list.size()>2);
    REQUIRE(fixture.verifySingleLineHeader(list, testData.mMetric));

    CHECK(list[2].toDouble() == Approx(distance));
}

TEST_CASE("DistanceMetric can get a valid reference coordinate", "[unit]")
{
}

