#ifndef CXTEST_TOOL_H_
#define CXTEST_TOOL_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "cxTool.h"


/**Unit tests that show the properties of the cxTracker class
 */
class TestTool: public CppUnit::TestFixture
{
public:
  void setUp();
  void tearDown();

  void testConstructor();

public:
CPPUNIT_TEST_SUITE( TestTool );
    CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST_SUITE_END();
private:
  cx::Tool::InternalStructure mValidPolarisInternal;
  cx::Tool::InternalStructure mValidAuroraInternal;

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestTool );

#endif /*CXTEST_TOOL_H_*/
