/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxSlicerRepSW.h"
#include "cxSliceProxy.h"
#include "cxImage.h"
#include "cxRegistrationTransform.h"
#include "cxtestUtilities.h"

namespace cxtest
{

TEST_CASE("SliceRepSW: a 2D image is in the slice plane only when parallel and close to it", "[unit][resource][visualization]")
{
	cx::Vector3D center(10, 10, 0);
	CHECK(cx::SliceRepSW::isInSlicePlane(cx::Transform3D::Identity(), center));
	CHECK(cx::SliceRepSW::isInSlicePlane(cx::createTransformRotateZ(0.7) * cx::createTransformTranslate(cx::Vector3D(30, -5, 0.2)), center));
	CHECK(cx::SliceRepSW::isInSlicePlane(cx::createTransformRotateX(M_PI), center)); // seen from the other side
	CHECK_FALSE(cx::SliceRepSW::isInSlicePlane(cx::createTransformTranslate(cx::Vector3D(0, 0, 2)), center));
	CHECK_FALSE(cx::SliceRepSW::isInSlicePlane(cx::createTransformRotateX(0.1), center));
	CHECK_FALSE(cx::SliceRepSW::isInSlicePlane(cx::createTransformRotateY(M_PI / 2), center)); // a line in the slice
}

TEST_CASE("SliceRepSW: draws a 2D image in the slice plane directly, and slices other images", "[unit][resource][visualization]")
{
	cx::SimpleSliceProxyPtr slicer(new cx::SimpleSliceProxy());
	cx::SliceRepSWPtr rep = cx::SliceRepSW::New("rep");
	rep->setSliceProxy(slicer);

	cx::ImagePtr image2D = cxtest::Utilities::create3DImage(Eigen::Array3i(10, 10, 1), 100);
	rep->setImage(image2D);
	CHECK(rep->isDrawnDirectly());

	slicer->set_sMr(cx::createTransformRotateX(M_PI / 2)); // e.g. a view across the image
	CHECK_FALSE(rep->isDrawnDirectly());

	image2D->get_rMd_History()->setRegistration(cx::createTransformRotateX(-M_PI / 2)); // the image follows the plane
	CHECK(rep->isDrawnDirectly());

	rep->setImage(cxtest::Utilities::create3DImage(Eigen::Array3i(10, 10, 10), 100));
	CHECK_FALSE(rep->isDrawnDirectly());
}

} // namespace cxtest
