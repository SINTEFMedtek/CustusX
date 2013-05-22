#ifndef CXTEST_SPEED_H_
#define CXTEST_SPEED_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

/**Unit tests test the entire CustusX application for
 * render speed (and similar).
 */
class cxTestSpeed : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

//	void testKaisaSpeed();
//	void testLapSpeed();
	void testKaisaMetricsSpeed();

public:
	CPPUNIT_TEST_SUITE( cxTestSpeed );
		CPPUNIT_TEST( testKaisaMetricsSpeed );
//		CPPUNIT_TEST( testKaisaMetricsSpeed );
//		CPPUNIT_TEST( testLapSpeed );
	CPPUNIT_TEST_SUITE_END();
private:
	int mPreviousRenderingInterval;
};
CPPUNIT_TEST_SUITE_REGISTRATION( cxTestSpeed );


#endif /*CXTEST_SPEED_H_*/
