#ifndef CXTESTTUBESEGMENTATIONFRAMEWORK_H_
#define CXTESTTUBESEGMENTATIONFRAMEWORK_H_

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

class QString;
class paramList;

class TestTubeSegmentationFramework : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testConstructor();///< Test constructor

	void testParameters(); ///< Test the default parameters read from file (TSF)
	void testDefaultPreset();
	void testSyntheticVascusynthPreset();
	void testPhantomAccUSPreset();
	void testNeuroVesselsMRAPreset();
	void testNeuroVesselsUSAPreset();
	void testLungAirwaysCTPreset();
	void testLiverVesselsCTPreset();
	void testLiverVesselsMRPreset();

	void testLoadParameterFile(); ///< Test to see if the options(adapters) are correctly fill with information form the parameter file

private:
	paramList loadPreset(QString preset);
	void runFilter(QString preset);

public:
	CPPUNIT_TEST_SUITE( TestTubeSegmentationFramework );
		CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testDefaultPreset );
		CPPUNIT_TEST( testSyntheticVascusynthPreset );
		CPPUNIT_TEST( testPhantomAccUSPreset );
		CPPUNIT_TEST( testNeuroVesselsMRAPreset );
		CPPUNIT_TEST( testNeuroVesselsUSAPreset );
		CPPUNIT_TEST( testLungAirwaysCTPreset );
		//TODO make these work
		CPPUNIT_TEST( testLiverVesselsCTPreset );
		CPPUNIT_TEST( testLiverVesselsMRPreset );
		CPPUNIT_TEST( testParameters );
		CPPUNIT_TEST( testLoadParameterFile );
	CPPUNIT_TEST_SUITE_END();

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestTubeSegmentationFramework );

#endif /* CXTESTTUBESEGMENTATIONFRAMEWORK_H_ */
