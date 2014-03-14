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

#include <vtkImageData.h>
#include <QDomElement>

#include "sscReconstructAlgorithm.h"
#include "TordReconstruct/cxSimpleSyntheticVolume.h"
#include "catch.hpp"
#include "sscPNNReconstructAlgorithm.h"
#include "QFileInfo"
#include "sscDummyTool.h"

#ifdef CX_USE_OPENCL_UTILITY
#include "TordReconstruct/TordTest.h"
#endif

#include "sscMessageManager.h"
#include "cxtestReconstructAlgorithmFixture.h"
#include "cxtestUtilities.h"
#include "cxtestJenkinsMeasurement.h"

namespace cxtest
{

TEST_CASE("ReconstructAlgorithm: PNN on sphere","[unit][usreconstruction][synthetic][ca_rec1][ca_rec]")
{
	ReconstructAlgorithmFixture fixture;
	QDomDocument domdoc;
	QDomElement settings = domdoc.createElement("PNN");

	fixture.setOverallBoundsAndSpacing(100, 5);
	fixture.setVerbose(true);
	fixture.getInputGenerator()->setSpherePhantom();

	fixture.setAlgorithm(cx::PNNReconstructAlgorithm::create());
	fixture.reconstruct(settings);

	fixture.checkRMSBelow(20.0);
	fixture.checkCentroidDifferenceBelow(1);
	fixture.checkMassDifferenceBelow(0.01);

	if (fixture.getVerbose())
	{
		fixture.saveOutputToFile("/Users/christiana/test/sphere_rec.mhd");
		fixture.saveNominalOutputToFile("/Users/christiana/test/sphere_nom.mhd");
	}
}

TEST_CASE("ReconstructAlgorithm: PNN on sphere, tilt","[unit][usreconstruction][synthetic][ca_rec5][ca_rec]")
{
	QDomDocument domdoc;
	QDomElement settings = domdoc.createElement("PNN");

	ReconstructAlgorithmFixture fixture;
	fixture.setVerbose(true);

	SyntheticReconstructInputPtr generator = fixture.getInputGenerator();
	generator->defineProbeMovementSteps(40);
	generator->defineProbeMovementNormalizedTranslationRange(0.8);
	generator->defineProbeMovementAngleRange(M_PI/6);
	generator->defineProbe(cx::DummyToolTestUtilities::createProbeDataLinear(100, 100, Eigen::Array2i(150,150)));
	generator->setSpherePhantom();
	fixture.defineOutputVolume(100, 2);

	fixture.setAlgorithm(cx::PNNReconstructAlgorithm::create());
	fixture.reconstruct(settings);

	fixture.checkRMSBelow(30.0);
	fixture.checkCentroidDifferenceBelow(2);
	fixture.checkMassDifferenceBelow(0.01);

	if (fixture.getVerbose())
	{
		fixture.saveOutputToFile("/Users/christiana/test/sphere_rec.mhd");
		fixture.saveNominalOutputToFile("/Users/christiana/test/sphere_nom.mhd");
	}
}

//TEST_CASE("ReconstructAlgorithm: PNN on box+lines, tilt","[usreconstruction][synthetic][hide][ca_rec2]")
//{
//	ReconstructAlgorithmFixture fixture;
//	fixture.setBoxAndLinesPhantom();

//	fixture.setAlgorithm(cx::PNNReconstructAlgorithm::create());
//	fixture.reconstruct();

//	fixture.checkRMSBelow(15.0);
//	fixture.checkCentroidDifferenceBelow(2);
//	fixture.checkMassDifferenceBelow(0.05);

////	fixture.saveOutputToFile("/Users/christiana/test/boxlines_rec.mhd");
//}

#ifdef CX_USE_OPENCL_UTILITY
TEST_CASE("ReconstructAlgorithm: Tord/VNN on sphere","[unit][tordtest][usreconstruction][synthetic][not_win32][unstable][broken]")
{
	cx::messageManager()->initialize();

	ReconstructAlgorithmFixture fixture;

	fixture.setOverallBoundsAndSpacing(100, 5);
	fixture.getInputGenerator()->setSpherePhantom();
	QDomDocument domdoc;
	QDomElement settings = domdoc.createElement("TordTest");
	boost::shared_ptr<cx::TordTest> algorithm(new cx::TordTest);
	algorithm->enableProfiling();

	JenkinsMeasurement jenkins;
	jenkins.initialize();

	QString name = "DefaultTord";

	fixture.setAlgorithm(algorithm);
	algorithm->getRadiusOption(settings)->setValue(10);
	SECTION("VNN")
	{
		name = "VNN";
		std::cerr << "Testing VNN\n";
		algorithm->getMethodOption(settings)->setValue("VNN");
		algorithm->getPlaneMethodOption(settings)->setValue("Heuristic");
		algorithm->getMaxPlanesOption(settings)->setValue(8);
		algorithm->getNStartsOption(settings)->setValue(1);
	}
	SECTION("VNN2")
	{
		name = "VNN2";
		std::cerr << "Testing VNN2\n";
		algorithm->getMethodOption(settings)->setValue("VNN2");
		algorithm->getPlaneMethodOption(settings)->setValue("Heuristic");
		algorithm->getMaxPlanesOption(settings)->setValue(8);
		algorithm->getNStartsOption(settings)->setValue(1);
	}
	SECTION("DW")
	{
		name = "DW";
		std::cerr << "Testing DW\n";
		algorithm->getMethodOption(settings)->setValue("DW");
		algorithm->getPlaneMethodOption(settings)->setValue("Heuristic");
		algorithm->getMaxPlanesOption(settings)->setValue(8);
		algorithm->getNStartsOption(settings)->setValue(1);
	}
	SECTION("Anisotropic")
	{
		name = "Anisotropic";
		std::cerr << "Testing Anisotropic\n";
		algorithm->getMethodOption(settings)->setValue("Anisotropic");
		algorithm->getPlaneMethodOption(settings)->setValue("Heuristic");
		algorithm->getMaxPlanesOption(settings)->setValue(8);
		algorithm->getNStartsOption(settings)->setValue(1);
		algorithm->getBrightnessWeightOption(settings)->setValue(0);
		algorithm->getNewnessWeightOption(settings)->setValue(0);
	}
	SECTION("Multistart search")
	{
		name = "Multistart search";
		std::cerr << "Testing multistart search\n";
		algorithm->getMethodOption(settings)->setValue("VNN");
		algorithm->getPlaneMethodOption(settings)->setValue("Heuristic");
		algorithm->getMaxPlanesOption(settings)->setValue(8);
		algorithm->getNStartsOption(settings)->setValue(5);
	}

	fixture.reconstruct(settings);

	double executionTime = algorithm->getKernelExecutionTime();
	jenkins.createOutput(name, QString::number(executionTime));

	fixture.checkRMSBelow(20.0);
	fixture.checkCentroidDifferenceBelow(1);
	fixture.checkMassDifferenceBelow(0.01);

	//need to be sure opencl thread is finished before shutting down messagemanager,
	//or else we could get seg fault because og a callbackk from opencl to messagemAnager after it is shut down
	Utilities::sleep_sec(1);
	cx::messageManager()->shutdown();
}
#endif//CX_USE_OPENCL_UTILITY

} // namespace cxtest


