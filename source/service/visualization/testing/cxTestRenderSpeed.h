#ifndef CXTESTRENDERSPEED_H_
#define CXTESTRENDERSPEED_H_

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "sscView.h"
class QGridLayout;

typedef vtkSmartPointer<class vtkRenderWindow> vtkRenderWindowPtr;

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
	void testLotsOfViews();
	void testVtkRenderWindow();
	void testSeveralVtkRenderWindows();

    CPPUNIT_TEST_SUITE( TestRenderSpeed );
        CPPUNIT_TEST( testSingleView );
        CPPUNIT_TEST( testSeveralViews );
//        CPPUNIT_TEST( testLotsOfViews );
        CPPUNIT_TEST( testVtkRenderWindow );
        CPPUNIT_TEST( testSeveralVtkRenderWindows );
    CPPUNIT_TEST_SUITE_END();

private:
    void createVtkRenderWindows(int num);
    void create3Dviews(int num);
    void create2Dviews(int num);
    void showViews();
    void renderNumTimes(int num);
    void renderViewNum(int viewNum);
    void printResult();
    void addViewsToLayout(QLayout* layout);
    void addViewsToGridLayout(QGridLayout* layout);
    const char* getViewName();
    int getTotalRenderTimeInMs();
    void setTotalRenderTimeInMs(int time);
    double getAverageRenderTimeInMs();
    int getRenderFPS();
    int getNumViews();

	std::vector<ssc::ViewWidget*> mViews;
	std::vector<vtkRenderWindowPtr> mVtkRenderWindows;
	int mNumRenderings;
	int mRenderTimeInMs;
	int mNumViews;
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestRenderSpeed );

} //namespace cxtest

#endif /* CXTESTRENDERSPEED_H_ */
