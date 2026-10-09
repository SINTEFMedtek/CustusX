/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <cmath>
#include "cxMathUtils.h"
#include "cxTransform3D.h"

namespace cxtest
{

namespace
{
cx::Transform3D createPose()
{
	return cx::createTransformTranslate(cx::Vector3D(10, -20, 30))
			* cx::createTransformRotateZ(0.4) * cx::createTransformRotateX(-1.1) * cx::createTransformRotateY(2.0);
}

bool isRotation(const cx::Transform3D& pose)
{
	Eigen::Matrix3d rotation = pose.matrix().block<3, 3>(0, 0);
	return (rotation * rotation.transpose()).isIdentity(1e-9) && std::abs(rotation.determinant() - 1) < 1e-9;
}
} // namespace

TEST_CASE("MathUtils: matrixToQuaternion gives the quaternion as x, y, z, w, then the translation", "[unit]")
{
	double angle = std::acos(-1.0)/2; // 90 degrees
	cx::Transform3D pose = cx::createTransformTranslate(cx::Vector3D(1, 2, 3)) * cx::createTransformRotateZ(angle);

	Eigen::ArrayXd q = cx::matrixToQuaternion(pose);

	REQUIRE(q.size() == 7);
	CHECK(std::abs(q(0)) < 1e-12);
	CHECK(std::abs(q(1)) < 1e-12);
	CHECK(std::abs(q(2)) == Approx(std::sin(angle/2)));
	CHECK(std::abs(q(3)) == Approx(std::cos(angle/2)));
	bool sameSign = q(2)*q(3) > 0; // z and w have the same sign for a positive rotation about z
	CHECK(sameSign);
	CHECK(q(4) == Approx(1));
	CHECK(q(5) == Approx(2));
	CHECK(q(6) == Approx(3));
}

TEST_CASE("MathUtils: quaternionToMatrix is the inverse of matrixToQuaternion", "[unit]")
{
	cx::Transform3D pose = createPose();
	CHECK(cx::similar(cx::quaternionToMatrix(cx::matrixToQuaternion(pose)), pose));
}

TEST_CASE("MathUtils: quaternionToMatrix normalizes the quaternion, e.g. after averaging", "[unit]")
{
	cx::Transform3D pose = createPose();
	Eigen::ArrayXd q = cx::matrixToQuaternion(pose);
	Eigen::ArrayXd scaled = q;
	scaled.segment<4>(0) *= 0.8; // the average of two different unit quaternions is shorter than 1

	cx::Transform3D result = cx::quaternionToMatrix(scaled);

	CHECK(isRotation(result));
	CHECK(cx::similar(result, pose));
}

TEST_CASE("MathUtils: q and -q give the same pose, and alignQuaternionSign picks the sign closest to a reference", "[unit]")
{
	cx::Transform3D pose = createPose();
	Eigen::ArrayXd q = cx::matrixToQuaternion(pose);
	Eigen::ArrayXd negated = q;
	negated.segment<4>(0) *= -1;
	CHECK(cx::similar(cx::quaternionToMatrix(negated), pose));

	Eigen::ArrayXd aligned = cx::alignQuaternionSign(negated, q);
	CHECK((aligned - q).abs().maxCoeff() < 1e-12);
	CHECK((cx::alignQuaternionSign(q, q) - q).abs().maxCoeff() < 1e-12);
	CHECK((aligned.segment<3>(4) - q.segment<3>(4)).abs().maxCoeff() < 1e-12); // translation unchanged
}

} // namespace cxtest
