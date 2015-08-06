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

#include "cxReporter.h"
#include "cxVNNclAlgorithm.h"
#include "cxVNNclReconstructionMethodService.h"
#include "cxBoolProperty.h"
#include "cxtestUtilities.h"
#include "cxtestReconstructRealData.h"
#include "cxtestJenkinsMeasurement.h"
#include "cxtestReconstructionAlgorithmFixture.h"
#include "cxtestReconstructionManagerFixture.h"
#include "cxLogicManager.h"

#ifdef CX_USE_OPENCL_UTILITY
#include "cxSimpleSyntheticVolume.h"
#endif // CX_USE_OPENCL_UTILITY

namespace cxtest
{

#ifdef CX_USE_OPENCL_UTILITY
TEST_CASE("VNNcl init", "[unit][VNNcl][usreconstruction][synthetic]")
{
	cx::LogicManager::initialize();

	ctkPluginContext* pluginContext = cx::logicManager()->getPluginContext();

	ReconstructionAlgorithmFixture fixture;

	cx::VNNclReconstructionMethodServicePtr algorithm = cx::VNNclReconstructionMethodService::create(pluginContext);

	REQUIRE(algorithm);

	cx::LogicManager::shutdown();
}

TEST_CASE("VNNcl: VNN on sphere", "[unit][VNNcl][usreconstruction][synthetic]")
{
	cx::LogicManager::initialize();

	ReconstructionAlgorithmFixture fixture;

	ctkPluginContext* pluginContext = cx::logicManager()->getPluginContext();
	cx::VNNclReconstructionMethodServicePtr algorithm = cx::VNNclReconstructionMethodService::create(pluginContext);

	QString name = "VNN";

	fixture.setOverallBoundsAndSpacing(100, 5);
	fixture.getInputGenerator()->setSpherePhantom();

	algorithm->enableProfiling();

	QDomDocument domdoc;
	QDomElement settings = domdoc.createElement("VNNcl");

	algorithm->getRadiusOption(settings)->setValue(10);
	algorithm->getMethodOption(settings)->setValue("VNN");
	algorithm->getPlaneMethodOption(settings)->setValue("Heuristic");
	algorithm->getMaxPlanesOption(settings)->setValue(8);
	algorithm->getNStartsOption(settings)->setValue(1);

	fixture.setAlgorithm(algorithm);
	fixture.reconstruct(settings);

	double executionTime = algorithm->getKernelExecutionTime();
	JenkinsMeasurement jenkins;
	jenkins.createOutput(name, QString::number(executionTime));

	fixture.checkRMSBelow(20.0);
	fixture.checkCentroidDifferenceBelow(1);
	fixture.checkMassDifferenceBelow(0.01);


	cx::LogicManager::shutdown();
}

TEST_CASE("ReconstructAlgorithm: VNNcl on sphere","[unit][VNNcl][usreconstruction][synthetic][not_win32][broken][not_mavericks]")
{
	cx::LogicManager::initialize();
	cx::Reporter::initialize();

	ctkPluginContext* pluginContext = cx::logicManager()->getPluginContext();

	ReconstructionAlgorithmFixture fixture;

	fixture.setOverallBoundsAndSpacing(100, 5);
	fixture.getInputGenerator()->setSpherePhantom();
	QDomDocument domdoc;
	QDomElement settings = domdoc.createElement("VNNcl");
	cx::VNNclReconstructionMethodServicePtr algorithm = cx::VNNclReconstructionMethodService::create(pluginContext);
	algorithm->enableProfiling();

	QString name = "DefaultVNNcl";

	fixture.setAlgorithm(algorithm);
	algorithm->getRadiusOption(settings)->setValue(10);
	SECTION("VNN")
	{
		name = "VNN";
		std::cout << "Testing VNN\n";
		algorithm->getMethodOption(settings)->setValue("VNN");
		algorithm->getPlaneMethodOption(settings)->setValue("Heuristic");
		algorithm->getMaxPlanesOption(settings)->setValue(8);
		algorithm->getNStartsOption(settings)->setValue(1);
		REQUIRE(true);
	}
	SECTION("VNN2")
	{
		name = "VNN2";
		std::cout << "Testing VNN2\n";
		algorithm->getMethodOption(settings)->setValue("VNN2");
		algorithm->getPlaneMethodOption(settings)->setValue("Heuristic");
		algorithm->getMaxPlanesOption(settings)->setValue(8);
		algorithm->getNStartsOption(settings)->setValue(1);
		REQUIRE(true);
	}
	SECTION("DW")
	{
		name = "DW";
		std::cout << "Testing DW\n";
		algorithm->getMethodOption(settings)->setValue("DW");
		algorithm->getPlaneMethodOption(settings)->setValue("Heuristic");
		algorithm->getMaxPlanesOption(settings)->setValue(8);
		algorithm->getNStartsOption(settings)->setValue(1);
		REQUIRE(true);
	}
	SECTION("Anisotropic")
	{
		name = "Anisotropic";
		std::cout << "Testing Anisotropic\n";
		algorithm->getMethodOption(settings)->setValue("Anisotropic");
		algorithm->getPlaneMethodOption(settings)->setValue("Heuristic");
		algorithm->getMaxPlanesOption(settings)->setValue(8);
		algorithm->getNStartsOption(settings)->setValue(1);
		algorithm->getBrightnessWeightOption(settings)->setValue(0);
		algorithm->getNewnessWeightOption(settings)->setValue(0);
		REQUIRE(true);
	}
	SECTION("Multistart search")
	{
		name = "Multistart search";
		std::cout << "Testing multistart search\n";
		algorithm->getMethodOption(settings)->setValue("VNN");
		algorithm->getPlaneMethodOption(settings)->setValue("Heuristic");
		algorithm->getMaxPlanesOption(settings)->setValue(8);
		algorithm->getNStartsOption(settings)->setValue(5);
		REQUIRE(true);
	}

	fixture.reconstruct(settings);

	double executionTime = algorithm->getKernelExecutionTime();
	JenkinsMeasurement jenkins;
	jenkins.createOutput(name, QString::number(executionTime));

	fixture.checkRMSBelow(20.0);
	fixture.checkCentroidDifferenceBelow(1);
	fixture.checkMassDifferenceBelow(0.01);

	//need to be sure opencl thread is finished before shutting down Reporter,
	//or else we could get seg fault because og a callbackk from opencl to Reporter after it is shut down
	Utilities::sleep_sec(1);
	cx::Reporter::shutdown();
	cx::LogicManager::shutdown();
}
#endif//CX_USE_OPENCL_UTILITY


#ifdef CX_USE_OPENCL_UTILITY
TEST_CASE("ReconstructManager: VNNcl on real data", "[usreconstruction][integration][VNNcl][not_apple][unstable]")
{
	ReconstructionManagerTestFixture fixture;
	ReconstructRealTestData realData;
	cx::UsReconstructionServicePtr reconstructer = fixture.getManager();

//	reconstructer->init();
	reconstructer->selectData(realData.getSourceFilename());
	reconstructer->getParam("Algorithm")->setValueFromVariant("VNNcl");
	reconstructer->getParam("Angio data")->setValueFromVariant(false);
	reconstructer->getParam("Dual Angio")->setValueFromVariant(false);

	cx::VNNclReconstructionMethodService* algorithm;
	cx::ReconstructionMethodService* algorithmService = reconstructer->createAlgorithm();
	REQUIRE(algorithmService);
	algorithm = dynamic_cast<cx::VNNclReconstructionMethodService*>(algorithmService);
	REQUIRE(algorithm);// Check if we got the algorithm

	QDomElement algo = reconstructer->getSettings().getElement("algorithms", "VNNcl");
	algorithm->getRadiusOption(algo)->setValue(1.0);

	// First test with VNN
	algorithm->getMethodOption(algo)->setValue("VNN");
	algorithm->getPlaneMethodOption(algo)->setValue("Heuristic");
	algorithm->getMaxPlanesOption(algo)->setValue(1);
	algorithm->getNStartsOption(algo)->setValue(1);
	SECTION("VNN2")
	{
		algorithm->getMethodOption(algo)->setValue("VNN2");
		algorithm->getPlaneMethodOption(algo)->setValue("Heuristic");
		algorithm->getMaxPlanesOption(algo)->setValue(8);
	}
	SECTION("DW")
	{
		algorithm->getMethodOption(algo)->setValue("DW");
		algorithm->getPlaneMethodOption(algo)->setValue("Heuristic");
		algorithm->getMaxPlanesOption(algo)->setValue(8);
	}
	SECTION("Anisotropic")
	{
		algorithm->getMethodOption(algo)->setValue("Anisotropic");
		algorithm->getPlaneMethodOption(algo)->setValue("Heuristic");
		algorithm->getMaxPlanesOption(algo)->setValue(8);
	}
	SECTION("Multistart search")
	{
		algorithm->getMethodOption(algo)->setValue("VNN");
		algorithm->getNStartsOption(algo)->setValue(5);
	}
	SECTION("Closest")
	{
		algorithm->getMethodOption(algo)->setValue("VNN");
		algorithm->getPlaneMethodOption(algo)->setValue("Closest");
		algorithm->getMaxPlanesOption(algo)->setValue(8);
	}

	// run the reconstruction in the main thread
	fixture.reconstruct();
	// check validity of output:
	REQUIRE(fixture.getOutput().size()==1);
	realData.validateBModeData(fixture.getOutput()[0]);
}
#endif // CX_USE_OPENCL_UTILITY
} //namespace cxtest
