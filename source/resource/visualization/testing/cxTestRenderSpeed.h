#ifndef CXTESTRENDERSPEED_H_
#define CXTESTRENDERSPEED_H_

#include "cxView.h"
class QGridLayout;

typedef vtkSmartPointer<class vtkRenderWindow> vtkRenderWindowPtr;
typedef vtkSmartPointer<class vtkRenderer> vtkRendererPtr;
typedef vtkSmartPointer<class vtkRenderWindowInteractor> vtkRenderWindowInteractorPtr;

namespace cxtest
{
/*
 * \class TestRenderSpeed
 *
 * \Brief Helper class for testing view render speed
 *
 *  \date May 27, 2013
 *  \author Ole Vegard Solberg, SINTEF
 */
class TestRenderSpeed
{
public:
	TestRenderSpeed();
	~TestRenderSpeed();
	void testSingleView();
	void testSeveralViews();
	void testLotsOfViews();
	void testVtkRenderWindow();
	void testSeveralVtkRenderWindows();

	void printResult();
	int getRenderFPS();

//private:
    void createVtkRenderWindows(int num);
    void create3Dviews(int num);
    void create2Dviews(int num);
    void showViews();
		void renderNumTimes(int num);
		void renderViewNum(int viewNum);
    void addViewsToLayout(QLayout* layout);
    void addViewsToGridLayout(QGridLayout* layout);
		const char* getViewName();
		void setTotalRenderTimeInMs(int time);
		int getTotalRenderTimeInMs();
		double getAverageRenderTimeInMs();
    int getNumViews();

	std::vector<cx::ViewWidget*> mViews;
//	std::vector<vtkRenderWindowPtr> mVtkRenderWindows;
	boost::shared_ptr<QWidget> mMainWidget;
	int mNumRenderings;
	int mRenderTimeInMs;
	int mNumViews;

	std::vector<vtkRenderWindowInteractorPtr> mInteractors;
};

} //namespace cxtest

#endif /* CXTESTRENDERSPEED_H_ */
