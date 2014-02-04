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
 *  - ToolRep3D
 *  - SliceRepSW
 *  - VolumetricRep
 */
class ViewsWindow : public QMainWindow
{
	Q_OBJECT

public:
	ViewsWindow(QString displayText);
	virtual ~ViewsWindow();
	
	void setDescription(const QString& desc);
	void define3D(const QString& imageFilename, const ImageParameters* parameters, int r, int c);
	void defineSlice(const QString& uid, const QString& imageFilename, cx::PLANE_TYPE plane, int r, int c);
	bool defineGPUSlice(const QString& uid, const QString&    imageFilename, cx::PLANE_TYPE plane, int r, int c);
	bool defineGPUSlice(const QString& uid, const std::vector<cx::ImagePtr> images, cx::PLANE_TYPE plane, int r, int c);

//	// was test accepted?
//	bool accepted() const { return mAcceptanceBox->accepted(); }
	
	// setup views
	void insertView(cx::ViewWidget *view, const QString& uid, const QString& volume, int r, int c);
	bool runWidget();
	bool runWidget(int duration);
	bool quickRunWidget();

	double getFractionOfBrightPixelsInView(int viewIndex, int threshold, int component=0);
	void dumpDebugViewToDisk(QString text, int viewIndex);

	cx::ImagePtr loadImage(const QString& imageFilename);

private:
	void applyParameters(cx::ImagePtr image, const ImageParameters* parameters);
	void fixToolToCenter();
	void prettyZoom(cx::View *view);
	cx::SliceProxyPtr createSliceProxy(cx::PLANE_TYPE plane);
	cx::ViewWidget* create2DView(const QString& title, int r, int c);

	std::vector<cx::View *> mLayouts;
	QGridLayout* mSliceLayout;
	QString mDisplayText;
	
	double mZoomFactor;
	QString mShaderFolder;
//	cx::AcceptanceBoxWidget* mAcceptanceBox;
	QTimer* mRenderingTimer;
	int mRemaindingRenderings;

public slots:
	void updateRender();
};

#endif /*SSCTESTSLICEANDTOOLREP_H_*/
