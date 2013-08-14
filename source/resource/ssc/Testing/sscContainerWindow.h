#ifndef SSCTESTCONTAINERWINDOW_H_
#define SSCTESTCONTAINERWINDOW_H_
#include <QtGui>
#include <map>
#include "sscViewContainer.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscAcceptanceBoxWidget.h"
#include "sscViewsWindow.h"

/**Test class  with convenience methods for defining views.
 * Uses the following reps:
 *  - ssc::ToolRep3D
 *  - ssc::SliceRepSW
 *  - ssc::VolumetricRep
 */
class ContainerWindow : public QMainWindow
{
	Q_OBJECT

public:
	ContainerWindow(QString displayText, bool showSliders);
	virtual ~ContainerWindow();
	
	void setDescription(const QString& desc);

	// create ssc::ViewContainer-based rendering views (do not mix with above ssc::ViewWidget-based functions)
	void container3D(ssc::ViewItem *view, const QString& imageFilename);
	void containerGPUSlice(ssc::ViewItem *view, const QString& uid, const QString& imageFilename, ssc::PLANE_TYPE plane);
	void containerGPU3D(ssc::ViewItem *view, const QStringList& imageFilenames, const ImageParameters* parameters);

	// was test accepted?
	bool accepted() const { return mAcceptanceBox->accepted(); }
	
	// setup views
	void setupViewContainer(ssc::ViewContainer *view, const QString& uid, const QString& volume, int r, int c);

	/// ugly hack
	bool mDumpSpeedData;

private:
	void start(bool showSliders);
	ssc::ImagePtr loadImage(const QString& imageFilename);

	QSlider* mBrightnessSlider;
	QSlider *mContrastSlider;
	
	typedef std::set<ssc::View *> LayoutMap;
	std::set<ssc::View *> mLayouts;
	QGridLayout* mSliceLayout;
	QString mDisplayText;
	
	double mZoomFactor;
	int mRenderCount;
	int mTotalRender;
	int mTotalOther;
	QTime mLastRenderEnd;
	QString mShaderFolder;
	ssc::AcceptanceBoxWidget* mAcceptanceBox;
	ssc::ViewContainer *mContainer;

public slots:
	void updateRender();
};

#endif
