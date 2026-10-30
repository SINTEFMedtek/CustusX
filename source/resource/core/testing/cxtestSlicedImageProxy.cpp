/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <vtkImageAlgorithm.h>
#include <vtkImageData.h>
#include "cxSlicedImageProxy.h"
#include "cxImage.h"
#include "cxRegistrationTransform.h"
#include "cxtestUtilities.h"

namespace cxtest
{

TEST_CASE("SlicedImageProxy: an unchanged slice plane doesn't slice again", "[unit][resource][core]")
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(20, 20, 20));
	cx::SlicedImageProxy proxy;
	proxy.setImage(image);
	proxy.setOutputFormat(cx::Vector3D(0, 0, 0), Eigen::Array3i(10, 10, 1), cx::Vector3D(1, 1, 1));

	vtkImageAlgorithmPtr output = proxy.getOutputPortWithoutLUT();
	output->Update();
	vtkMTimeType sliced = output->GetOutput()->GetMTime();

	proxy.update(); // e.g. the image and the plane moved together
	output->Update();
	CHECK(output->GetOutput()->GetMTime() == sliced);

	image->get_rMd_History()->setRegistration(cx::createTransformTranslate(cx::Vector3D(5, 0, 0)));
	output->Update();
	CHECK(output->GetOutput()->GetMTime() > sliced);
}

TEST_CASE("SlicedImageProxy: slices the new image data when it is replaced", "[unit][resource][core]")
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(20, 20, 20), 100);
	cx::SlicedImageProxy proxy;
	proxy.setImage(image);
	proxy.setOutputFormat(cx::Vector3D(0, 0, 0), Eigen::Array3i(10, 10, 1), cx::Vector3D(1, 1, 1));
	vtkImageAlgorithmPtr output = proxy.getOutputPortWithoutLUT();
	output->Update();
	CHECK(output->GetOutput()->GetScalarRange()[1] == Approx(100));

	// e.g. a video stream that reconnects gives a new image data object
	image->setVtkImageData(cxtest::Utilities::create3DImage(Eigen::Array3i(20, 20, 20), 200)->getBaseVtkImageData(), false);
	output->Update();
	CHECK(output->GetOutput()->GetScalarRange()[1] == Approx(200));
}

} // namespace cxtest
