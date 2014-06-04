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

#include "cxReconstructParams.h"
#include "cxBoolDataAdapterXml.h"
#include "catch.hpp"
#include "cxtestReconstructionManagerFixture.h"
#include "cxtestReconstructRealData.h"
#include "cxtestSyntheticReconstructInput.h"

#include "cxDummyTool.h"
#include "cxPNNReconstructAlgorithm.h"
#include "cxReconstructPreprocessor.h"
#include <vtkImageData.h>
#include "cxStringDataAdapterXml.h"
#include "recConfig.h"
#include "cxReconstructionExecuter.h"

#ifdef CX_USE_OPENCL_UTILITY
#include "TordReconstruct/TordTest.h"
#include "TordReconstruct/cxSimpleSyntheticVolume.h"
#endif // CX_USE_OPENCL_UTILITY

namespace cxtest
{

TEST_CASE("ReconstructManager: PNN on sphere","[unit][usreconstruction][synthetic][not_win32][ca_rec]")
{
	ReconstructionManagerTestFixture fixture;
	fixture.setVerbose(true);

	SyntheticReconstructInputPtr input(new SyntheticReconstructInput);
	input->setOverallBoundsAndSpacing(100, 5);
//	input->setOverallBoundsAndSpacing(100, 0.2);
	input->setSpherePhantom();
	cx::USReconstructInputData inputData = input->generateSynthetic_USReconstructInputData();

	cx::ReconstructionManagerPtr reconstructer = fixture.getManager();
	reconstructer->selectData(inputData);
	reconstructer->getParams()->mAlgorithmAdapter->setValue("PNN");//default
//	reconstructer->getParams()->mAngioAdapter->setValue(true);
	reconstructer->getParams()->mCreateBModeWhenAngio->setValue(false);
	// set an algorithm-specific parameter
	fixture.setPNN_InterpolationSteps(1);

	// run the reconstruction in the main thread
	fixture.reconstruct();

	// check validity of output:
	REQUIRE(fixture.getOutput().size()==1);

	SyntheticVolumeComparerPtr comparer = fixture.getComparerForOutput(input, 0);
	comparer->checkRMSBelow(30.0);
	comparer->checkCentroidDifferenceBelow(1);
	comparer->checkMassDifferenceBelow(0.01);
	// check the value in the sphere center:
	comparer->checkValueWithin(input->getPhantom()->getBounds()/2, 200, 255);

	if (comparer->getVerbose())
	{
		comparer->saveOutputToFile("sphere_recman.mhd");
		comparer->saveNominalOutputToFile("sphere_nomman.mhd");
	}
}

TEST_CASE("ReconstructManager: PNN on angio sphere","[unit][usreconstruction][synthetic][ca_rec7][ca_rec][hide]")
{
	/** Test on a phantom containing a colored sphere and a gray sphere.
	  * Verify that the angio algo reconstructs only the colored, and the
	  * BMode reconstructs only the gray.
	  *
	  */
	ReconstructionManagerTestFixture fixture;
	fixture.setVerbose(true);

	SyntheticReconstructInputPtr input(new SyntheticReconstructInput);
	input->setOverallBoundsAndSpacing(100, 5);
	input->setSpherePhantom();
	cx::USReconstructInputData inputData = input->generateSynthetic_USReconstructInputData();
//	REQUIRE(!input.mFrames.empty());
//	CHECK(input.mFrames[0]->);

	cx::ReconstructionManagerPtr reconstructer = fixture.getManager();
	reconstructer->selectData(inputData);
	reconstructer->getParams()->mAlgorithmAdapter->setValue("PNN");//default
	reconstructer->getParams()->mAngioAdapter->setValue(true);
	reconstructer->getParams()->mCreateBModeWhenAngio->setValue(false);
	// set an algorithm-specific parameter
	fixture.setPNN_InterpolationSteps(1);

	// run the reconstruction in the main thread
	fixture.reconstruct();

	// check validity of output:
	REQUIRE(fixture.getOutput().size()==1);

	SyntheticVolumeComparerPtr comparer = fixture.getComparerForOutput(input, 0);
	comparer->checkRMSBelow(30.0);
	comparer->checkCentroidDifferenceBelow(1);
	comparer->checkMassDifferenceBelow(0.01);
	// check the value in the sphere center:
	comparer->checkValueWithin(input->getPhantom()->getBounds()/2, 200, 255);

	if (comparer->getVerbose())
	{
		comparer->saveOutputToFile("sphere_recman.mhd");
		comparer->saveNominalOutputToFile("sphere_nomman.mhd");
	}
}

TEST_CASE("ReconstructManager: Angio Reconstruction on real data", "[usreconstruction][integration][not_win32]")
{
	ReconstructionManagerTestFixture fixture;
	ReconstructRealTestData realData;
	cx::ReconstructionManagerPtr reconstructer = fixture.getManager();

	reconstructer->selectData(realData.getSourceFilename());
	reconstructer->getParams()->mAlgorithmAdapter->setValue("PNN");
	reconstructer->getParams()->mAngioAdapter->setValue(true);
	reconstructer->getParams()->mCreateBModeWhenAngio->setValue(false);
	// set an algorithm-specific parameter
	fixture.setPNN_InterpolationSteps(1);

	// run the reconstruction in the main thread
	fixture.reconstruct();
	// check validity of output:
	REQUIRE(fixture.getOutput().size()==1);
	realData.validateAngioData(fixture.getOutput()[0]);
}

TEST_CASE("ReconstructManager: Threaded Dual Angio on real data", "[usreconstruction][integration][not_win32]")
{
	ReconstructionManagerTestFixture fixture;
	ReconstructRealTestData realData;
	cx::ReconstructionManagerPtr reconstructer = fixture.getManager();

	reconstructer->selectData(realData.getSourceFilename());
	reconstructer->getParams()->mAlgorithmAdapter->setValue("PNN");
	reconstructer->getParams()->mAngioAdapter->setValue(true);
	reconstructer->getParams()->mCreateBModeWhenAngio->setValue(true);
	// set an algorithm-specific parameter
	fixture.setPNN_InterpolationSteps(1);

	// run threaded reconstruction
	fixture.threadedReconstruct();
	// validate output
	REQUIRE(fixture.getOutput().size()==2);
	realData.validateBModeData(fixture.getOutput()[0]);
	realData.validateAngioData(fixture.getOutput()[1]);

}

TEST_CASE("ReconstructManager: Preprocessor handles too large clip rect","[integration][usreconstruction][synthetic][not_win32]")
{
	ReconstructionManagerTestFixture fixture;
	fixture.setVerbose(true);

	SyntheticReconstructInputPtr generator(new SyntheticReconstructInput);
	Eigen::Array2i frameSize = Eigen::Array2i(150,150);
	Eigen::Array2i extent = frameSize - 1;
	cx::ProbeDefinition probeDefinition = cx::DummyToolTestUtilities::createProbeDataLinear(100, 100, frameSize);

	//Adding 2 sections creates 3 runs: 1 with the simple case clip rect == extent, the other 2 with extent+1 and +500
	SECTION("Set clip rect just to large")
		probeDefinition.setClipRect_p(cx::DoubleBoundingBox3D(0, extent[0]+1, 0, extent[1]+1, 0, 0));
    SECTION("Set clip rect very large")
		probeDefinition.setClipRect_p(cx::DoubleBoundingBox3D(0, extent[0]+500, 0, extent[1]+500, 0, 0));

	generator->defineProbe(probeDefinition);

	generator->setSpherePhantom();
	cx::USReconstructInputData inputData = generator->generateSynthetic_USReconstructInputData();

	cx::ReconstructionManagerPtr reconstructer = fixture.getManager();
	reconstructer->selectData(inputData);
	reconstructer->getParams()->mAlgorithmAdapter->setValue("PNN");//default
	reconstructer->getParams()->mCreateBModeWhenAngio->setValue(false);
	fixture.setPNN_InterpolationSteps(1);// set an algorithm-specific parameter

	cx::ReconstructionExecuterPtr executor(new cx::ReconstructionExecuter);
//	cx::ReconstructPreprocessorPtr preprocessor = reconstructer->createPreprocessor();
	bool validInputData = true; //TODO should be checked in some way???
	cx::ReconstructPreprocessorPtr preprocessor = executor->createPreprocessor(reconstructer->createCoreParameters(), reconstructer->getSelectedFileData(), validInputData);
	REQUIRE(preprocessor);
//	std::vector<cx::ReconstructCorePtr> cores = reconstructer->createCores();
	bool createBModeWhenAngio = reconstructer->getParams()->mCreateBModeWhenAngio->getValue();
	std::vector<cx::ReconstructCorePtr> cores = executor->createCores(reconstructer->createAlgorithm(), reconstructer->createCoreParameters(), createBModeWhenAngio, validInputData);
	REQUIRE(!cores.empty());
	std::vector<cx::ProcessedUSInputDataPtr> processedInput = preprocessor->createProcessedInput(cores);

	REQUIRE(processedInput.size() == cores.size());
	{
		Eigen::Array3i dimFirstFrame = processedInput[0]->getDimensions();
		dimFirstFrame[2] = 1;
		INFO("Clip prect: " << probeDefinition.getClipRect_p());
		INFO(dimFirstFrame << " == " << Eigen::Array3i(processedInput[0]->getMask()->GetDimensions()));
		REQUIRE(dimFirstFrame.isApprox(Eigen::Array3i(processedInput[0]->getMask()->GetDimensions())));
	}
}

#ifdef CX_USE_OPENCL_UTILITY
TEST_CASE("ReconstructManager: TordTest on real data", "[usreconstruction][integration][tordtest][not_apple][unstable]")
{
	ReconstructionManagerTestFixture fixture;
	ReconstructRealTestData realData;
	cx::ReconstructionManagerPtr reconstructer = fixture.getManager();

	reconstructer->selectData(realData.getSourceFilename());
	reconstructer->getParams()->mAlgorithmAdapter->setValue("TordTest");
	reconstructer->getParams()->mAngioAdapter->setValue(false);
	reconstructer->getParams()->mCreateBModeWhenAngio->setValue(false);
	
	boost::shared_ptr<cx::TordTest> algorithm;
	algorithm = boost::dynamic_pointer_cast<cx::TordTest>(reconstructer->createAlgorithm());
	REQUIRE(algorithm);// Check if we got the algorithm

	QDomElement algo = reconstructer->getSettings().getElement("algorithms", "TordTest");
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


} // namespace cxtest


