#ifndef SSCTESTSLICEANDTOOLREP_H_
#define SSCTESTSLICEANDTOOLREP_H_
#include <QtGui>
#include <map>
#include "sscView.h"

#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscAcceptanceBoxWidget.h"

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

	// create ssc::View-based rendering (will not work with ViewContainers)
	void define3D(const QString& imageFilename, int r, int c);
	void defineSlice(const QString& uid, const QString& imageFilename, ssc::PLANE_TYPE plane, int r, int c);
	void defineGPUSlice(const QString& uid, const QString& imageFilename, ssc::PLANE_TYPE plane, int r, int c);

	// create ssc::ViewContainer-based rendering views (do not mix with above ssc::View-based functions)
	void container3D(ssc::ViewContainer *widget, int pos, const QString& imageFilename, int r, int c);
	void containerGPUSlice(ssc::ViewContainer *widget, int pos, const QString& uid, const QString& imageFilename, ssc::PLANE_TYPE plane, int r, int c);

	// was test accepted?
	bool accepted() const { return mAcceptanceBox->accepted(); }
	
	// setup views
	void insertView(ssc::View *view, const QString& uid, const QString& volume, int r, int c);
	void setupViewContainer(ssc::ViewContainer *view, const QString& uid, const QString& volume, int r, int c);

	/// ugly hack
	bool mDumpSpeedData;

private:
	void start(bool showSliders);
	ssc::ImagePtr loadImage(const QString& imageFilename);

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

public slots:
	void updateRender();

	// FIXME - do we need these here??
	void contrast(int val) {}
	void brightness(int val) {}
};

#endif /*SSCTESTSLICEANDTOOLREP_H_*/
