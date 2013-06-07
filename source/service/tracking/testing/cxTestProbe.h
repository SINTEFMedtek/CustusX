#ifndef CXTESTPROBE_H_
#define CXTESTPROBE_H_

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "sscTool.h"
#include "cxProbe.h"

namespace cxtest
{

/*
 * \class TestProbe
 *
 * \Brief Unit tests for the cxProbe class
 *
 * \date Jun 3, 2013
 * \author Ole Vegard Solberg, SINTEF
 */
class TestProbe : public CppUnit::TestFixture
{
public:
	TestProbe();
	void setUp();
	void tearDown();
	void testConstructorWithDefaultXmlParser();
	void testConstructorWithMockXmlParser();
	void testDigitalVideoSetting();
	void testRTSource();
	void testDefaultProbeSector();
	void testCustomProbeSector();
	void testSetActiveStreamToCustomProbeSector();
	void testSetActiveStream();
	void runAllTests();

    CPPUNIT_TEST_SUITE( TestProbe );
//        CPPUNIT_TEST( testConstructorWithDefaultXmlParser );
//The tests below are for now called from runAllTests() to save time
//        CPPUNIT_TEST( testConstructorWithMockXmlParser );
//        CPPUNIT_TEST( testDigitalVideoSetting );
//        CPPUNIT_TEST( testRTSource );
//        CPPUNIT_TEST( testDefaultProbeSector );
//        CPPUNIT_TEST( testCustomProbeSector );
//        CPPUNIT_TEST( testSetActiveStreamToCustomProbeSector );
//        CPPUNIT_TEST( testSetActiveStream );
        CPPUNIT_TEST( runAllTests );
    CPPUNIT_TEST_SUITE_END();
private:
    void createTestProbe();
    ssc::ProbeData createProbeData(QString uid);
    cx::ProbePtr mProbe;
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestProbe );

} //namespace cxtest

#endif /* CXTESTPROBE_H_ */
