#include "cxTestProbe.h"

#include <QString>
#include "sscMessageManager.h"
#include "ProbeXmlConfigParserMock.h"
#include "sscProbeData.h"
#include "sscTestVideoSource.h"

namespace cxtest
{

TestProbe::TestProbe()
{
}

void TestProbe::setUp()
{
	// this stuff will be performed just before all tests in this class
	ssc::MessageManager::initialize();
	this->createTestProbe();
}

void TestProbe::tearDown()
{
	// this stuff will be performed just after all tests in this class
	ssc::MessageManager::shutdown();
}

//Disabled for now. Test will output a warning. Use the test below with the Mock XmlParser instead
void TestProbe::testConstructorWithDefaultXmlParser()
{
	mProbe = cx::Probe::New("TestProbe", "TestScanner");

	CPPUNIT_ASSERT(mProbe);
	CPPUNIT_ASSERT_MESSAGE("Probe's config id is not empty. It should be since the test probe setup is not present in the config id list",
			mProbe->getConfigId().isEmpty());
	CPPUNIT_ASSERT_MESSAGE("Test probe is valid. It should not be.",
			!mProbe->isValid());
}

void TestProbe::testConstructorWithMockXmlParser()
{
	CPPUNIT_ASSERT(mProbe);
	CPPUNIT_ASSERT(!mProbe->getConfigId().isEmpty());
	CPPUNIT_ASSERT(mProbe->getConfigId().compare("TestScanner TestProbe DummySource") == 0);
	CPPUNIT_ASSERT(mProbe->isValid());
}

void TestProbe::testDigitalVideoSetting()
{
	CPPUNIT_ASSERT(!mProbe->isUsingDigitalVideo());
	CPPUNIT_ASSERT(mProbe->getRtSourceName().compare("DummySource") == 0);
	mProbe->useDigitalVideo(true);
	CPPUNIT_ASSERT(mProbe->isUsingDigitalVideo());
	CPPUNIT_ASSERT(mProbe->getRtSourceName().compare("Digital") == 0);
	CPPUNIT_ASSERT(mProbe->getConfigId().compare("Digital") == 0);
}

void TestProbe::testRTSource()
{
	CPPUNIT_ASSERT(!mProbe->getRTSource());
	ssc::TestVideoSourcePtr videoSource(new ssc::TestVideoSource("TestVideoSourceUid", "TestVideoSource" , 80, 40));
	mProbe->setRTSource(videoSource);
	CPPUNIT_ASSERT(mProbe->getRTSource());
	CPPUNIT_ASSERT(mProbe->getRTSource()->getUid().compare("TestVideoSourceUid") == 0);
	CPPUNIT_ASSERT(mProbe->getRTSource()->getName().compare("TestVideoSource") == 0);
}

void TestProbe::testDefaultProbeSector()
{
	CPPUNIT_ASSERT(mProbe->getSector());
	CPPUNIT_ASSERT(mProbe->getProbeData().getUid().compare("default") == 0);
	CPPUNIT_ASSERT(ssc::similar(mProbe->getProbeData().getTemporalCalibration(), 0));
}

void TestProbe::testCustomProbeSector()
{
	mProbe->setProbeSector(this->createProbeData("TestProbeData"));
	CPPUNIT_ASSERT(mProbe->getSector("TestProbeData"));
	CPPUNIT_ASSERT(mProbe->getProbeData("TestProbeData").getUid().compare("TestProbeData") == 0);
	CPPUNIT_ASSERT(ssc::similar(mProbe->getProbeData("TestProbeData").getTemporalCalibration(), 1000.5));

}

void TestProbe::testSetActiveStreamToCustomProbeSector()
{
	mProbe->setProbeSector(this->createProbeData("TestProbeData"));
	CPPUNIT_ASSERT(mProbe->getProbeData().getUid().compare("default") == 0);

	mProbe->setActiveStream("TestProbeData");
	CPPUNIT_ASSERT(mProbe->getProbeData().getUid().compare("TestProbeData") == 0);
	CPPUNIT_ASSERT(ssc::similar(mProbe->getProbeData().getTemporalCalibration(), 1000.5));

	mProbe->setActiveStream("default");
	this->testDefaultProbeSector();
}

void TestProbe::testSetActiveStream()
{
	CPPUNIT_ASSERT(mProbe->getActiveStream().compare("default") == 0); //Name set to "default" in ssc::ProbeData()
	mProbe->setActiveStream("TestStream");
	CPPUNIT_ASSERT(mProbe->getActiveStream().compare("TestStream") == 0);
	CPPUNIT_ASSERT(mProbe->getProbeData().getUid().compare("TestStream") == 0);
}

//The setup time of each test is 1.35 sec, so merging 7 tests into 1 saves 8 sec
//TODO: Change later if setup time can be reduced (by using Catch?)
void TestProbe::runAllTests()
{
	this->testConstructorWithMockXmlParser();
	this->testDigitalVideoSetting();
	this->testRTSource();
	this->testDefaultProbeSector();
	this->testCustomProbeSector();
	this->testSetActiveStreamToCustomProbeSector();
	this->testSetActiveStream();
}

void TestProbe::createTestProbe()
{
	QString xmlFileName = "testXmlFileName";
	ProbeXmlConfigParserPtr mXml;
	mXml.reset(new ProbeXmlConfigParserMock(xmlFileName));
	mProbe = cx::Probe::New("TestProbe", "TestScanner", mXml);
}

ssc::ProbeData TestProbe::createProbeData(QString uid)
{
	ssc::ProbeData probeData = mProbe->getProbeData();
	probeData.setUid(uid);
	probeData.setTemporalCalibration(1000.5);
	return probeData;
}

} //namespace cxtest
