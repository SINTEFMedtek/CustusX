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
	
	void setDescription(const QString& desc);
	void define3D(const QString& imageFilename, int r, int c);
	void defineSlice(const QString& uid, const QString& imageFilename, ssc::PLANE_TYPE plane, int r, int c);
	void defineGPUSlice(const QString& uid, const QString& imageFilename, ssc::PLANE_TYPE plane, int r, int c);
	bool accepted() const { return mAcceptanceBox->accepted(); }
	
	bool mDumpSpeedData;
	
	void insertView(ssc::View* view, const QString& uid, const QString& volume, int r, int c);
private:
	void start(bool showSliders);
	ssc::View* generateSlice(const QString& uid, ssc::ToolPtr tool, ssc::ImagePtr image, ssc::PLANE_TYPE plane);
	ssc::View* generateGPUSlice(const QString& uid, ssc::ToolPtr tool, ssc::ImagePtr image, ssc::PLANE_TYPE plane);
	ssc::ImagePtr loadImage(const QString& imageFilename);
	ssc::View* getView2D();

//	ssc::Transform3D get_vpMs(ssc::View* view) const;
//	ssc::DoubleBoundingBox3D getViewport(ssc::View* view) const;
//	double mmPerPix(ssc::View* view) const;

	QSlider* mBrightnessSlider;
	QSlider *mContrastSlider;
	
	typedef std::set<ssc::View*> LayoutMap;
	std::set<ssc::View*> mLayouts;
	QGridLayout* mSliceLayout;
	QString mDisplayText;
	
	double mZoomFactor;
	int mRenderCount;
	int mTotalRender;
	int mTotalOther;
	QTime mLastRenderEnd;
	ssc::AcceptanceBoxWidget* mAcceptanceBox; 
	
	
	ssc::Texture3DSlicerRepPtr m_test_rep;
	ssc::View* m_test_view;

public slots:
	void updateRender();
	void contrast(int val);
	void brightness(int val);
};



#endif /*SSCTESTSLICEANDTOOLREP_H_*/
