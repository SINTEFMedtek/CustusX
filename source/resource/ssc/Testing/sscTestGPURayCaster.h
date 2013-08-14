#ifndef SSCTESTGPURAYCASTER_H_
#define SSCTESTGPURAYCASTER_H_

#ifndef WIN32

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "sscGPURayCastVolumeRep.h"
/**Unit tests for class ssc::GPURayCasterVolumeRep
 */
class TestGPURayCaster : public CppUnit::TestFixture
{
public:
	void setUp();
	void testSetImages();
	void testParameters();
	
public:
	CPPUNIT_TEST_SUITE( TestGPURayCaster );
		CPPUNIT_TEST( testSetImages );
		CPPUNIT_TEST( testParameters );
	CPPUNIT_TEST_SUITE_END();
private:
	ssc::GPURayCastVolumeRepPtr mRep;

};

#endif //WIN32
#endif /*SSCTESTGPURAYCASTER_H_*/
