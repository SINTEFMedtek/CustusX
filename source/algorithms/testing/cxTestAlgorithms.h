#ifndef CXTEST_ALGORITHMS_H_
#define CXTEST_ALGORITHMS_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

/**Unit tests for cx algorithms
 */
class TestAlgorithms : public CppUnit::TestFixture
{
public:
//	void setUp();
//	void tearDown();

	void testTemporalCalibration();

public:
	CPPUNIT_TEST_SUITE( TestAlgorithms );
		CPPUNIT_TEST( testTemporalCalibration );
	CPPUNIT_TEST_SUITE_END();
private:

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestAlgorithms );


#endif /*CXTEST_ALGORITHMS_H_*/
