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
//#include "cxtestToolMetricFixture.h"


//TEST_CASE("ToolMetric can set/get tool data", "[unit]")
//{
//    cxtest::MetricFixture fixture;
//    ToolMetricData testData = fixture.getToolMetricData();
////    fixture.createToolMetric(testData);

//    CHECK(fixture.metricEqualsData(testData));
//}

//TEST_CASE("ToolMetric can save/load XML", "[unit]")
//{
//    cxtest::MetricFixture fixture;
//    ToolMetricPtr metric = fixture.createToolMetric();

//    ToolMetricPtr loadedMetric = fixture.saveAndLoadFromXml(metric);

//    CHECK(fixture.isEqualToOriginal(loadedMetric));
//}

//// USE THIS
//TEST_CASE("ToolMetric can save/load XML", "[unit]")
//{
//    cxtest::MetricFixture fixture;
//    ToolMetricData testData = fixture.getToolMetricData();

//    CHECK(fixture.saveLoadXmlGivesEqualTransform(testData));
//}

//// USE THIS TO GENERATE FIXTURE XML LOADSAVE method
//TEST_CASE("ToolMetric can save/load XML", "[unit]")
//{
//    cxtest::MetricFixture fixture;
//    ToolMetricData testData = fixture.getToolMetricData();
//    ToolMetricPtr metric = fixture.createToolMetric(testData);

//    QDomNode xmlNode = fixture.createDummyXmlNode();
//    metric->addXml(xmlNode);

//    ToolMetricPtr loadedMetric = fixture.createFromXml(xmlNode);
//    CHECK(fixture.isEqual(loadedMetric, testData));
//}

//TEST_CASE("ToolMetric can convert values to single line string", "[unit]")
//{
//    cxtest::MetricFixture fixture;
//    ToolMetricData testData = fixture.getToolMetricData();

//    QString metricString = testData.mMetric->getAsSingleLineString();

//    //TODO:  flytt alt inn i egen function i fixture
////    fixture.checkSingleLineStringEquals(metricString, testData);

//    REQUIRE(!metricString.isEmpty());

//    QStringList list = metricString.split("\"reference\"");
//    REQUIRE(list.size()==2);
//    QString headingAndName = list[0];
//    QString maxtrixString = list[1];

//    CHECK(headingAndName == ("%1 \"%2\" ")
//          .arg(testData.mMetric->getType())
//          .arg(testData.mMetric->getName()));

//    bool transformStringOk = false;
//    ssc::Transform3D readTransform = ssc::Transform3D::fromString(matrixString, &transformStringOk);
//    REQUIRE(transformStringOk);
//    REQUIRE(ssc::similar(testData.m_qMt, readTransform));
//}

//TEST_CASE("ToolMetric can set space correctly", "[unit]")
//{
//    cxtest::MetricFixture fixture;
//    ToolMetricData testData = fixture.getToolMetricData();

//    fixture.setPatientRegistration();

//    testData.mMetric->setSpace(ssc::CoordinateSystemHelpers::getPr());
//    CHECK_FALSE(fixture.isEqual(testData.mMetric, testData));

//    testData.mMetric->setSpace(testData.mSpace);
//    CHECK(fixture.isEqual(testData.mMetric, testData));
//}

//TEST_CASE("ToolMetric can get a valid reference coordinate", "[unit]")
//{
//    cxtest::MetricFixture fixture;
//    ToolMetricData testData = fixture.getToolMetricData();

////    ssc::Vector3D testCoord(0,0,0); // TODO insert correct value
////    ssc::Vector3D refCoord = testData.mMetric->getRefCoord();
////    CHECK(ssc::similar(refCoord, testCoord);

//    fixture.setPatientRegistration();
//    testData.mMetric->setSpace(ssc::CoordinateSystemHelpers::getPr());

//    ssc::Vector3D testCoord(0,0,0); // TODO insert correct value
//    ssc::Vector3D refCoord = testData.mMetric->getRefCoord();
//    CHECK(ssc::similar(refCoord, testCoord);
//}
