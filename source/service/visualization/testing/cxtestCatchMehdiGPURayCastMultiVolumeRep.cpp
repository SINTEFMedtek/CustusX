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

#include "cxConfig.h"
#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME

#include "catch.hpp"
#include "cxMehdiGPURayCastMultiVolumeRep.h"


#include <QtGui>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkWindowToImageFilter.h>
#include "sscView.h"
#include "sscVolumetricRep.h"
#include "sscVector3D.h"
#include "sscImage.h"
#include "cxtestRenderTester.h"
#include "cxtestUtilities.h"


typedef vtkSmartPointer<class vtkWindowToImageFilter> vtkWindowToImageFilterPtr;

namespace cxtest
{

TEST_CASE("MehdiGPURayCastMultiVolumeRep can render 3 small volumes.", "[integration][gui][notmac]")
{
	unsigned int imageCount = 3;
	std::vector<cx::ImagePtr> mImages = cxtest::Utilities::create3DImages(imageCount, Eigen::Array3i(3,3,3), 200);

	cx::MehdiGPURayCastMultiVolumeRepPtr rep = cx::MehdiGPURayCastMultiVolumeRep::New("");
	REQUIRE(rep);
	rep->setImages(mImages);

	unsigned int viewAxisSize = 30;
	RenderTesterPtr renderTester = cxtest::RenderTester::create(rep, viewAxisSize);

	vtkImageDataPtr output = renderTester->renderToImage();
	REQUIRE(cx::similar(Eigen::Array3i(output->GetDimensions()), Eigen::Array3i(viewAxisSize,viewAxisSize,1)));

	unsigned int numNonZeroPixels = renderTester->getNumberOfNonZeroPixels(output);
	REQUIRE(numNonZeroPixels > 0);
	REQUIRE(numNonZeroPixels < viewAxisSize*viewAxisSize);

//	QTimer::singleShot(SSC_DEFAULT_TEST_TIMEOUT_SECS*1000, qApp, SLOT(quit()));
//	qApp->exec();
}

TEST_CASE("VolumetricRep can render 1 small volume.", "[unit][gui]")
{
	unsigned int imageCount = 1;
	std::vector<cx::ImagePtr> mImages = cxtest::Utilities::create3DImages(imageCount, Eigen::Array3i(3,3,3), 200);

	cx::VolumetricRepPtr rep = cx::VolumetricRep::New("");
	REQUIRE(rep);
	rep->setImage(mImages[0]);

	unsigned int viewAxisSize = 30;
	RenderTesterPtr renderTester = cxtest::RenderTester::create(rep, viewAxisSize);

	vtkImageDataPtr output = renderTester->renderToImage();
	REQUIRE(cx::similar(Eigen::Array3i(output->GetDimensions()), Eigen::Array3i(viewAxisSize,viewAxisSize,1)));

	unsigned int numNonZeroPixels = renderTester->getNumberOfNonZeroPixels(output);
	REQUIRE(numNonZeroPixels > 0);
	REQUIRE(numNonZeroPixels < viewAxisSize*viewAxisSize);

//	QTimer::singleShot(SSC_DEFAULT_TEST_TIMEOUT_SECS*1000, qApp, SLOT(quit()));
//	qApp->exec();

}

} // namespace cxtest


#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME
