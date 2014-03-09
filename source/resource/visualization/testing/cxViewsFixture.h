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

#include "cxtestDummyDataManager.h"
#include "cxMessageListener.h"
#include "cxForwardDeclarations.h"
#include "sscDefinitions.h"
#include <vector>

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

/** Test class  with convenience methods for defining views.
 */
class ViewsFixture : public QObject
{
	Q_OBJECT

public:
	ViewsFixture(QString displayText="");
	virtual ~ViewsFixture();

	void define3D(const QString& imageFilename, const ImageParameters* parameters, int r, int c);
	void defineSlice(const QString& uid, const QString& imageFilename, cx::PLANE_TYPE plane, int r, int c);
	bool defineGPUSlice(const QString& uid, const QString&    imageFilename, cx::PLANE_TYPE plane, int r, int c);
	bool defineGPUSlice(const QString& uid, const std::vector<cx::ImagePtr> images, cx::PLANE_TYPE plane, int r, int c);

	cx::ViewWidget* addView(QString caption, int row, int col);

	bool quickRunWidget();

	double getFractionOfBrightPixelsInView(int viewIndex, int threshold, int component=0);
	void dumpDebugViewToDisk(QString text, int viewIndex);

	cx::ImagePtr loadImage(const QString& imageFilename);
	cx::DummyToolPtr dummyTool();

private:
	void applyParameters(cx::ImagePtr image, const ImageParameters* parameters);
	void fixToolToCenter();
	cx::SliceProxyPtr createSliceProxy(cx::PLANE_TYPE plane);
	RenderTesterPtr getRenderTesterForView(int viewIndex);

	QString mShaderFolder;
	cxtest::TestServicesPtr mServices;
	cx::MessageListenerPtr mMessageListener;
	ViewsWindowPtr mWindow;
};

} // namespace cxtest


#endif // CXVIEWSFIXTURE_H
