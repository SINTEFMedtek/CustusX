// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "catch.hpp"
#include "cxImageParameters.h"
#include "cxReporter.h"

namespace cxtest
{

TEST_CASE("ImageParameters empty constructor init all variables", "[unit]")
{
	cx::ImageParameters params;
	INFO(params.getDim() << " == " << Eigen::Array3i(0,0,0));
	CHECK(params.getDim().isApprox(Eigen::Array3i(0,0,0)));
}

TEST_CASE("ImageParameters: Constructed from bounds and spacing gives correct dim", "[unit]")
{
	cx::ImageParameters params;

	params.setSpacingKeepDim(Eigen::Array3d(1,0.1,1));

	Eigen::Array3d bounds(10, 20, 30);
	params.setDimKeepBoundsAlignSpacing(bounds);

	Eigen::Array3i expectedDim(11, 201, 31);
	{
		INFO(params.getDim() << " == " << expectedDim);
		CHECK(params.getDim().isApprox(expectedDim));
	}
	{
		INFO(params.getBounds() << " == " << bounds);
		CHECK(params.getBounds().isApprox(bounds));
	}
}

TEST_CASE("ImageParameters: limitVoxelsKeepBounds() works", "[unit]")
{
	cx::ImageParameters params;
	params.setSpacingKeepDim(Eigen::Array3d(1,1,1));
	Eigen::Array3d bounds(10, 20, 30);
	params.setDimKeepBoundsAlignSpacing(bounds);

	long maxNumVoxels = 1000;
	params.limitVoxelsKeepBounds(maxNumVoxels);

	INFO(params.getBounds() << " == " << bounds);
	INFO("dim: " << params.getDim());
	INFO("spacing: " << params.getSpacing());
	CHECK(params.getNumVoxels() <= maxNumVoxels);
	CHECK((params.getDim()+1).prod() > maxNumVoxels);

	CHECK(params.getBounds().isApprox(bounds));
}

TEST_CASE("ImageParameters: limitVoxelsKeepBounds() works with non uniform spacing", "[unit]")
{
	cx::ImageParameters params;

	Eigen::Array3d inputSpacing(0.5, 0.2, 0.1);
	params.setSpacingKeepDim(inputSpacing);
	Eigen::Array3d bounds(120, 30, 20);
	params.setDimKeepBoundsAlignSpacing(bounds);

	long maxNumVoxels = 1000;
	params.limitVoxelsKeepBounds(maxNumVoxels);

	INFO("dim: " << params.getDim());
	INFO("new spacing: " << params.getSpacing());

	Eigen::Array3d spacingRatio = inputSpacing / params.getSpacing();
	INFO("spacingRatio:" << spacingRatio);
	CHECK(spacingRatio[0] == Approx(spacingRatio[1]).epsilon(0.1));
	CHECK(spacingRatio[0] == Approx(spacingRatio[2]).epsilon(0.1));


	CHECK(params.getNumVoxels() <= maxNumVoxels);
	long numVoxelsWithAllDimsPlusOne = (params.getDim()+1).prod();
	CHECK(numVoxelsWithAllDimsPlusOne > maxNumVoxels);

	INFO(params.getBounds() << " == " << bounds);
	CHECK(params.getBounds().isApprox(bounds));
}


TEST_CASE("ImageParameters: Existing functionality in OutputVolumeParams is kept", "[unit]")
{
	cx::ImageParameters params;
	// input data taken from test "ReconstructManager: Angio Reconstruction"
	double inputSpacing = 0.0641024;
	long maxVolumeSize = 32;
	Eigen::Array3d bounds(42.9099, 40.7953, 31.0081);

	// OutputVolumeParams Constructor
	params.setSpacingKeepDim(Eigen::Array3d(inputSpacing,inputSpacing,inputSpacing));
	params.setDimKeepBoundsAlignSpacing(bounds);
	params.limitVoxelsKeepBounds(maxVolumeSize);

	{
		INFO(params.getBounds() << " == " << bounds);
		CHECK(params.getBounds().isApprox(bounds));
		CHECK((long)params.getNumVoxels() <= maxVolumeSize);
	}

	maxVolumeSize = 3.35544e+07;

	params.setSpacingKeepDim(Eigen::Array3d(inputSpacing,inputSpacing,inputSpacing));
	params.setDimKeepBoundsAlignSpacing(bounds);
	params.limitVoxelsKeepBounds(maxVolumeSize);

	{
		INFO(params.getBounds() << " == " << bounds);
		CHECK(params.getBounds().isApprox(bounds));
		CHECK((long)params.getNumVoxels() <= maxVolumeSize);
	}
}

TEST_CASE("ImageParameters: changeToUniformSpacing() works", "[unit]")
{
	cx::ImageParameters params;

	Eigen::Array3d inputSpacing(0.5, 0.2, 0.1);
	params.setSpacingKeepDim(inputSpacing);
	Eigen::Array3d bounds(120, 30, 20);
	params.setDimKeepBoundsAlignSpacing(bounds);

	long numVoxels = params.getNumVoxels();
	Eigen::Array3i inputDim = params.getDim();

	params.changeToUniformSpacing();

	Eigen::Array3d outSpacing = params.getSpacing();

	{
		INFO("new spacing: " << outSpacing);
		CHECK(outSpacing[0] == Approx(outSpacing[1]));
		CHECK(outSpacing[0] == Approx(outSpacing[2]));
	}
	{
		INFO(params.getBounds() << " == " << bounds);
		CHECK(params.getBounds().isApprox(bounds, 0.2));
	}

	CHECK((long)params.getNumVoxels() == Approx(numVoxels).epsilon(0.05));
}

} //namespace cxtest
