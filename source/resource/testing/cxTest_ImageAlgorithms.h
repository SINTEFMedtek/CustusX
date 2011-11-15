#ifndef CXTEST_IMAGEALGORITHMS_H_
#define CXTEST_IMAGEALGORITHMS_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

/**Unit tests that show the properties of the cxToolConfigurationParser class
 */
class TestImageAlgorithms : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testResample();///< Test the resample algorithm

public:
	CPPUNIT_TEST_SUITE( TestImageAlgorithms );
		CPPUNIT_TEST( testResample );
	CPPUNIT_TEST_SUITE_END();
private:

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestImageAlgorithms );


#endif /*CXTEST_IMAGEALGORITHMS_H_*/
