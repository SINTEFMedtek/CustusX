#ifndef CXTESTPROBE_H_
#define CXTESTPROBE_H_

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "sscTool.h"

namespace cxtest
{

/*
 * \class TestProbe
 *
 * \Brief Unit tests for the cxProbe class
 *
 * \date Jun 3, 2013
 * \author Ole Vegard Solberg, SINTEF
 */
class TestProbe : public CppUnit::TestFixture
{
public:
	TestProbe();
	void setUp();
	void tearDown();
	void testConstructor();
	void testConstructorWithMocXmlParser();

    CPPUNIT_TEST_SUITE( TestProbe );
//        CPPUNIT_TEST( testConstructor );
        CPPUNIT_TEST( testConstructorWithMocXmlParser );
    CPPUNIT_TEST_SUITE_END();
private:
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestProbe );

} //namespace cxtest

#endif /* CXTESTPROBE_H_ */
