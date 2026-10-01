/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <vector>
#include "cxLandmarkTranslationRegistration.h"
#include "cxTypeConversions.h"

namespace cxtest
{

namespace
{

std::vector<cx::Vector3D> createLandmarks()
{
	std::vector<cx::Vector3D> retval;
	retval.push_back(cx::Vector3D(0, 0, 0));
	retval.push_back(cx::Vector3D(100, 0, 0));
	retval.push_back(cx::Vector3D(0, 80, 0));
	retval.push_back(cx::Vector3D(0, 0, 60));
	retval.push_back(cx::Vector3D(50, 40, 30));
	return retval;
}

std::vector<cx::Vector3D> translated(std::vector<cx::Vector3D> points, cx::Vector3D t)
{
	for (cx::Vector3D& point : points)
	{
		point += t;
	}
	return points;
}

cx::Vector3D registeredTranslation(std::vector<cx::Vector3D> ref, std::vector<cx::Vector3D> target)
{
	cx::LandmarkTranslationRegistration registration;
	bool ok = false;
	cx::Transform3D result = registration.registerPoints(ref, target, &ok);
	REQUIRE(ok);
	cx::Transform3D rotationPart = result;
	rotationPart.translation() = cx::Vector3D::Zero();
	CHECK(cx::similar(rotationPart, cx::Transform3D::Identity()));
	return result.translation();
}

} // namespace

TEST_CASE("LandmarkTranslationRegistration: finds the translation from target to ref", "[unit][plugins][org.custusx.registration]")
{
	const cx::Vector3D t(3, -2, 5);
	std::vector<cx::Vector3D> ref = createLandmarks();
	std::vector<cx::Vector3D> target = translated(ref, -t);

	for (unsigned numberOfPoints = 1; numberOfPoints <= ref.size(); ++numberOfPoints)
	{
		INFO("Number of points: " << numberOfPoints);
		std::vector<cx::Vector3D> r(ref.begin(), ref.begin()+numberOfPoints);
		std::vector<cx::Vector3D> s(target.begin(), target.begin()+numberOfPoints);
		cx::Vector3D result = registeredTranslation(r, s);
		INFO("Result: " << result);
		CHECK(cx::similar(result, t));
	}
}

TEST_CASE("LandmarkTranslationRegistration: noisy landmarks give the mean offset", "[unit][plugins][org.custusx.registration]")
{
	std::vector<cx::Vector3D> ref = createLandmarks();
	std::vector<cx::Vector3D> target = translated(ref, cx::Vector3D(-1, 2, -4));
	target[0] += cx::Vector3D(0.5, 0, 0);
	target[1] += cx::Vector3D(-0.5, 0.3, 0);
	target[2] += cx::Vector3D(0, -0.3, 0.2);
	target[3] += cx::Vector3D(0, 0, -0.2);

	cx::Vector3D mean = cx::Vector3D::Zero();
	for (unsigned i = 0; i < ref.size(); ++i)
	{
		mean += ref[i] - target[i];
	}
	mean /= ref.size();

	cx::Vector3D result = registeredTranslation(ref, target);
	INFO("Result: " << result << ", mean offset: " << mean);
	CHECK(cx::similar(result, mean));
}

TEST_CASE("LandmarkTranslationRegistration: fails for different number of points", "[unit][plugins][org.custusx.registration]")
{
	std::vector<cx::Vector3D> ref = createLandmarks();
	std::vector<cx::Vector3D> target(ref.begin(), ref.begin()+3);
	cx::LandmarkTranslationRegistration registration;
	bool ok = true;
	registration.registerPoints(ref, target, &ok);
	CHECK_FALSE(ok);
	registration.registerPoints(std::vector<cx::Vector3D>(), std::vector<cx::Vector3D>(), &ok);
	CHECK_FALSE(ok);
}

} // namespace cxtest
