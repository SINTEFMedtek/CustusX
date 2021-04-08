/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXVIEWSFIXTURE_H
#define CXVIEWSFIXTURE_H

#include "cxtest_org_custusx_core_view_export.h"

#include "cxtestVisServices.h"
#include "cxMessageListener.h"
#include "cxForwardDeclarations.h"
#include "cxDefinitions.h"
#include <vector>
#include "cxtestFileManagerServiceMock.h"
#include "cxtestPatientModelServiceMock.h"
#include "cxFileReaderWriterService.h"
#include "cxtestRenderTester.h"

namespace cxtest
{

/**
  * @brief Creates and returns a lut, based on range parameters.
  *
  * @return Pointer to a complete vtkLookupTable.
  */
vtkLookupTablePtr getCreateLut(int tableRangeMin, int tableRangeMax, double hueRangeMin, double hueRangeMax,
	double saturationRangeMin = 0, double saturationRangeMax = 1, double valueRangeMin = 1, double valueRangeMax = 1);

class CXTEST_ORG_CUSTUSX_CORE_VIEW_EXPORT ImageParameters
{
public:
	ImageParameters() : llr(.1), alpha(1) {}

	double llr;
	double alpha;
};

typedef boost::shared_ptr<class ViewsWindow> ViewsWindowPtr;

/** Test class  with convenience methods for defining views.
 *
 * Tests using this fixture must be integration tests.
 * This is because there can only be one cx::RenderWindowFactory.
 */
class CXTEST_ORG_CUSTUSX_CORE_VIEW_EXPORT ViewsFixture : public QObject
{
	Q_OBJECT

public:
	ViewsFixture(QString displayText="");
	virtual ~ViewsFixture();
	
	bool messageListenerContainErrors();

	void define3D(const QString& imageFilename, const ImageParameters* parameters, int r, int c);
	void defineSlice(const QString& uid, const QString& imageFilename, cx::PLANE_TYPE plane, int r, int c);
	bool defineGPUSlice(const QString& uid, const QString&    imageFilename, cx::PLANE_TYPE plane, int r, int c);
	bool defineGPUSlice(const QString& uid, const std::vector<cx::ImagePtr> images, cx::PLANE_TYPE plane, int r, int c);

	cx::ViewPtr addView(int row, int col);

	bool quickRunWidget();
	bool runWidget();
	void clear();

	double getFractionOfBrightPixelsInRenderWindowForView(int viewIndex, int threshold, int component=0);
	void dumpDebugViewToDisk(QString text, int viewIndex);

	cx::ImagePtr loadImage(const QString& imageFilename);
	cx::DummyToolPtr dummyTool();

	void addFileReaderWriter(cx::FileReaderWriterServicePtr service);

	cxtest::PatientModelServiceMockPtr getPatientModelService();
	cx::FileManagerServicePtr getFileManager();
private:
	void applyParameters(cx::ImagePtr image, const ImageParameters* parameters);
	void fixToolToCenter();
	cx::SliceProxyPtr createSliceProxy(cx::PLANE_TYPE plane);
	RenderTesterPtr getRenderTesterForRenderWindow(int viewIndex);

	QString mShaderFolder;
	cxtest::TestVisServicesPtr mServices;
	cx::MessageListenerPtr mMessageListener;
	ViewsWindowPtr mWindow;
	cx::RenderWindowFactoryPtr mFactory;
	cx::FileManagerServicePtr mFilemanager;
	std::set<cx::FileReaderWriterServicePtr> mDataReaders;
};

} // namespace cxtest


#endif // CXVIEWSFIXTURE_H
