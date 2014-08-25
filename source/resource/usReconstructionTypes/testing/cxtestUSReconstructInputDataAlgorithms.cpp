/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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


