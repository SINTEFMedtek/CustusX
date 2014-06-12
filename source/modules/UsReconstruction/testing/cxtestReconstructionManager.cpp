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
#include "cxReconstructPreprocessor.h"
#include <vtkImageData.h>
#include "cxStringDataAdapterXml.h"
#include "cxReconstructionExecuter.h"

namespace cxtest
{

TEST_CASE("ReconstructManager: PNN on sphere","[unit][usreconstruction][synthetic][not_win32][pnn]")
{
	ReconstructionManagerTestFixture fixture;
	fixture.setVerbose(true);

	SyntheticReconstructInputPtr input(new SyntheticReconstructInput);
	input->setOverallBoundsAndSpacing(100, 5);
	input->setSpherePhantom();
	cx::USReconstructInputData inputData = input->generateSynthetic_USReconstructInputData();

	cx::ReconstructionManagerPtr reconstructer = fixture.getManager();
	reconstructer->selectData(inputData);
	reconstructer->getParams()->mAlgorithmAdapter->setValue("PNNService");
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

TEST_CASE("ReconstructManager: PNN on angio sphere","[unit][usreconstruction][synthetic][pnn][hide]")
{
	/** Test on a phantom containing a colored sphere and a gray sphere.
	  * Verify that the angio algo reconstructs only the colored, and the
	  * BMode reconstructs only the gray.
	  *
	  */
	ReconstructionManagerTestFixture fixture;
	fixture.setVerbose(false);

	SyntheticReconstructInputPtr input(new SyntheticReconstructInput);
	input->setOverallBoundsAndSpacing(100, 5);
	input->setSpherePhantom();
	cx::USReconstructInputData inputData = input->generateSynthetic_USReconstructInputData();

	cx::ReconstructionManagerPtr reconstructer = fixture.getManager();
	reconstructer->selectData(inputData);
	reconstructer->getParams()->mAlgorithmAdapter->setValue("PNNService");//default
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
	reconstructer->getParams()->mAlgorithmAdapter->setValue("PNNService");
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
	reconstructer->getParams()->mAlgorithmAdapter->setValue("PNNService");
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
	fixture.setVerbose(false);

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

	cx::ReconstructPreprocessorPtr preprocessor(new cx::ReconstructPreprocessor());
	cx::USReconstructInputData inputData = generator->generateSynthetic_USReconstructInputData();
	cx::ReconstructCore::InputParams par;
	preprocessor->initialize(par, inputData);

	std::vector<bool> angio(1, false);
	std::vector<cx::ProcessedUSInputDataPtr> processedInput = preprocessor->createProcessedInput(angio);
	REQUIRE(processedInput.size() == 1);

	{
		Eigen::Array3i dimFirstFrame = processedInput[0]->getDimensions();
		dimFirstFrame[2] = 1;
		INFO("Clip prect: " << probeDefinition.getClipRect_p());
		INFO(dimFirstFrame << " == " << Eigen::Array3i(processedInput[0]->getMask()->GetDimensions()));
		REQUIRE(dimFirstFrame.isApprox(Eigen::Array3i(processedInput[0]->getMask()->GetDimensions())));
	}
}

} // namespace cxtest


