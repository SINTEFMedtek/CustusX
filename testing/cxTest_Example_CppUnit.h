#ifndef CXTEST_EXAMPLE_CPPUNIT_H_
#define CXTEST_EXAMPLE_CPPUNIT_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

/**Unit tests that show the properties of CppUnit by example
 */
class cxTestExamples : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testInitialize();
	void testIdentityTransform();
	void testTransformTranslation();
	void testTransformXRotation();
	void testMore();

public:
	CPPUNIT_TEST_SUITE( cxTestExamples );
		CPPUNIT_TEST( testInitialize );			
		CPPUNIT_TEST( testIdentityTransform );
		CPPUNIT_TEST( testTransformTranslation );
		CPPUNIT_TEST( testTransformXRotation );
		CPPUNIT_TEST( testMore );
	CPPUNIT_TEST_SUITE_END();
private:

};
CPPUNIT_TEST_SUITE_REGISTRATION( cxTestExamples );


#endif /*CXTEST_EXAMPLE_CPPUNIT_H_*/
