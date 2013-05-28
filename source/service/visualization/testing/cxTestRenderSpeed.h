#ifndef CXTESTRENDERSPEED_H_
#define CXTESTRENDERSPEED_H_

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "sscView.h"

namespace cxtest
{
/*
 * \class TestRenderSpeed
 *
 * \Brief Class for testing view render speed
 *
 *  \date May 27, 2013
 *  \author Ole Vegard Solberg, SINTEF
 */
class TestRenderSpeed : public CppUnit::TestFixture
{
public:
    void setUp();
    void tearDown();
	TestRenderSpeed();
	void testSingleView();
	void testSeveralViews();

    CPPUNIT_TEST_SUITE( TestRenderSpeed );
        CPPUNIT_TEST( testSingleView );
        CPPUNIT_TEST( testSeveralViews );
    CPPUNIT_TEST_SUITE_END();

private:
    void create3Dviews(int num);
    void create2Dviews(int num);
    void showViews();
    void renderNumTimes(int num);
    void printResult();
    void addViewsToLayout(QLayout* layout);
    int getTotalRenderTimeInMs();
    void setTotalRenderTimeInMs(int time);
    double getAverageRenderTimeInMs();
    int getRenderFPS();

	std::vector<ssc::ViewWidget*> mViews;
	int mNumRenderings;
	int mRenderTimeInMs;
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestRenderSpeed );

} //namespace cxtest

#endif /* CXTESTRENDERSPEED_H_ */
