#ifndef CXTESTTUBESEGMENTATIONFRAMEWORK_H_
#define CXTESTTUBESEGMENTATIONFRAMEWORK_H_

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

class TestTubeSegmentationFramework : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testConstructor();///< Test constructor

	void testFilter(); ///< Test TubeSegmentationFilter
	void testParameters(); ///< Test the default parameters read from file (TSF)

	void testLoadParameterFile(); ///< Test to se if the options(adapters) are correctly fill with information form the parameter file

public:
	CPPUNIT_TEST_SUITE( TestTubeSegmentationFramework );
		CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testFilter );
		CPPUNIT_TEST( testParameters );
		CPPUNIT_TEST( testLoadParameterFile );
	CPPUNIT_TEST_SUITE_END();

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestTubeSegmentationFramework );

#endif /* CXTESTTUBESEGMENTATIONFRAMEWORK_H_ */
