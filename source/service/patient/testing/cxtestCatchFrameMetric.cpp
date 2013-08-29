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
    cxtest::FrameMetricData testData = fixture.getFrameMetricData();
    CHECK(fixture.metricEqualsData(testData));

//	cxtest::FrameMetricFixture fixture;
//	REQUIRE(fixture.createAndSetTestTransform());

//	ssc::Transform3D returnedTransform = fixture.mOriginalMetric->getFrame();
//	REQUIRE(fixture.isEqualTransform(returnedTransform));
//	REQUIRE_FALSE(fixture.isEqualTransform(ssc::Transform3D::Identity()));
}

TEST_CASE("cxFrameMetric can save/load XML", "[unit]")
{
    cxtest::MetricFixture fixture;
    cxtest::FrameMetricData testData = fixture.getFrameMetricData();

    CHECK(fixture.saveLoadXmlGivesEqualTransform(testData));

//    cxtest::MetricFixture fixture;
//    FrameMetricData testData = fixture.getFrameMetricData();
//    FramMetricPtr metric = fixture.createFrameMetric(testData);

//    QDomNode xmlNode = fixture.createDummyXmlNode();
//    metric->addXml(xmlNode);

//    FramMetricPtr loadedMetric = fixture.createFromXml(xmlNode);
//    CHECK(fixture.isEqual(loadedMetric, testData));

//    cxtest::FrameMetricFixture fixture;
//	REQUIRE(fixture.createAndSetTestTransform());

//	QDomNode mXmlNode = fixture.createDummyXmlNode();
//	fixture.mOriginalMetric->addXml(mXmlNode);
//	ssc::Transform3D returnedTransform = fixture.createFromXml(mXmlNode)->getFrame();
//	REQUIRE(fixture.isEqualTransform(returnedTransform));
//	REQUIRE_FALSE(fixture.isEqualTransform(ssc::Transform3D::Identity()));
}

TEST_CASE("cxFrameMetric can convert transform to single line string", "[unit]")
{
    cxtest::MetricFixture fixture;
    cxtest::FrameMetricData testData = fixture.getFrameMetricData();

    QString metricString = testData.mMetric->getAsSingleLineString();

    //TODO:  flytt alt inn i egen function i fixture
//    fixture.checkSingleLineStringEquals(metricString, testData);

    REQUIRE(!metricString.isEmpty());

    QStringList list = metricString.split("\"reference\"");
    REQUIRE(list.size()==2);
    QString headingAndName = list[0];
    QString valueString = list[1];

    CHECK(headingAndName == QString("%1 \"%2\" ")
          .arg(testData.mMetric->getType())
          .arg(testData.mMetric->getName()));

    bool transformStringOk = false;
    ssc::Transform3D readTransform = ssc::Transform3D::fromString(valueString, &transformStringOk);
    REQUIRE(transformStringOk);
    REQUIRE(ssc::similar(testData.m_qMt, readTransform));
}

TEST_CASE("cxFrameMetric can set space correctly", "[unit]")
{
    cxtest::MetricFixture fixture;
    cxtest::FrameMetricData testData = fixture.getFrameMetricData();

    fixture.setPatientRegistration();

    testData.mMetric->setSpace(ssc::CoordinateSystemHelpers::getPr());
    CHECK_FALSE(fixture.metricEqualsData(testData));

    testData.mMetric->setSpace(testData.mSpace);
    CHECK(fixture.metricEqualsData(testData));


//	cxtest::FrameMetricFixture fixture;
//	REQUIRE(fixture.createAndSetTestTransform());

//	fixture.setSpaceToOrigial();
//	REQUIRE(fixture.isEqualTransform(fixture.mModifiedMetric->getFrame()));

//	fixture.setPatientRegistration();
//	fixture.changeSpaceToPatientReference();
//	REQUIRE_FALSE(fixture.isEqualTransform(fixture.mModifiedMetric->getFrame()));

//	fixture.setSpaceToOrigial();

//	REQUIRE(fixture.isEqualTransform(fixture.mModifiedMetric->getFrame()));
}
