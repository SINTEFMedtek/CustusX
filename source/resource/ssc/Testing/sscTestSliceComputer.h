#ifndef SSCTESTSLICECOMPUTER_H_
#define SSCTESTSLICECOMPUTER_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

/**Unit tests for class ssc::SliceComputer
 */
class TestSliceComputer : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();
	void testInitialize();
	void testACS();
	void testACS_FollowTool();
	void testAnyPlanes();
	void testMore();
	
public:
	CPPUNIT_TEST_SUITE( TestSliceComputer );
		CPPUNIT_TEST( testInitialize );			
		CPPUNIT_TEST( testACS );			
		CPPUNIT_TEST( testACS_FollowTool );
		CPPUNIT_TEST( testAnyPlanes );
//		CPPUNIT_TEST( testAnyPlanesWithGravity );
		
	CPPUNIT_TEST_SUITE_END();
private:

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestSliceComputer );


#endif /*SSCTESTSLICECOMPUTER_H_*/
