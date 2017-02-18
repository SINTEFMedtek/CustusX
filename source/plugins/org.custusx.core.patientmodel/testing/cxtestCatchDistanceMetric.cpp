/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
	CHECK(list[3].toDouble() == Approx(distance));
}

TEST_CASE("DistanceMetric can get a valid reference coordinate", "[unit]")
{
}

