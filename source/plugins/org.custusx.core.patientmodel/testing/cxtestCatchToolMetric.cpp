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


TEST_CASE("ToolMetric can set/get tool data", "[unit]")
{
	cxtest::MetricFixture fixture;
	cxtest::ToolMetricWithInput data = fixture.getToolMetricWithInput();

	CHECK(fixture.inputEqualsMetric(data));
}

TEST_CASE("ToolMetric can save/load XML", "[unit]")
{
	cxtest::MetricFixture fixture;
	cxtest::ToolMetricWithInput data = fixture.getToolMetricWithInput();

	CHECK(fixture.saveLoadXmlGivesEqualTransform(data));
}

TEST_CASE("ToolMetric can set space correctly", "[unit]")
{
	cxtest::MetricFixture fixture;
	cxtest::ToolMetricWithInput testData = fixture.getToolMetricWithInput();

	fixture.setPatientRegistration();

	testData.mMetric->setSpace(cx::CoordinateSystem::patientReference());
	CHECK_FALSE(fixture.inputEqualsMetric(testData));

	testData.mMetric->setSpace(testData.mSpace);
	CHECK(fixture.inputEqualsMetric(testData));
}

TEST_CASE("ToolMetric can get a valid reference coordinate", "[unit]")
{
	cxtest::MetricFixture fixture;
	cxtest::ToolMetricWithInput testData = fixture.getToolMetricWithInput();

	cx::Vector3D testCoord(-2,1,3);
	cx::Vector3D refCoord = testData.mMetric->getRefCoord();
	INFO(qstring_cast(testCoord)+" == "+qstring_cast(refCoord));
	CHECK(cx::similar(refCoord, testCoord));

	testData.mMetric->setSpace(cx::CoordinateSystem::patientReference());

	refCoord = testData.mMetric->getRefCoord();
	INFO(qstring_cast(testCoord)+" == "+qstring_cast(refCoord));
	CHECK(cx::similar(refCoord, testCoord));

	fixture.setPatientRegistration();

	testCoord = cx::Vector3D(3,7,10);
	refCoord = testData.mMetric->getRefCoord();
	INFO(qstring_cast(testCoord)+" == "+qstring_cast(refCoord));
	CHECK(cx::similar(refCoord, testCoord));
}
