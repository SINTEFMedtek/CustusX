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

	QString stringTransform = fixture.mOriginalMetric->getAsSingleLineString();
	REQUIRE(!stringTransform.isEmpty());
//	std::cout << stringTransform << std::endl;
	REQUIRE(stringTransform == fixture.expectedStringAfterConversion());
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
