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
#include "cxtestFrameMetricFixture.h"



#include "sscDataManagerImpl.h"
#include "sscDummyToolManager.h"

TEST_CASE("sscDataManagerImpl setup/shutdown works", "[unit]")
{
	ssc::DataManagerImpl::initialize();
	ssc::ToolManager::setInstance(ssc::DummyToolManager::getInstance());

	ssc::ToolManager::shutdown();
	ssc::DummyToolManager::shutdown();
	ssc::DataManagerImpl::shutdown();
	REQUIRE(true);

	ssc::DataManagerImpl::initialize();
	ssc::ToolManager::setInstance(ssc::DummyToolManager::getInstance());

	ssc::ToolManager::shutdown();
	ssc::DummyToolManager::shutdown();
	ssc::DataManagerImpl::shutdown();
	REQUIRE(true);
}

TEST_CASE("cxFrameMetric can get/get transform", "[unit]")
{
	cxtest::FrameMetricFixture fixture;
	REQUIRE(fixture.createAndSetTestTransform());

	ssc::Transform3D returnedTransform = fixture.mOriginalMetric->getFrame();
	REQUIRE(fixture.isEqualTransform(returnedTransform));
	REQUIRE_FALSE(fixture.isEqualTransform(ssc::Transform3D::Identity()));
}

TEST_CASE("cxFrameMetric can save/load XML", "[unit]")
{
	cxtest::FrameMetricFixture fixture;
	REQUIRE(fixture.createAndSetTestTransform());

	QDomNode mXmlNode = fixture.createDummyXmlNode();
	fixture.mOriginalMetric->addXml(mXmlNode);
	ssc::Transform3D returnedTransform = fixture.createFromXml(mXmlNode)->getFrame();
	REQUIRE(fixture.isEqualTransform(returnedTransform));
	REQUIRE_FALSE(fixture.isEqualTransform(ssc::Transform3D::Identity()));
}

TEST_CASE("cxFrameMetric can convert transform to single line string", "[unit]")
{
	cxtest::FrameMetricFixture fixture;
	REQUIRE(fixture.createAndSetTestTransform());

	QString metricString = fixture.mOriginalMetric->getAsSingleLineString();
	REQUIRE(!metricString.isEmpty());

	QStringList list = metricString.split("\"reference\"");
	QString headingAndName = list[0];
	QString maxtrixString = list[1];

	CHECK(headingAndName == "frameMetric \"testMetric\" ");
	REQUIRE(fixture.readTransformFromString(maxtrixString));
	REQUIRE(fixture.isEqualTransform(fixture.mReturnedTransform));
}

TEST_CASE("cxFrameMetric can set space correctly", "[unit]")
{
	cxtest::FrameMetricFixture fixture;
	REQUIRE(fixture.createAndSetTestTransform());

	fixture.setSpaceToOrigial();
	REQUIRE(fixture.isEqualTransform(fixture.mModifiedMetric->getFrame()));

	fixture.setPatientRegistration();
	fixture.changeSpaceToPatientReference();
	REQUIRE_FALSE(fixture.isEqualTransform(fixture.mModifiedMetric->getFrame()));

	fixture.setSpaceToOrigial();

	REQUIRE(fixture.isEqualTransform(fixture.mModifiedMetric->getFrame()));
}
