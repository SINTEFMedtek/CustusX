#ifndef SSCTESTTRANSFORM3D_H_
#define SSCTESTTRANSFORM3D_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "sscTransform3D.h"

/**Tests for Transform3D
 */
class TestTransform3D : public CppUnit::TestFixture
{
public:
//  void setUp();
//  void tearDown();

  void testIdentityTransformHasNoEffectOnVector();
  void testInsertedElementsEqualsAccessedElements();
  void testEqualTransformsAreSimilar();
  void testDifferentTransformsAreNotSimilar();
  void testTranslate();
  void testRotate();
  void testVtkConvert();
  void testStringConvert();
  void testDoTransform();
  void testInverse();
  void testMultiply();

public:
  CPPUNIT_TEST_SUITE( TestTransform3D );
  CPPUNIT_TEST( testIdentityTransformHasNoEffectOnVector);
  CPPUNIT_TEST( testInsertedElementsEqualsAccessedElements);
  CPPUNIT_TEST( testEqualTransformsAreSimilar);
  CPPUNIT_TEST( testDifferentTransformsAreNotSimilar);
  CPPUNIT_TEST( testTranslate);
  CPPUNIT_TEST( testRotate);
  CPPUNIT_TEST( testVtkConvert);
  CPPUNIT_TEST( testStringConvert);
  CPPUNIT_TEST( testDoTransform);
  CPPUNIT_TEST( testInverse);
  CPPUNIT_TEST( testMultiply);
  CPPUNIT_TEST_SUITE_END();
private:
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestTransform3D );


#endif /*SSCTESTTRANSFORM3D_H_*/
