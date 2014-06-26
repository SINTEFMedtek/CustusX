#include "catch.hpp"

#include "cxReporter.h"
#include "cxVNNclAlgorithm.h"
#include "cxVNNclReconstructionService.h"
#include "cxReconstructParams.h"
#include "cxBoolDataAdapterXml.h"
#include "cxtestUtilities.h"
#include "cxtestReconstructRealData.h"
#include "cxtestJenkinsMeasurement.h"
#include "cxtestReconstructionAlgorithmFixture.h"
#include "cxtestReconstructionManagerFixture.h"

#ifdef CX_USE_OPENCL_UTILITY
#include "cxSimpleSyntheticVolume.h"
#endif // CX_USE_OPENCL_UTILITY


namespace cxtest
{

#ifdef CX_USE_OPENCL_UTILITY
TEST_CASE("ReconstructAlgorithm: VNNcl on sphere","[unit][VNNcl][usreconstruction][synthetic][not_win32]")
{
	cx::Reporter::initialize();

	ReconstructionAlgorithmFixture fixture;

	fixture.setOverallBoundsAndSpacing(100, 5);
	fixture.getInputGenerator()->setSpherePhantom();
	QDomDocument domdoc;
	QDomElement settings = domdoc.createElement("VNNcl");
	cx::VNNclReconstructionServicePtr algorithm(new cx::VNNclReconstructionService);
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
}
#endif//CX_USE_OPENCL_UTILITY


#ifdef CX_USE_OPENCL_UTILITY
TEST_CASE("ReconstructManager: VNNcl on real data", "[usreconstruction][integration][VNNcl][not_apple][unstable]")
{
	ReconstructionManagerTestFixture fixture;
	ReconstructRealTestData realData;
	cx::ReconstructionManagerPtr reconstructer = fixture.getManager();

	reconstructer->init();
	reconstructer->selectData(realData.getSourceFilename());
	reconstructer->getParams()->mAlgorithmAdapter->setValue("VNNcl");
	reconstructer->getParams()->mAngioAdapter->setValue(false);
	reconstructer->getParams()->mCreateBModeWhenAngio->setValue(false);

	boost::shared_ptr<cx::VNNclReconstructionService> algorithm;
	cx::ReconstructionServicePtr algorithmService = reconstructer->createAlgorithm();
	REQUIRE(algorithmService);
	algorithm = boost::dynamic_pointer_cast<cx::VNNclReconstructionService>(algorithmService);
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
