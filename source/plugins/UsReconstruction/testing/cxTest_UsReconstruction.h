#ifndef CXTEST_USRECONSTRUCTION_H_
#define CXTEST_USRECONSTRUCTION_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include "sscReconstructManager.h"

/**Unit tests that test the US reconstruction plugin
 */
class TestUsReconstruction : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testSlerpInterpolation();///< Test position matrix slerp interpolation
	void testConstructor();///< Test reconstructer constructor
	void testAngioReconstruction();///< Test reconstruction of US angio data (#318)
	void testThunderGPUReconstruction();///< Test Thunder GPU reconstruction
	void testDualAngio();

public:
	CPPUNIT_TEST_SUITE( TestUsReconstruction );
		CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testSlerpInterpolation );
		CPPUNIT_TEST( testAngioReconstruction );
		CPPUNIT_TEST( testThunderGPUReconstruction );
		CPPUNIT_TEST( testDualAngio );
	CPPUNIT_TEST_SUITE_END();
private:
	ssc::ReconstructManagerPtr createManager();
	void validateData(ssc::ImagePtr output);
	/** Validate the bmode data output from the specific data set used.
	  */
	void validateBModeData(ssc::ImagePtr bmodeOut);
	/** Validate the angio data output from the specific data set used.
	  */
	void validateAngioData(ssc::ImagePtr angioOut);
	int getValue(ssc::ImagePtr data, int x, int y, int z);

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestUsReconstruction );


#endif /*CXTEST_USRECONSTRUCTION_H_*/
