/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTRENDERSPEED_H_
#define CXTESTRENDERSPEED_H_

#include "cxtestresourcevisualization_export.h"

#include "cxView.h"
#include <QTime>
#include "cxViewCollectionWidget.h"

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
