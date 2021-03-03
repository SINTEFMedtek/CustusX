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



TEST_CASE("DistanceMetric composed of two points gives a correct distance", "[unit]")
{
    cxtest::MetricFixture fixture;
    double distance = 2;
//    PointMetricWithInput p0 = fixture.getPointMetricWithInput(Vector3D(0,0,0));
//    PointMetricWithInput p1 = fixture.getPointMetricWithInput(Vector3D(distance,0,0));
//    DistanceMetricWithInput testData = fixture.getDistanceMetricWithInput(distance, p0, p1);
	cxtest::DistanceMetricWithInput testData = fixture.getDistanceMetricWithInput(distance);

	CHECK(fixture.inputEqualsMetric(testData));
	CHECK_FALSE(fixture.messageListenerContainErrors());
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
	cx::Vector3D origin(10,10,0);

	cxtest::PointMetricWithInput p0 = fixture.getPointMetricWithInput(cx::Vector3D(0,0, distance));
	cxtest::PointMetricWithInput plane_origin = fixture.getPointMetricWithInput(origin);
	cxtest::PointMetricWithInput plane_dir = fixture.getPointMetricWithInput(origin+normal);
	cxtest::PlaneMetricWithInput p1 = fixture.getPlaneMetricWithInput(origin,
																	  normal,
																	  plane_origin.mMetric,
																	  plane_dir.mMetric);

	// from point to plane: negative distance
	cxtest::DistanceMetricWithInput testData = fixture.getDistanceMetricWithInput(-distance, p0.mMetric, p1.mMetric);
	CHECK(fixture.inputEqualsMetric(testData));

	// from plane to point: positive distance
	testData = fixture.getDistanceMetricWithInput(distance, p1.mMetric, p0.mMetric);
	CHECK(fixture.inputEqualsMetric(testData));
	CHECK_FALSE(fixture.messageListenerContainErrors());
}

TEST_CASE("DistanceMetric can save/load XML", "[unit]")
{
    cxtest::MetricFixture fixture;

    double distance = 2;
	cxtest::DistanceMetricWithInput testData = fixture.getDistanceMetricWithInput(distance);

    CHECK(fixture.saveLoadXmlGivesEqualTransform(testData));
		CHECK_FALSE(fixture.messageListenerContainErrors());
}

