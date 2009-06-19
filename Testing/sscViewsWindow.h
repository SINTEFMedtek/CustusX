#ifndef SSCTESTSLICEANDTOOLREP_H_
#define SSCTESTSLICEANDTOOLREP_H_
#include <QtGui>
#include <map>
#include "sscView.h"

#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscAcceptanceBoxWidget.h"

//class SingleLayout
//{
//public:
//	ssc::View* mView;
//	ssc::SliceProxyPtr mSlicer;         
//	ssc::SliceRepSWPtr mSliceRep;  ///en vector av slice overlay....
//};


/**Test class  with convenience methods for defining views.
 * Uses the following reps:
 *  - ssc::ToolRep3D
 *  - ssc::SliceRepSW
 *  - ssc::VolumetricRep
 */
class ViewsWindow : public QMainWindow
{
	Q_OBJECT

public:
	ViewsWindow(QString displayText, bool showSliders);
	virtual ~ViewsWindow();
	
	void define3D(const std::string& imageFilename, int r, int c);
	void defineSlice(const std::string& uid, const std::string& imageFilename, ssc::PLANE_TYPE plane, int r, int c);
	bool accepted() const { return mAcceptanceBox->accepted(); }
	
	bool mDumpSpeedData;
	
	void insertView(ssc::View* view, const std::string& uid, const std::string& volume, int r, int c);
private:
	void start(bool showSliders);
	ssc::View* generateSlice(const std::string& uid, ssc::ToolPtr tool, ssc::ImagePtr image, ssc::PLANE_TYPE plane);
	ssc::ImagePtr loadImage(const std::string& imageFilename);
	
	QSlider* mBrightnessSlider;
	QSlider *mContrastSlider;
	
	typedef std::set<ssc::View*> LayoutMap;
	std::set<ssc::View*> mLayouts;
	QGridLayout* mSliceLayout;
	QString mDisplayText;
	
	int mRenderCount;
	int mTotalRender;
	int mTotalOther;
	QTime mLastRenderEnd;
	ssc::AcceptanceBoxWidget* mAcceptanceBox; 
	
	
public slots:
	void updateRender();
	void contrast(int val);
	void brightness(int val);
};



#endif /*SSCTESTSLICEANDTOOLREP_H_*/
