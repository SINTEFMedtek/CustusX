/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <vtkAlgorithm.h>
#include <vtkDataObject.h>
#include <vtkImageData.h>
#include <vtkImageSlice.h>
#include <vtkImageMapper3D.h>
#include "cxImage2DRep3D.h"
#include "cxImage.h"
#include "cxtestUtilities.h"

namespace cxtest
{

namespace
{
double shownValue(cx::Image2DProxyPtr proxy)
{
	vtkAlgorithm* mapper = proxy->getSlice()->GetMapper();
	mapper->GetInputAlgorithm()->Update();
	vtkImageData* shown = vtkImageData::SafeDownCast(mapper->GetInputDataObject(0, 0));
	return shown->GetScalarComponentAsDouble(0, 0, 0, 0);
}
}

TEST_CASE("Image2DProxy: shows the new image data when it is replaced", "[unit][resource][visualization]")
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(10, 10, 1), 50);
	cx::Image2DProxyPtr proxy = cx::Image2DProxy::New();
	proxy->setImage(image);
	double before = shownValue(proxy);

	// e.g. a video stream that reconnects gives a new image data object
	image->setVtkImageData(cxtest::Utilities::create3DImage(Eigen::Array3i(10, 10, 1), 200)->getBaseVtkImageData(), false);
	CHECK(shownValue(proxy) != Approx(before));
}

TEST_CASE("Image2DProxy: shows new pixel values written into the same image data", "[unit][resource][visualization]")
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(10, 10, 1), 50);
	cx::Image2DProxyPtr proxy = cx::Image2DProxy::New();
	proxy->setImage(image);
	double before = shownValue(proxy);

	// e.g. a new video frame
	vtkImageDataPtr data = image->getBaseVtkImageData();
	data->SetScalarComponentFromDouble(0, 0, 0, 0, 200);
	data->Modified();
	CHECK(shownValue(proxy) != Approx(before));
}

} // namespace cxtest
