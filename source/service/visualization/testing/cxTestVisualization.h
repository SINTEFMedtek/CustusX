#ifndef CXTESTVISUALIZATION_H
#define CXTESTVISUALIZATION_H

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

/**Unit tests for the Visualization module
 */
class TestVisualization : public CppUnit::TestFixture
{
public:
    void setUp();
    void tearDown();
    TestVisualization();
    void testSortData();

    CPPUNIT_TEST_SUITE( TestVisualization );
        CPPUNIT_TEST( testSortData );
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestVisualization );

#endif // CXTESTVISUALIZATION_H
