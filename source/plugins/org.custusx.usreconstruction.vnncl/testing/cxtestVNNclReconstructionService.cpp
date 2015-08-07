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

#include "cxVNNclAlgorithm.h"
#include "cxtestVNNclFixture.h"

//#ifdef CX_USE_OPENCL_UTILITY
//#include "cxSimpleSyntheticVolume.h"
//#endif // CX_USE_OPENCL_UTILITY

namespace cxtest
{

#ifdef CX_USE_OPENCL_UTILITY

TEST_CASE("VNNcl: VNN on sphere", "[unit][VNNcl][usreconstruction][synthetic]")
{
	VNNclSyntheticFixture vnnClFixture;
	vnnClFixture.initVNN();
	vnnClFixture.reconstruct();
	vnnClFixture.verify();
}
TEST_CASE("VNNcl: VNN2 on sphere", "[unit][VNNcl][usreconstruction][synthetic]")
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
TEST_CASE("VNNcl: VNN multistart on sphere", "[unit][VNNcl][usreconstruction][synthetic]")
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
