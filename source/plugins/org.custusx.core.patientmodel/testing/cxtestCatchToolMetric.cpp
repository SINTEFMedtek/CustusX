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

	REQUIRE(list[2]=="reference");
	REQUIRE(list[3]==testData.mName);
	REQUIRE(list[4].toDouble()==Approx(testData.mOffset));
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
