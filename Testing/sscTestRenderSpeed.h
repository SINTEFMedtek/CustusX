/*
 * sscTestRenderSpeed.h
 *
 *  Created on: Apr 23, 2009
 *      Author: christian
 */

#ifndef SSCTESTRENDERSPEED_H_
#define SSCTESTRENDERSPEED_H_

#include <QtGui>
#include <map>
#include "sscView.h"

#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"

class SingleLayout
{
public:
	ssc::View* mView;
	ssc::SliceProxyPtr mSlicer;
	ssc::SliceRepSWPtr mSliceRep;  ///en vector av slice overlay....
};

class TestSliceAndToolRep : public QMainWindow
{
	Q_OBJECT

public:
	TestSliceAndToolRep();
	virtual ~TestSliceAndToolRep();
private:
	QTime mLastRenderEnd;
	int mRenderCount;
	int mTotalRender;
	int mTotalOther;

	QWidget* mWidget;
	void start();
	ssc::View* view(const std::string& uid);
	void generateSlice(const std::string& uid, ssc::ToolPtr tool, ssc::ImagePtr image, ssc::PLANE_TYPE plane);
	void generateView(const std::string& uid);
	void defineSlice(const std::string& uid, QGridLayout* sliceLayout, ssc::ToolPtr tool, ssc::ImagePtr image, ssc::PLANE_TYPE plane, int r, int c);
	QSlider* mBrightnessSlider;
	QSlider *mContrastSlider;
	std::string imageFileName1;

	typedef std::map<std::string, SingleLayout> LayoutMap;
	LayoutMap mLayouts;

private slots:
	void updateRender();
	void contrast(int val);
	void brightness(int val);
};



#endif /* SSCTESTRENDERSPEED_H_ */
