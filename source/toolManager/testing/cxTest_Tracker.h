#ifndef CXTEST_TRACKER_H_
#define CXTEST_TRACKER_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>


/**Unit tests that show the properties of the cxTracker class
 */
class TestTracker: public CppUnit::TestFixture
{
public:
  void setUp();
  void tearDown();

  void testConstructor();

public:
CPPUNIT_TEST_SUITE( TestTracker );
    CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST_SUITE_END();
private:

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestTracker );

#endif /*CXTEST_TRACKER_H_*/
