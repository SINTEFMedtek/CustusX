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
#include "cxConfig.h"

#include "cxtestTSFFixture.h"

namespace cxtest
{

TEST_CASE("TubeSegmentationFramework testConstructor works", "[integration][tsf]")
{
	TestTubeSegmentationFramework helper;
	helper.testConstructor();
	REQUIRE(true);
}

TEST_CASE("TubeSegmentationFramework can set parameters", "[integration][tsf]")
{
	TestTubeSegmentationFramework helper;
	helper.testParameters();
}

TEST_CASE("TubeSegmentationFramework can load parameter file", "[integration][tsf]")
{
	TestTubeSegmentationFramework helper;
	helper.testLoadParameterFile();
}

//This test is tagged as unstable as it sometimes fails on OSX. Could be tagged as not_apple instead
TEST_CASE("TubeSegmentationFramework default preset works", "[integration][unstable][tsf]")
{
	TestTubeSegmentationFramework helper;
	helper.testDefaultPreset();
}

TEST_CASE("TubeSegmentationFramework SyntheticVascusynthPreset works", "[integration][tsf][not_apple][broken]")
{
	TestTubeSegmentationFramework helper;
	helper.testSyntheticVascusynthPreset();
}

#ifdef CX_CUSTUS_SINTEF
TEST_CASE("TubeSegmentationFramework PhantomAccUSPreset works", "[integration][not_apple][broken][tsf]")
{
	TestTubeSegmentationFramework helper;
	helper.testPhantomAccUSPreset();
}

TEST_CASE("TubeSegmentationFramework NeuroVesselsMRAPreset works", "[integration][not_apple][tsf]")
{
	TestTubeSegmentationFramework helper;
	helper.testNeuroVesselsMRAPreset();
}

TEST_CASE("TubeSegmentationFramework testNeuroVesselsUSAPreset works", "[integration][not_apple][tsf]")
{
	TestTubeSegmentationFramework helper;
	helper.testNeuroVesselsUSAPreset();
}

TEST_CASE("TubeSegmentationFramework testLungAirwaysCTPreset works", "[integration][not_apple][tsf]")
{
	TestTubeSegmentationFramework helper;
	helper.testLungAirwaysCTPreset();
}
#endif

} // namespace cxtest
