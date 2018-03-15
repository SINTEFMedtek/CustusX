/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxUSReconstructInputDataAlgoritms.h"

namespace cxtest
{

TEST_CASE("Slerp Interpolation", "[usreconstruction][unit]")
{
//	cx::ReconstructCorePtr reconstructer(new cx::ReconstructCore());

	cx::Transform3D a;
	cx::Transform3D b;

	Eigen::Matrix3d am;
	am =
			Eigen::AngleAxisd(0/*M_PI / 3.0*/, Eigen::Vector3d::UnitX()) * //60 deg
			Eigen::AngleAxisd(M_PI / 180.0*2.0, Eigen::Vector3d::UnitY()) * // 2 deg
			Eigen::AngleAxisd(0/*M_PI / 180.0*10.0*/, Eigen::Vector3d::UnitZ()); // 10 deg

	a.matrix().block<3, 3>(0, 0) = am;
	a.matrix().block<4, 1>(0, 3) = Eigen::Vector4d(0.0, 0.0, 0.0, 1.0);

	Eigen::Matrix3d bm;
	bm =
			Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX()) * //0 deg
			Eigen::AngleAxisd(0, Eigen::Vector3d::UnitY()) *
			Eigen::AngleAxisd(0, Eigen::Vector3d::UnitZ());

	b.matrix().block<3, 3>(0, 0) = bm;
	b.matrix().block<4, 1>(0, 3) = Eigen::Vector4d(10.0, 10.0, 10.0, 1.0);

	double t = 0.5;

	cx::Transform3D c = cx::USReconstructInputDataAlgorithm::slerpInterpolate(a, b, t);
	//Transform3D c = reconstructer->interpolate(a, b, t);

	Eigen::Matrix3d goalm;
	goalm =
			Eigen::AngleAxisd(0/*M_PI / 6.0*/, Eigen::Vector3d::UnitX()) * //30 deg
			Eigen::AngleAxisd(M_PI / 180.0, Eigen::Vector3d::UnitY()) * // 1 deg
			Eigen::AngleAxisd(0/*M_PI / 180.0*5.0*/, Eigen::Vector3d::UnitZ()); // 5 deg

	cx::Transform3D goal;
	goal.matrix().block<3, 3>(0, 0) = goalm;
	goal.matrix().block<4, 1>(0, 3) = Eigen::Vector4d(5.0, 5.0, 5.0, 1.0);

	if (!cx::similar(c, goal))
	{
		std::cout << "result: "<< std::endl << c << std::endl;
		std::cout << "goal: "<< std::endl << goal << std::endl;
	}
	REQUIRE(cx::similar(c, goal));

	// Test if normalized = the column lengths are 1
	double norm = goal.matrix().block<3, 1>(0, 0).norm();
	//	std::cout << "norm: " << norm << std::endl;
	REQUIRE(cx::similar(norm, 1.0));
	norm = goal.matrix().block<3, 1>(0, 1).norm();
	REQUIRE(cx::similar(norm, 1.0));
	norm = goal.matrix().block<3, 1>(0, 2).norm();
	REQUIRE(cx::similar(norm, 1.0));
}

} // namespace cxtest


