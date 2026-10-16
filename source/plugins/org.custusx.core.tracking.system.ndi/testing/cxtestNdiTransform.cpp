/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <cmath>
#include "cxNdiTransformUtils.h"
#include "cxTransform3D.h"

namespace cxtest
{

TEST_CASE("ndiQuaternionToTransform3D: identity quaternion gives identity matrix", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::Transform3D result = cx::ndiQuaternionToTransform3D(1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

	CHECK(cx::similar(result, cx::Transform3D::Identity()));
}

TEST_CASE("ndiQuaternionToTransform3D: translation is preserved", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::Transform3D result = cx::ndiQuaternionToTransform3D(1.0, 0.0, 0.0, 0.0, 10.0, 20.0, 30.0);

	cx::Transform3D expected = cx::Transform3D::Identity();
	expected.matrix()(0, 3) = 10.0;
	expected.matrix()(1, 3) = 20.0;
	expected.matrix()(2, 3) = 30.0;
	CHECK(cx::similar(result, expected));
}

TEST_CASE("ndiQuaternionToTransform3D: 90-degree rotation around Z", "[unit][org.custusx.core.tracking.system.ndi]")
{
	// 90 degrees around Z: q = (cos(45), 0, 0, sin(45)) = (sqrt(2)/2, 0, 0, sqrt(2)/2)
	const double s = std::sqrt(2.0) / 2.0;
	cx::Transform3D result = cx::ndiQuaternionToTransform3D(s, 0.0, 0.0, s, 0.0, 0.0, 0.0);

	// Expected rotation matrix for 90 degrees around Z:
	// [ 0 -1  0  0 ]
	// [ 1  0  0  0 ]
	// [ 0  0  1  0 ]
	// [ 0  0  0  1 ]
	cx::Transform3D expected = cx::Transform3D::Identity();
	expected.matrix()(0, 0) =  0.0;
	expected.matrix()(0, 1) = -1.0;
	expected.matrix()(1, 0) =  1.0;
	expected.matrix()(1, 1) =  0.0;
	CHECK(cx::similar(result, expected));
}

TEST_CASE("ndiQuaternionToTransform3D: 90-degree rotation around X", "[unit][org.custusx.core.tracking.system.ndi]")
{
	// 90 degrees around X: q = (cos(45), sin(45), 0, 0) = (sqrt(2)/2, sqrt(2)/2, 0, 0)
	const double s = std::sqrt(2.0) / 2.0;
	cx::Transform3D result = cx::ndiQuaternionToTransform3D(s, s, 0.0, 0.0, 0.0, 0.0, 0.0);

	// Expected rotation matrix for 90 degrees around X:
	// [ 1  0  0  0 ]
	// [ 0  0 -1  0 ]
	// [ 0  1  0  0 ]
	// [ 0  0  0  1 ]
	cx::Transform3D expected = cx::Transform3D::Identity();
	expected.matrix()(1, 1) =  0.0;
	expected.matrix()(1, 2) = -1.0;
	expected.matrix()(2, 1) =  1.0;
	expected.matrix()(2, 2) =  0.0;
	CHECK(cx::similar(result, expected));
}

TEST_CASE("ndiQuaternionToTransform3D: non-unit quaternion is normalized", "[unit][org.custusx.core.tracking.system.ndi]")
{
	// Scale the identity quaternion by 2 -- after normalization should give identity rotation
	cx::Transform3D result = cx::ndiQuaternionToTransform3D(2.0, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0);

	cx::Transform3D expected = cx::Transform3D::Identity();
	expected.matrix()(0, 3) = 5.0;
	CHECK(cx::similar(result, expected));
}

} // namespace cxtest
