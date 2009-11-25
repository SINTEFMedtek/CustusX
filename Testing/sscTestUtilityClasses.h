#ifndef SSCTESTUTILITYCLASSES_H_
#define SSCTESTUTILITYCLASSES_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "sscTransform3D.h"
using ssc::Transform3D;
using ssc::Vector3D;

/**Tests for minor classes in the code/Utilities section
 */
class TestUtilities : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();
	void testTransform3DAccess();
	void testFrame();
	
public:
	CPPUNIT_TEST_SUITE( TestUtilities );
		CPPUNIT_TEST( testTransform3DAccess );					
		CPPUNIT_TEST( testFrame );					
	CPPUNIT_TEST_SUITE_END();
private:
	void singleTestFrameRotationAxis(const Vector3D& k);
	void singleTestFrame(const Transform3D& transform);
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestUtilities );


#endif /*SSCTESTUTILITYCLASSES_H_*/
