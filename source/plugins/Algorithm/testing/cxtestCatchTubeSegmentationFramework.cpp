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

#ifdef CX_USE_TSF
#include "catch.hpp"

#include "cxTestTubeSegmentationFramework.h"

namespace cxtest
{

TEST_CASE("TubeSegmentationFramework testConstructor works", "[integration]")
{
	TestTubeSegmentationFramework helper;
	helper.testConstructor();
	REQUIRE(true);
}

TEST_CASE("TubeSegmentationFramework can set parameters", "[integration]")
{
	TestTubeSegmentationFramework helper;
	helper.testParameters();
}

TEST_CASE("TubeSegmentationFramework can load parameter file", "[integration]")
{
	TestTubeSegmentationFramework helper;
	helper.testLoadParameterFile();
}

TEST_CASE("TubeSegmentationFramework default preset works", "[integration][unstable]")
{
	TestTubeSegmentationFramework helper;
	helper.testDefaultPreset();
}

TEST_CASE("TubeSegmentationFramework SyntheticVascusynthPreset works", "[integration]")
{
	TestTubeSegmentationFramework helper;
	helper.testSyntheticVascusynthPreset();
}

TEST_CASE("TubeSegmentationFramework PhantomAccUSPreset works", "[integration][unstable]")
{
	TestTubeSegmentationFramework helper;
	helper.testPhantomAccUSPreset();
}

TEST_CASE("TubeSegmentationFramework NeuroVesselsMRAPreset works", "[integration][not_apple]")
{
	TestTubeSegmentationFramework helper;
	helper.testNeuroVesselsMRAPreset();
}

TEST_CASE("TubeSegmentationFramework testNeuroVesselsUSAPreset works", "[integration][not_apple]")
{
	TestTubeSegmentationFramework helper;
	helper.testNeuroVesselsUSAPreset();
}

TEST_CASE("TubeSegmentationFramework testLungAirwaysCTPreset works", "[integration][not_apple]")
{
	TestTubeSegmentationFramework helper;
	helper.testLungAirwaysCTPreset();
}

} // namespace cxtest
#endif //CX_USE_TSF
