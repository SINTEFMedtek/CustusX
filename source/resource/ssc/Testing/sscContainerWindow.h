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
 *  - ToolRep3D
 *  - SliceRepSW
 *  - VolumetricRep
 */
class ContainerWindow : public QMainWindow
{
	Q_OBJECT

public:
	ContainerWindow(QString displayText, bool showSliders);
	virtual ~ContainerWindow();
	
	void setDescription(const QString& desc);

	// create ViewContainer-based rendering views (do not mix with above ViewWidget-based functions)
	void container3D(cx::ViewItem *view, const QString& imageFilename);
	void containerGPUSlice(cx::ViewItem *view, const QString& uid, const QString& imageFilename, cx::PLANE_TYPE plane);
	void containerGPU3D(cx::ViewItem *view, const QStringList& imageFilenames, const ImageParameters* parameters);

	// was test accepted?
	bool accepted() const { return mAcceptanceBox->accepted(); }
	
	// setup views
	void setupViewContainer(cx::ViewContainer *view, const QString& uid, const QString& volume, int r, int c);

	/// ugly hack
	bool mDumpSpeedData;

private:
	void start(bool showSliders);
	cx::ImagePtr loadImage(const QString& imageFilename);

	QSlider* mBrightnessSlider;
	QSlider *mContrastSlider;
	
	typedef std::set<cx::View *> LayoutMap;
	std::set<cx::View *> mLayouts;
	QGridLayout* mSliceLayout;
	QString mDisplayText;
	
	double mZoomFactor;
	int mRenderCount;
	int mTotalRender;
	int mTotalOther;
	QTime mLastRenderEnd;
	QString mShaderFolder;
	cx::AcceptanceBoxWidget* mAcceptanceBox;
	cx::ViewContainer *mContainer;

public slots:
	void updateRender();
};

#endif
