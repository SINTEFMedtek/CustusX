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
#include "cxtestRenderTester.h"
#include <vtkImageData.h>
#include <vtkImageSlice.h>

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

TEST_CASE("SliceRepSW: a 2D image near the slice plane is moved exactly into it", "[unit][resource][visualization]")
{
	cx::Vector3D center(10, 10, 0);
	cx::Transform3D sMd = cx::createTransformRotateX(0.005) * cx::createTransformTranslate(cx::Vector3D(30, -5, 0.3));
	cx::Transform3D moved = cx::SliceRepSW::moveIntoSlicePlane(sMd, center);
	CHECK(moved.vector(cx::Vector3D(0, 0, 1)).normal()[2] == Approx(1.0));
	cx::Vector3D movedCenter = moved.coord(center);
	cx::Vector3D originalCenter = sMd.coord(center);
	CHECK(movedCenter[0] == Approx(originalCenter[0]));
	CHECK(movedCenter[1] == Approx(originalCenter[1]));
	CHECK(std::abs(movedCenter[2]) < 1e-9);

	cx::Transform3D flipped = cx::SliceRepSW::moveIntoSlicePlane(cx::createTransformRotateX(M_PI) * sMd, center);
	CHECK(flipped.vector(cx::Vector3D(0, 0, 1)).normal()[2] == Approx(-1.0));
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
	rep->update();
	CHECK_FALSE(rep->isDrawnDirectly());

	image2D->get_rMd_History()->setRegistration(cx::createTransformRotateX(-M_PI / 2)); // the image follows the plane
	rep->update();
	CHECK(rep->isDrawnDirectly());

	rep->setImage(cxtest::Utilities::create3DImage(Eigen::Array3i(10, 10, 10), 100));
	CHECK_FALSE(rep->isDrawnDirectly());
}

namespace
{
cx::ImagePtr createImageWithGradient()
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(40, 20, 1), cx::Vector3D(1, 1, 1), 0);
	vtkImageDataPtr data = image->getBaseVtkImageData();
	for (int x = 0; x < 40; ++x)
	{
		for (int y = 0; y < 20; ++y)
		{
			data->SetScalarComponentFromDouble(x, y, 0, 0, 50 + 5 * x);
		}
	}
	data->Modified();
	return image;
}

class TestSliceRepSW : public cx::SliceRepSW
{
public:
	static std::shared_ptr<TestSliceRepSW> New()
	{
		return wrap_new(new TestSliceRepSW(), "rep");
	}
	using cx::SliceRepSW::getActor;
};

vtkImageDataPtr renderSlice(std::shared_ptr<TestSliceRepSW> rep, bool drawDirectly)
{
	rep->setDirectDrawingEnabled(drawDirectly);
	CHECK(rep->isDrawnDirectly() == drawDirectly);
	rep->update();
	cxtest::RenderTesterPtr tester = cxtest::RenderTester::create();
	tester->addProp(rep->getActor());
	vtkImageDataPtr viewArea = cxtest::Utilities::create3DVtkImageData(Eigen::Array3i(120, 120, 1));
	viewArea->SetOrigin(-60, -60, 0);
	tester->alignRenderWindowWithImage(viewArea);
	vtkImageDataPtr retval = tester->renderToImage();
	CHECK(cxtest::Utilities::getFractionOfVoxelsAboveThreshold(retval, 0) > 0.01);
	return retval;
}
} // namespace

TEST_CASE("SliceRepSW: a 2D image drawn directly looks like the sliced image", "[unit][resource][visualization]")
{
	cx::SimpleSliceProxyPtr slicer(new cx::SimpleSliceProxy());
	slicer->set_sMr(cx::createTransformRotateX(M_PI / 2));
	cx::ImagePtr image = createImageWithGradient();
	cx::Transform3D sMd = cx::createTransformRotateZ(0.5) * cx::createTransformTranslate(cx::Vector3D(-15, -5, 0));
	image->get_rMd_History()->setRegistration(slicer->get_sMr().inv() * sMd);

	std::shared_ptr<TestSliceRepSW> rep = TestSliceRepSW::New();
	rep->setSliceProxy(slicer);
	rep->setImage(image);

	vtkImageDataPtr direct = renderSlice(rep, true);
	vtkImageDataPtr sliced = renderSlice(rep, false);
	cxtest::RenderTesterPtr tester = cxtest::RenderTester::create();
	tester->setImageErrorThreshold(1.0);
	CHECK(tester->findDifference(direct, sliced));

	image->get_rMd_History()->setRegistration(slicer->get_sMr().inv() * cx::createTransformTranslate(cx::Vector3D(5, 0, 0)) * sMd);
	vtkImageDataPtr moved = renderSlice(rep, true);
	CHECK(Eigen::Array3i(moved->GetDimensions()).isApprox(Eigen::Array3i(sliced->GetDimensions())));
	CHECK_FALSE(tester->findDifference(moved, sliced));
}

} // namespace cxtest
