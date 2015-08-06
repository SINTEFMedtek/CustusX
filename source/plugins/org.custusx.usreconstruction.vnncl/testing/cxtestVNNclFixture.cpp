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
#include "cxtestVNNclFixture.h"

#include "catch.hpp"

#include "cxVNNclAlgorithm.h"
#include "cxVNNclReconstructionMethodService.h"
#include "cxBoolProperty.h"


namespace cxtest
{

VNNclFixture::VNNclFixture() :
	mAlgorithm(NULL)
{
	cx::UsReconstructionServicePtr reconstructer = mFixture.getManager();

	reconstructer->selectData(mRealData.getSourceFilename());
	reconstructer->getParam("Algorithm")->setValueFromVariant("VNNcl");
	reconstructer->getParam("Angio data")->setValueFromVariant(false);
	reconstructer->getParam("Dual Angio")->setValueFromVariant(false);

	cx::ReconstructionMethodService* algorithmService = reconstructer->createAlgorithm();
	REQUIRE(algorithmService);
	mAlgorithm = dynamic_cast<cx::VNNclReconstructionMethodService*>(algorithmService);
	REQUIRE(mAlgorithm);// Check if we got the algorithm

	mAlgo = reconstructer->getSettings().getElement("algorithms", "VNNcl");
	mAlgorithm->getRadiusOption(mAlgo)->setValue(1.0);

	this->initVNN();
}

void VNNclFixture::initVNN()
{
	mAlgorithm->getMethodOption(mAlgo)->setValue("VNN");
	mAlgorithm->getPlaneMethodOption(mAlgo)->setValue("Heuristic");
	mAlgorithm->getMaxPlanesOption(mAlgo)->setValue(1);
	mAlgorithm->getNStartsOption(mAlgo)->setValue(1);
}

void VNNclFixture::initVNN2()
{
	mAlgorithm->getMethodOption(mAlgo)->setValue("VNN2");
	mAlgorithm->getPlaneMethodOption(mAlgo)->setValue("Heuristic");
	mAlgorithm->getMaxPlanesOption(mAlgo)->setValue(8);
}

void VNNclFixture::initDW()
{
	mAlgorithm->getMethodOption(mAlgo)->setValue("DW");
	mAlgorithm->getPlaneMethodOption(mAlgo)->setValue("Heuristic");
	mAlgorithm->getMaxPlanesOption(mAlgo)->setValue(8);
}

void VNNclFixture::initAnisotropic()
{
	mAlgorithm->getMethodOption(mAlgo)->setValue("Anisotropic");
	mAlgorithm->getPlaneMethodOption(mAlgo)->setValue("Heuristic");
	mAlgorithm->getMaxPlanesOption(mAlgo)->setValue(8);
}

void VNNclFixture::initVNNMultistart()
{
	mAlgorithm->getMethodOption(mAlgo)->setValue("VNN");
	mAlgorithm->getNStartsOption(mAlgo)->setValue(5);
}

void VNNclFixture::initVNNClosest()
{
	mAlgorithm->getMethodOption(mAlgo)->setValue("VNN");
	mAlgorithm->getPlaneMethodOption(mAlgo)->setValue("Closest");
	mAlgorithm->getMaxPlanesOption(mAlgo)->setValue(8);
}

void VNNclFixture::reconstruct()
{
	mFixture.reconstruct();
}

void VNNclFixture::verify()
{
	REQUIRE(mFixture.getOutput().size()==1);
	mRealData.validateBModeData(mFixture.getOutput()[0]);
}

}//namespace
