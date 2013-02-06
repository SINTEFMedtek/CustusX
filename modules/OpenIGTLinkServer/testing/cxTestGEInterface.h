#ifndef CXTESTGEINTERFACE_H_
#define CXTESTGEINTERFACE_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

/**Unit tests that test the acquisition plugin
 */
class TestGEInterface : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testConstructor();///< Test constructor
	void testInit();///< Test init of cxImageSenderGE
	void testGEStreamer();///< Test GEStreamer directly. Don't use cxImageSenderGE

public:
	CPPUNIT_TEST_SUITE( TestGEInterface );
//		CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testInit );
		CPPUNIT_TEST( testGEStreamer );
	CPPUNIT_TEST_SUITE_END();
private:
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestGEInterface );


#endif /*CXTESTGEINTERFACE_H_*/
