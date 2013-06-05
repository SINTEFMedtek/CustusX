#include "cxTestProbe.h"

#include "sscMessageManager.h"
#include "cxProbe.h"
#include "ProbeXmlConfigParserMoc.h"

#include "sscProbeData.h"

namespace cxtest
{

TestProbe::TestProbe()
{
}

void TestProbe::setUp()
{
	// this stuff will be performed just before all tests in this class
	ssc::MessageManager::initialize();
}

void TestProbe::tearDown()
{
	// this stuff will be performed just after all tests in this class
	ssc::MessageManager::shutdown();
}

void TestProbe::testConstructor()
{
	cx::ProbePtr probe = cx::Probe::New("TestProbe", "TestScanner");

	CPPUNIT_ASSERT(probe);
	CPPUNIT_ASSERT_MESSAGE("Probe's config id is not empty. It should be since the test probe setup is not present in the config id list",
			probe->getConfigId().isEmpty());
	CPPUNIT_ASSERT_MESSAGE("Test probe is valid. It should not be.",
			!probe->isValid());
}

void TestProbe::testConstructorWithMocXmlParser()
{
	QString xmlFileName = "testXmlFileName";
	ProbeXmlConfigParserPtr mXml;
	mXml.reset(new ProbeXmlConfigParserMoc(xmlFileName));

	cx::ProbePtr probe = cx::Probe::New("TestProbe", "TestScanner", mXml);
	CPPUNIT_ASSERT(probe);
	CPPUNIT_ASSERT(!probe->getConfigId().isEmpty());
	CPPUNIT_ASSERT(probe->getConfigId().compare("TestScanner TestProbe DummySource") == 0);
	CPPUNIT_ASSERT(probe->isValid());
	CPPUNIT_ASSERT(probe->getSector());
//	CPPUNIT_ASSERT(probe->getData());
//	CPPUNIT_ASSERT(probe->getRTSource());

	CPPUNIT_ASSERT(!probe->isUsingDigitalVideo());
	probe->useDigitalVideo(true);
	CPPUNIT_ASSERT(probe->isUsingDigitalVideo());
	CPPUNIT_ASSERT(probe->getRtSourceName().compare("Digital") == 0);
//	CPPUNIT_ASSERT(probe->getConfigId().compare("Digital") == 0);
}

} //namespace cxtest
