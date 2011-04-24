#ifndef SSCTESTBOUNDINGBOX3D_H_
#define SSCTESTBOUNDINGBOX3D_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "sscBoundingBox3D.h"

/**Tests for BoundingBox3D
 */
class TestBoundingBox3D : public CppUnit::TestFixture
{
public:
//  void setUp();
//  void tearDown();

  void testCreate();
  void testGetters();
  void testSimilar();
  void testCloud();
  void testContains();
  void testStringConvert();

public:
  CPPUNIT_TEST_SUITE( TestBoundingBox3D );
  CPPUNIT_TEST( testCreate);
  CPPUNIT_TEST( testGetters);
  CPPUNIT_TEST( testSimilar);
  CPPUNIT_TEST( testCloud);
  CPPUNIT_TEST( testContains);
  CPPUNIT_TEST( testStringConvert);
  CPPUNIT_TEST_SUITE_END();
private:
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestBoundingBox3D );


#endif /*SSCTESTBOUNDINGBOX3D_H_*/
