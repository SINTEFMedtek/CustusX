#include <testing/cxTestRenderSpeed.h>

#include <QTime>
//#include <QGridLayout>
#include <QBoxLayout>
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
//	cx::DataLocations::setTestMode();
//	cx::settings()->setValue("renderingInterval", 4);
}

void TestRenderSpeed::tearDown()
{
	// this stuff will be performed just after all tests in this class
}

void TestRenderSpeed::testSingleView()
{
	this->create3Dviews(1);
	this->showViews();
	this->renderNumTimes(100);
	this->printResult();
//	CPPUNIT_ASSERT(this->getTotalRenderTimeInMs() < 2000);
}

void TestRenderSpeed::testSeveralViews()
{
	this->create3Dviews(2);
	this->create2Dviews(8);
	this->showViews();
	this->renderNumTimes(100);
	this->printResult();
//	CPPUNIT_ASSERT(this->getTotalRenderTimeInMs() < 5000);
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
	QWidget* mainWidget = new QWidget;
	QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight, mainWidget);
	mainWidget->setLayout(layout);
	this->addViewsToLayout(layout);
	mainWidget->show();
}

void TestRenderSpeed::renderNumTimes(int num)
{
	mNumRenderings = num;
	QTime clock;
	clock.start();
	std::vector<ssc::ViewWidget*>::iterator iter;
	for(int i = 0; i < mNumRenderings; ++i)
		for (iter = mViews.begin(); iter != mViews.end(); ++iter)
			(*iter)->getRenderWindow()->Render();
	this->setTotalRenderTimeInMs(clock.elapsed());
}

void TestRenderSpeed::printResult()
{
	std::cout << "Render time:\t" << mViews.size();
	if (mViews.size() == 1)
		std::cout << " view. ";
	else
		std::cout << " views.";
	std::cout << "\tTotal: " << this->getTotalRenderTimeInMs() << "ms";
	std::cout << "\tAverage: " << this->getAverageRenderTimeInMs() << "ms";
	std::cout << "\t(FPS=" << this->getRenderFPS() << ")" << std::endl;
}

void TestRenderSpeed::addViewsToLayout(QLayout* layout)
{
	std::vector<ssc::ViewWidget*>::iterator iter;
	for (iter = mViews.begin(); iter != mViews.end(); ++iter)
		layout->addWidget(*iter);
}

int TestRenderSpeed::getTotalRenderTimeInMs()
{
	return mRenderTimeInMs;
}


void TestRenderSpeed::setTotalRenderTimeInMs(int time)
{
	mRenderTimeInMs = time;
}

double TestRenderSpeed::getAverageRenderTimeInMs()
{
	double time = mRenderTimeInMs / (double)mNumRenderings;
	return time;
}

int TestRenderSpeed::getRenderFPS()
{
	return 1000 / this->getAverageRenderTimeInMs();
}

} //namespace cxtest
