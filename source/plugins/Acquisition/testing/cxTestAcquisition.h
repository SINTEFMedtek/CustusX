#ifndef CXTESTACQUISITION_H_
#define CXTESTACQUISITION_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include "sscReconstructManager.h"

/**Unit tests that test the acquisition plugin
 */
class TestAcquisition : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testConstructor();///< Test constructor
	void testStoreMHDSource();

public:
	CPPUNIT_TEST_SUITE( TestAcquisition );
		CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testStoreMHDSource );
	CPPUNIT_TEST_SUITE_END();
private:
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestAcquisition );


#endif /*CXTESTACQUISITION_H_*/
