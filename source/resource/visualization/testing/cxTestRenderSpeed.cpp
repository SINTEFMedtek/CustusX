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
#include "cxLogger.h"
#include "cxReporter.h"
#include "vtkRenderWindowInteractor.h"
//#include "cxForwardDeclarations.h"

namespace cxtest
{
TestRenderSpeed::TestRenderSpeed():
		mNumViews(0)
{
	cx::reporter()->initialize();
}

TestRenderSpeed::~TestRenderSpeed()
{
	cx::Reporter::shutdown();
}

void TestRenderSpeed::testSingleView()
{
	this->create3Dviews(1);
	this->showViews();
	this->renderNumTimes(100);
}

void TestRenderSpeed::testSeveralViews()
{
	this->create3Dviews(2);
	this->create2Dviews(8);
	this->showViews();
	this->renderNumTimes(100);
}

void TestRenderSpeed::testLotsOfViews()
{
	this->create3Dviews(20);
	this->create2Dviews(80);
	this->showViews();
	this->renderNumTimes(10);
}

void TestRenderSpeed::testVtkRenderWindow()
{
	createVtkRenderWindows(1);
	this->renderNumTimes(100);
}

void TestRenderSpeed::testSeveralVtkRenderWindows()
{
	createVtkRenderWindows(10);
	this->renderNumTimes(100);
}

void TestRenderSpeed::createVtkRenderWindows(int num)
{
	mNumViews += num;
	for(int i = 0; i < num; ++i)
	{
		vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();
		vtkRenderWindowInteractorPtr interactor = vtkRenderWindowInteractorPtr::New();
		interactor->SetRenderWindow(renderWindow);
		mInteractors.push_back(interactor);
	}
}


void TestRenderSpeed::create3Dviews(int num)
{
	mNumViews += num;
	for(int i = 0; i < num; ++i)
	{
		cx::ViewWidget* view = new cx::ViewWidget("testView3D", "testView3D", NULL);
		mViews.push_back(view);
	}
}

void TestRenderSpeed::create2Dviews(int num)
{
	mNumViews += num;
	for(int i = 0; i < num; ++i)
	{
		cx::ViewWidget* view = new cx::ViewWidget("testView2D", "testView2D", NULL);
		mViews.push_back(view);
	}
}

void TestRenderSpeed::showViews()
{
	SSC_ASSERT(!mMainWidget);

	mMainWidget.reset(new QWidget);
	mMainWidget->resize(1000,500);
	QGridLayout* layout = new QGridLayout();
	mMainWidget->setLayout(layout);
	this->addViewsToGridLayout(layout);
	mMainWidget->show();
}

void TestRenderSpeed::renderNumTimes(int num)
{
	mNumRenderings = num;
	QTime clock;
	clock.start();
	for(int i = 0; i < mNumRenderings; ++i)
		for(int v = 0; v < this->getNumViews(); v++)
			this->renderViewNum(v);
	this->setTotalRenderTimeInMs(clock.elapsed());
}

void TestRenderSpeed::renderViewNum(int viewNum)
{
	if(mViews.size() != 0)
		mViews[viewNum]->getRenderWindow()->Render();
	else if(mInteractors.size() != 0)
		mInteractors[viewNum]->GetRenderWindow()->Render();
}

void TestRenderSpeed::printResult()
{
	std::cout << "Render time:\t" << this->getNumViews() << " " << this->getViewName();
	if (this->getNumViews() == 1)
		std::cout << " ";
	else
		std::cout << "s";
	std::cout << "\tTotal: " << this->getTotalRenderTimeInMs() << "ms";
	std::cout << "\tAverage: " << this->getAverageRenderTimeInMs() << "ms";
	std::cout << "\t(FPS=" << this->getRenderFPS() << ")" << std::endl;
}

void TestRenderSpeed::addViewsToLayout(QLayout* layout)
{
	std::vector<cx::ViewWidget*>::iterator iter;
	for (iter = mViews.begin(); iter != mViews.end(); ++iter)
		layout->addWidget(*iter);
}

void TestRenderSpeed::addViewsToGridLayout(QGridLayout* layout)
{
	int squareNumViews = sqrt((double)this->getNumViews());
	for (int i = 0; i < this->getNumViews(); i++)
		layout->addWidget(mViews[i], i / squareNumViews, i % squareNumViews);
}

const char* TestRenderSpeed::getViewName()
{
	if (mViews.size() != 0)
		return "cxView";
	else
		return "vtkRenderWindow";
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
	if (mNumRenderings==0)
		return 1000000;
	double time = mRenderTimeInMs / (double)mNumRenderings;
	return time;
}

int TestRenderSpeed::getRenderFPS()
{
	return 1000 / this->getAverageRenderTimeInMs();
}

int TestRenderSpeed::getNumViews()
{
	return mNumViews;
}

} //namespace cxtest
