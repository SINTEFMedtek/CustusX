/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxSlices3DRep.h"
#include "cxtestVisServices.h"
#include "cxtestUtilities.h"

namespace cxtest
{

TEST_CASE("Slices3DRep: can be constructed", "[unit]")
{
	cx::Slices3DRepPtr rep = cx::Slices3DRep::New();
	REQUIRE(rep);
}

TEST_CASE("Slices3DRep: addPlane does not crash for axial plane", "[unit]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::Slices3DRepPtr rep = cx::Slices3DRep::New();

	CHECK_NOTHROW(rep->addPlane(cx::ptAXIAL, services->patient()));
}

TEST_CASE("Slices3DRep: addPlane three standard planes does not crash", "[unit]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::Slices3DRepPtr rep = cx::Slices3DRep::New();

	CHECK_NOTHROW(rep->addPlane(cx::ptAXIAL, services->patient()));
	CHECK_NOTHROW(rep->addPlane(cx::ptCORONAL, services->patient()));
	CHECK_NOTHROW(rep->addPlane(cx::ptSAGITTAL, services->patient()));
}

TEST_CASE("Slices3DRep: setImages with a 3D image does not crash", "[unit]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::Slices3DRepPtr rep = cx::Slices3DRep::New();
	rep->addPlane(cx::ptAXIAL, services->patient());
	rep->addPlane(cx::ptCORONAL, services->patient());
	rep->addPlane(cx::ptSAGITTAL, services->patient());

	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(10, 10, 10));
	REQUIRE(image);

	std::vector<cx::ImagePtr> images;
	images.push_back(image);
	CHECK_NOTHROW(rep->setImages(images));
}

TEST_CASE("Slices3DRep: setTool with null tool does not crash", "[unit]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::Slices3DRepPtr rep = cx::Slices3DRep::New();
	rep->addPlane(cx::ptAXIAL, services->patient());

	CHECK_NOTHROW(rep->setTool(cx::ToolPtr()));
}

} // namespace cxtest
