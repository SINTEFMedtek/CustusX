#ifndef CXTEST_TOOLCONFIGURATIONPARSER_H_
#define CXTEST_TOOLCONFIGURATIONPARSER_H_s

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

/**Unit tests that show the properties of the cxToolConfigurationParser class
 */
class TestToolConfigurationParser : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testInitialize();

public:
	CPPUNIT_TEST_SUITE( TestToolConfigurationParser );
		CPPUNIT_TEST( testInitialize );			
	CPPUNIT_TEST_SUITE_END();
private:

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestToolConfigurationParser );


#endif /*CXTEST_TOOLCONFIGURATIONPARSER_H_*/
