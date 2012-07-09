#ifndef SSCTESTSLICEANDTOOLREP_H_
#define SSCTESTSLICEANDTOOLREP_H_
#include <QtGui>
#include <map>
#include "sscView.h"
#include "sscViewContainer.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscAcceptanceBoxWidget.h"

/**
  * @brief Creates and returns a lut, based on range parameters.
  *
  * @return Pointer to a complete vtkLookupTable.
  */
vtkLookupTablePtr getCreateLut(int tableRangeMin, int tableRangeMax, double hueRangeMin, double hueRangeMax,
	double saturationRangeMin = 0, double saturationRangeMax = 1, double valueRangeMin = 1, double valueRangeMax = 1);

class ImageParameters
{
public:
	ImageParameters() : llr(.1), alpha(1) {}

	double llr;
	double alpha;

	vtkLookupTablePtr lut;
};

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
	void define3D(const QString& imageFilename, const ImageParameters* parameters, int r, int c);
	void define3DGPU(const QStringList& imageFilenames, const ImageParameters* parameters, int r, int c);
	void defineSlice(const QString& uid, const QString& imageFilename, ssc::PLANE_TYPE plane, int r, int c);
	void defineGPUSlice(const QString& uid, const QString& imageFilename, ssc::PLANE_TYPE plane, int r, int c);

	// was test accepted?
	bool accepted() const { return mAcceptanceBox->accepted(); }
	
	// setup views
	void insertView(ssc::ViewWidget *view, const QString& uid, const QString& volume, int r, int c);

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

public slots:
	void updateRender();
};

#endif /*SSCTESTSLICEANDTOOLREP_H_*/
