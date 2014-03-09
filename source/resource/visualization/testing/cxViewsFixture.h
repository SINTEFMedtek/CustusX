// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXVIEWSFIXTURE_H
#define CXVIEWSFIXTURE_H

#include <QtGui>
#include <map>
#include "sscView.h"
#include "sscViewContainer.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscAcceptanceBoxWidget.h"
#include "cxtestDummyDataManager.h"
#include "cxMessageListener.h"

namespace cxtest
{
typedef boost::shared_ptr<class RenderTester> RenderTesterPtr;

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
};

typedef boost::shared_ptr<class ViewsWindow> ViewsWindowPtr;

/**Test class  with convenience methods for defining views.
 * Uses the following reps:
 *  - ToolRep3D
 *  - SliceRepSW
 *  - VolumetricRep
 */
class ViewsFixture : public QObject
{
	Q_OBJECT

public:
	ViewsFixture(QString displayText="");
	virtual ~ViewsFixture();

//	void setDescription(const QString& desc);
	void define3D(const QString& imageFilename, const ImageParameters* parameters, int r, int c);
	void defineSlice(const QString& uid, const QString& imageFilename, cx::PLANE_TYPE plane, int r, int c);
	bool defineGPUSlice(const QString& uid, const QString&    imageFilename, cx::PLANE_TYPE plane, int r, int c);
	bool defineGPUSlice(const QString& uid, const std::vector<cx::ImagePtr> images, cx::PLANE_TYPE plane, int r, int c);

//	// setup views
	cx::ViewWidget* addView(QString caption, int row, int col);

//	void insertView(cx::ViewWidget *view, const QString& uid, const QString& volume, int r, int c);
	bool quickRunWidget();

	double getFractionOfBrightPixelsInView(int viewIndex, int threshold, int component=0);
	void dumpDebugViewToDisk(QString text, int viewIndex);

	cx::ImagePtr loadImage(const QString& imageFilename);
	cx::DummyToolPtr dummyTool();

private:
	void applyParameters(cx::ImagePtr image, const ImageParameters* parameters);
	void fixToolToCenter();
//	void prettyZoom(cx::View *view);
	cx::SliceProxyPtr createSliceProxy(cx::PLANE_TYPE plane);
//	cx::ViewWidget* create2DView(const QString& title, int r, int c);
//	vtkImageDataPtr getRenderedImageForView(int viewIndex);
	RenderTesterPtr getRenderTesterForView(int viewIndex);

//	std::vector<cx::View *> mLayouts;
//	QGridLayout* mSliceLayout;
//	QString mDisplayText;

//	double mZoomFactor;
	QString mShaderFolder;
//	QTimer* mRenderingTimer;
	cxtest::TestServicesPtr mServices;
	cx::MessageListenerPtr mMessageListener;
	ViewsWindowPtr mWindow;

//public slots:
//	void updateRender();
};

} // namespace cxtest


#endif // CXVIEWSFIXTURE_H
