/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include <testing/cxTestRenderSpeed.h>

#include <QTime>
#include <QGridLayout>
#include <QBoxLayout>
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "cxDataLocations.h"
#include "cxSettings.h"
#include "cxReporter.h"
#include "vtkRenderWindowInteractor.h"
#include "cxTypeConversions.h"
#include "catch.hpp"
#include "cxRenderWindowFactory.h"

namespace cxtest
{


ViewServiceMocWithRenderWindowFactory::ViewServiceMocWithRenderWindowFactory()
{
	mRenderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
}

vtkRenderWindowPtr ViewServiceMocWithRenderWindowFactory::getRenderWindow(QString uid, bool offScreenRendering)
{
	return mRenderWindowFactory->getRenderWindow(uid, offScreenRendering);
}

vtkRenderWindowPtr ViewServiceMocWithRenderWindowFactory::getSharedRenderWindow() const
{
	return mRenderWindowFactory->getSharedRenderWindow();
}

///--------------------------------------------------------

RenderSpeedCounter::RenderSpeedCounter() :
	mNumViews(0),
	mNumRenderings(0)
{
}

void RenderSpeedCounter::printResult()
{
	std::cout << "Render time:\t" << mNumViews << " " << mName;
	if (mNumViews == 1)
		std::cout << " ";
	else
		std::cout << "s";
	std::cout << "\tTotal: " << this->getTotalRenderTimeInMs() << "ms";
	std::cout << "\tAverage: " << this->getAverageRenderTimeInMs() << "ms";
	std::cout << "\t(FPS=" << this->getRenderFPS() << ")" << std::endl;
}

void RenderSpeedCounter::startRender(int numRender, int numViews)
{
	mNumRenderings = numRender;
	mNumViews = numViews;
	mClock.start();
}
void RenderSpeedCounter::stopRender()
{
	this->setTotalRenderTimeInMs(mClock.elapsed());
}

int RenderSpeedCounter::getTotalRenderTimeInMs()
{
	return mRenderTimeInMs;
}


void RenderSpeedCounter::setTotalRenderTimeInMs(int time)
{
	mRenderTimeInMs = time;
}

double RenderSpeedCounter::getAverageRenderTimeInMs()
{
	if (mNumRenderings==0)
		return 1000000;
	double time = mRenderTimeInMs / (double)mNumRenderings;
	return time;
}

int RenderSpeedCounter::getRenderFPS()
{
	return 1000 / this->getAverageRenderTimeInMs();
}



///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


TestRenderSpeed::TestRenderSpeed()
{
//	ViewServiceMocWithRenderWindowFactoryPtr viewService = ViewServiceMocWithRenderWindowFactoryPtr(new ViewServiceMocWithRenderWindowFactory());
	cx::ViewServicePtr viewService = cx::ViewServicePtr(new ViewServiceMocWithRenderWindowFactory());
	mCounter.setName("cxView");
	cx::reporter()->initialize();
	mMainWidget.reset(cx::ViewCollectionWidget::createOptimizedLayout(viewService).data());
//	mMainWidget.reset(cx::ViewCollectionWidget::createViewWidgetLayout().data());
}

TestRenderSpeed::~TestRenderSpeed()
{
	cx::Reporter::shutdown();
}

void TestRenderSpeed::testSingleView()
{
	this->createViews(1);
	this->showViews();
	this->renderNumTimes(100);
}

void TestRenderSpeed::testSeveralViews()
{
	this->createViews(10);
	this->showViews();
	this->renderNumTimes(100);
}

void TestRenderSpeed::testLotsOfViews()
{
	this->createViews(100);
	this->showViews();
	this->renderNumTimes(10);
}

void TestRenderSpeed::createViews(int num)
{
	for(int i = 0; i < num; ++i)
	{
		int v = num;
		int rmax = sqrt(v);
		cx::LayoutRegion region(v%rmax, v/rmax);
		cx::ViewPtr view = mMainWidget->addView(cx::View::VIEW, region);

		mViews.push_back(view);
	}
}

void TestRenderSpeed::showViews()
{
	REQUIRE(mMainWidget);

	mMainWidget->resize(1000,500);
	mMainWidget->show();
}

void TestRenderSpeed::renderNumTimes(int num)
{
	mCounter.startRender(num, mViews.size());
	for(int i = 0; i < num; ++i)
	{
		for(int v = 0; v < mViews.size(); v++)
			mViews[v]->setModified();
		mMainWidget->render();
	}

	mCounter.stopRender();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------



TestRenderWindowSpeed::TestRenderWindowSpeed()
{
	mCounter.setName("vtkRenderWindow");
	cx::reporter()->initialize();
}

TestRenderWindowSpeed::~TestRenderWindowSpeed()
{
	cx::Reporter::shutdown();
}

void TestRenderWindowSpeed::testVtkRenderWindow()
{
	createVtkRenderWindows(1);
	this->renderNumTimes(100);
}

void TestRenderWindowSpeed::testSeveralVtkRenderWindows()
{
	createVtkRenderWindows(10);
	this->renderNumTimes(100);
}

void TestRenderWindowSpeed::createVtkRenderWindows(int num)
{
	for(int i = 0; i < num; ++i)
	{
		vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();
		vtkRenderWindowInteractorPtr interactor = vtkRenderWindowInteractorPtr::New();
		interactor->SetRenderWindow(renderWindow);
		mInteractors.push_back(interactor);
	}
}

void TestRenderWindowSpeed::renderNumTimes(int num)
{
	mCounter.startRender(num, mInteractors.size());
	for(int i = 0; i < num; ++i)
		for(int v = 0; v < mInteractors.size(); v++)
			mInteractors[v]->GetRenderWindow()->Render();
	mCounter.stopRender();
}

} //namespace cxtest
