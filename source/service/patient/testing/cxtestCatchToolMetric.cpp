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
	cxtest::ToolMetricWithInput data = fixture.getToolMetricWithInput();

	CHECK(fixture.inputEqualsMetric(data));
}

TEST_CASE("ToolMetric can save/load XML", "[unit]")
{
	cxtest::MetricFixture fixture;
	cxtest::ToolMetricWithInput data = fixture.getToolMetricWithInput();

	CHECK(fixture.saveLoadXmlGivesEqualTransform(data));
}

TEST_CASE("ToolMetric can convert values to single line string", "[unit]")
{
	cxtest::MetricFixture fixture;
	cxtest::ToolMetricWithInput testData = fixture.getToolMetricWithInput();

	QStringList list = fixture.getSingleLineDataList(testData.mMetric);
	REQUIRE(fixture.verifySingleLineHeader(list, testData.mMetric));

	REQUIRE(list[2]==testData.mName);
	REQUIRE(list[3].toDouble()==Approx(testData.mOffset));
	REQUIRE(list[4]=="reference");
	INFO(list.join("\n"));
	bool transformStringOk = false;
	cx::Transform3D readTransform = cx::Transform3D::fromString(QStringList(list.mid(5, 16)).join(" "), &transformStringOk);
	REQUIRE(transformStringOk);
	REQUIRE(cx::similar(testData.m_qMt, readTransform));
}

TEST_CASE("ToolMetric can set space correctly", "[unit]")
{
	cxtest::MetricFixture fixture;
	cxtest::ToolMetricWithInput testData = fixture.getToolMetricWithInput();

	fixture.setPatientRegistration();

	testData.mMetric->setSpace(cx::CoordinateSystemHelpers::getPr());
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

	testData.mMetric->setSpace(cx::CoordinateSystemHelpers::getPr());

	refCoord = testData.mMetric->getRefCoord();
	INFO(qstring_cast(testCoord)+" == "+qstring_cast(refCoord));
	CHECK(cx::similar(refCoord, testCoord));

	fixture.setPatientRegistration();

	testCoord = cx::Vector3D(3,7,10);
	refCoord = testData.mMetric->getRefCoord();
	INFO(qstring_cast(testCoord)+" == "+qstring_cast(refCoord));
	CHECK(cx::similar(refCoord, testCoord));
}
