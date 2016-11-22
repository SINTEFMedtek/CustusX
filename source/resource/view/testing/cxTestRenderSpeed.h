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

#ifndef CXTESTRENDERSPEED_H_
#define CXTESTRENDERSPEED_H_

#include "cxtestresourcevisualization_export.h"

#include "cxView.h"
#include <QTime>
#include "cxViewCollectionWidget.h"
#include "cxViewServiceNull.h"

class QGridLayout;
class QLayout;

typedef vtkSmartPointer<class vtkRenderWindow> vtkRenderWindowPtr;
typedef vtkSmartPointer<class vtkRenderer> vtkRendererPtr;
typedef vtkSmartPointer<class vtkRenderWindowInteractor> vtkRenderWindowInteractorPtr;

namespace cx
{
typedef boost::shared_ptr<class RenderWindowFactory> RenderWindowFactoryPtr;
}

namespace cxtest
{
typedef boost::shared_ptr<class ViewServiceMocWithRenderWindowFactory> ViewServiceMocWithRenderWindowFactoryPtr;

class CXTESTRESOURCEVISUALIZATION_EXPORT ViewServiceMocWithRenderWindowFactory : public cx::ViewServiceNull
{
public:
	ViewServiceMocWithRenderWindowFactory();
	virtual vtkRenderWindowPtr getRenderWindow(QString uid, bool offScreenRendering = true);
	virtual vtkRenderWindowPtr getSharedRenderWindow() const;
private:
	cx::RenderWindowFactoryPtr mRenderWindowFactory;
};


class CXTESTRESOURCEVISUALIZATION_EXPORT RenderSpeedCounter
{
public:
	RenderSpeedCounter();

	void setName(QString name) { mName = name; }
	void printResult();
	int getRenderFPS();

	void startRender(int numRender, int numViews);
	void stopRender();

private:
	int getTotalRenderTimeInMs();
	void setTotalRenderTimeInMs(int time);
	double getAverageRenderTimeInMs();

	int mNumRenderings;
	int mNumViews;
	int mRenderTimeInMs;
	QTime mClock;
	QString mName;

};

/*
 * \class TestRenderSpeed
 *
 * \Brief Helper class for testing view render speed
 *
 *  \date May 27, 2013
 *  \author Ole Vegard Solberg, SINTEF
 */
class CXTESTRESOURCEVISUALIZATION_EXPORT TestRenderSpeed
{
public:
	TestRenderSpeed();
	~TestRenderSpeed();
	void testSingleView();
	void testSeveralViews();
	void testLotsOfViews();
	int getRenderFPS() { return mCounter.getRenderFPS(); }

	void showViews();
	void renderNumTimes(int num);
	void createViews(int num);

	std::vector<cx::ViewPtr> mViews;
	boost::shared_ptr<cx::ViewCollectionWidget> mMainWidget;
	RenderSpeedCounter mCounter;
};

/*
 * \class TestRenderSpeed
 *
 * \Brief Helper class for testing view render speed
 *
 *  \date May 27, 2013
 *  \author Ole Vegard Solberg, SINTEF
 */
class CXTESTRESOURCEVISUALIZATION_EXPORT TestRenderWindowSpeed
{
public:
	TestRenderWindowSpeed();
	~TestRenderWindowSpeed();
	void testVtkRenderWindow();
	void testSeveralVtkRenderWindows();

	void createVtkRenderWindows(int num);
	void showViews();
	void renderNumTimes(int num);
	int getRenderFPS() { return mCounter.getRenderFPS(); }

	RenderSpeedCounter mCounter;
	std::vector<vtkRenderWindowInteractorPtr> mInteractors;
};

} //namespace cxtest

#endif /* CXTESTRENDERSPEED_H_ */
