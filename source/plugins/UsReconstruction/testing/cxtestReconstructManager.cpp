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

#include "sscReconstructParams.h"
#include "sscBoolDataAdapterXml.h"
#include "catch.hpp"
#include "cxtestReconstructManagerFixture.h"
#include "cxtestReconstructRealData.h"
#include "cxtestSyntheticReconstructInput.h"

#include "recConfig.h"
#ifdef SSC_USE_OpenCL
	#include "TordReconstruct/TordTest.h"
  #include "TordReconstruct/cxSimpleSyntheticVolume.h"
#endif // SSC_USE_OpenCL

namespace cxtest
{

TEST_CASE("ReconstructManager: PNN on sphere","[unit][usreconstruction][synthetic][ca_rec6][ca_rec]")
{
	ReconstructManagerTestFixture fixture;
	fixture.setVerbose(true);

	SyntheticReconstructInputPtr input(new SyntheticReconstructInput);
	input->setOverallBoundsAndSpacing(100, 5);
//	input->setOverallBoundsAndSpacing(100, 0.2);
	input->setSpherePhantom();
	cx::USReconstructInputData inputData = input->generateSynthetic_USReconstructInputData();

	cx::ReconstructManagerPtr reconstructer = fixture.getManager();
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
	ReconstructManagerTestFixture fixture;
	fixture.setVerbose(true);

	SyntheticReconstructInputPtr input(new SyntheticReconstructInput);
	input->setOverallBoundsAndSpacing(100, 5);
	input->setSpherePhantom();
	cx::USReconstructInputData inputData = input->generateSynthetic_USReconstructInputData();
//	REQUIRE(!input.mFrames.empty());
//	CHECK(input.mFrames[0]->);

	cx::ReconstructManagerPtr reconstructer = fixture.getManager();
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

TEST_CASE("ReconstructManager: Angio Reconstruction on real data", "[usreconstruction][integration]")
{
	ReconstructManagerTestFixture fixture;
	ReconstructRealTestData realData;
	cx::ReconstructManagerPtr reconstructer = fixture.getManager();

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

TEST_CASE("ReconstructManager: Threaded Dual Angio on real data", "[usreconstruction][integration]")
{
	ReconstructManagerTestFixture fixture;
	ReconstructRealTestData realData;
	cx::ReconstructManagerPtr reconstructer = fixture.getManager();

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

#ifdef SSC_USE_OpenCL
TEST_CASE("ReconstructManager: TordTest on real data", "[usreconstruction][integration][tordtest][hide]")
{
	ReconstructManagerTestFixture fixture;
	ReconstructRealTestData realData;
	cx::ReconstructManagerPtr reconstructer = fixture.getManager();

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
#endif // SSC_USE_OpenCL


} // namespace cxtest


