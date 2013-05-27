#include <testing/cxTestRenderSpeed.h>

#include <QTime>
#include "vtkRenderWindow.h"
#include "cxDataLocations.h"
#include "cxSettings.h"
//#include "cxForwardDeclarations.h"
#include "cxView3D.h"
#include "cxView2D.h"

namespace cxtest
{
TestRenderSpeed::TestRenderSpeed()
{
}

void TestRenderSpeed::setUp()
{
	// this stuff will be performed just before all tests in this class
	cx::DataLocations::setTestMode();
	cx::settings()->setValue("renderingInterval", 4);
}

void TestRenderSpeed::tearDown()
{
	// this stuff will be performed just after all tests in this class
}

void TestRenderSpeed::testSingleView()
{
	this->create3Dviews(1);
	showViews();
	std::cout << "render time 1 view: " << this->renderTimeInMs() << "ms" << std::endl;
}

void TestRenderSpeed::testSeveralViews()
{
	this->create3Dviews(2);
	this->create2Dviews(8);
	showViews();
	std::cout << "render time 10 views : " << this->renderTimeInMs() << "ms" << std::endl;
}

int TestRenderSpeed::renderTimeInMs()
{
	QTime clock;
	clock.start();
	std::vector<ssc::ViewWidget*>::iterator iter;
	for(int i = 0; i < 100; ++i)
		for (iter = mViews.begin(); iter != mViews.end(); ++iter)
			(*iter)->getRenderWindow()->Render();
	return clock.elapsed();
}

void TestRenderSpeed::create3Dviews(int num)
{
	for(int i = 0; i < num; ++i)
	{
		cx::View3D* view = new cx::View3D("testView3D", "testView3D", NULL);
		mViews.push_back(view);
	}
}

void TestRenderSpeed::create2Dviews(int num)
{
	for(int i = 0; i < num; ++i)
	{
		cx::View2D* view = new cx::View2D("testView2D", "testView2D", NULL);
		mViews.push_back(view);
	}
}

void TestRenderSpeed::showViews()
{
	std::vector<ssc::ViewWidget*>::iterator iter;
	for (iter = mViews.begin(); iter != mViews.end(); ++iter)
		(*iter)->show();
}

} //namespace cxtest
