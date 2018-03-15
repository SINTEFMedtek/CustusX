/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxVNNclAlgorithm.h"
#include "cxtestVNNclFixture.h"

//#ifdef CX_USE_OPENCL_UTILITY
//#include "cxSimpleSyntheticVolume.h"
//#endif // CX_USE_OPENCL_UTILITY

namespace cxtest
{

#ifdef CX_USE_OPENCL_UTILITY

TEST_CASE("VNNcl: VNN on sphere", "[unit][VNNcl][usreconstruction][synthetic][not_apple]")
{
	VNNclSyntheticFixture vnnClFixture;
	vnnClFixture.initVNN();
	vnnClFixture.reconstruct();
	vnnClFixture.verify();
}

TEST_CASE("VNNcl: VNN2 on sphere", "[unit][VNNcl][usreconstruction][synthetic][not_apple]")
{
	VNNclSyntheticFixture vnnClFixture;
	vnnClFixture.initVNN2();
	vnnClFixture.reconstruct();
	vnnClFixture.verify();
}

//Crashes on OSX 10.8.5
TEST_CASE("VNNcl: DW on sphere", "[unit][VNNcl][usreconstruction][synthetic][not_apple]")
{
	VNNclSyntheticFixture vnnClFixture;
	vnnClFixture.initDW();
	vnnClFixture.reconstruct();
	vnnClFixture.verify();
}

//Crashes on OSX 10.9.5 and 10.8.5
TEST_CASE("VNNcl: Anisotropic on sphere", "[unit][VNNcl][usreconstruction][synthetic][not_apple]")
{
	VNNclSyntheticFixture vnnClFixture;
	vnnClFixture.initAnisotropic();
	vnnClFixture.reconstruct();
	vnnClFixture.verify();
}

//Crashes on OSX 10.8.5
TEST_CASE("VNNcl: VNN multistart on sphere", "[unit][VNNcl][usreconstruction][synthetic][not_apple]")
{
	VNNclSyntheticFixture vnnClFixture;
	vnnClFixture.initVNNMultistart();
	vnnClFixture.reconstruct();
	vnnClFixture.verify();
}


//The following 6 tests seem to constantly fail on OSX, but sometimes run on windows and Linux
TEST_CASE("VNNcl: VNN on real data", "[usreconstruction][integration][VNNcl][unstable][not_apple]")
{
	VNNclFixture vnnClFixture;
	vnnClFixture.initVNN();
	vnnClFixture.reconstruct();
	vnnClFixture.verify();
}

TEST_CASE("VNNcl: VNN2 on real data", "[usreconstruction][integration][VNNcl][unstable][not_apple]")
{
	VNNclFixture vnnClFixture;
	vnnClFixture.initVNN2();
	vnnClFixture.reconstruct();
	vnnClFixture.verify();
}

TEST_CASE("VNNcl: DW on real data", "[usreconstruction][integration][VNNcl][unstable][not_apple]")
{
	VNNclFixture vnnClFixture;
	vnnClFixture.initDW();
	vnnClFixture.reconstruct();
	vnnClFixture.verify();
}

TEST_CASE("VNNcl: Anisotropic on real data", "[usreconstruction][integration][VNNcl][unstable][not_apple]")
{
	VNNclFixture vnnClFixture;
	vnnClFixture.initAnisotropic();
	vnnClFixture.reconstruct();
	vnnClFixture.verify();
}

TEST_CASE("VNNcl: VNN Multistart on real data", "[usreconstruction][integration][VNNcl][unstable][not_apple]")
{
	VNNclFixture vnnClFixture;
	vnnClFixture.initVNNMultistart();
	vnnClFixture.reconstruct();
	vnnClFixture.verify();
}

TEST_CASE("VNNcl: VNN Closest on real data", "[usreconstruction][integration][VNNcl][unstable][not_apple]")
{
	VNNclFixture vnnClFixture;
	vnnClFixture.initVNNClosest();
	vnnClFixture.reconstruct();
	vnnClFixture.verify();
}

#endif//CX_USE_OPENCL_UTILITY

} //namespace cxtest
