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

#include <QApplication>
#include <vtkImageData.h>
#include "sscReconstructManager.h"
#include "sscImage.h"
#include "sscThunderVNNReconstructAlgorithm.h"
#include "sscPNNReconstructAlgorithm.h"
#include "cxDataLocations.h"
#include "cxDataManager.h"
#include "sscReconstructPreprocessor.h"
#include "sscReconstructParams.h"
#include "cxTimedAlgorithm.h"
#include "cxUSReconstructInputDataAlgoritms.h"
#include "sscReconstructManager.h"

#include "catch.hpp"

namespace cxtest
{

/** 
 *
 *
 * \ingroup cxtest
 * \date june 25, 2013
 * \author christiana
 */

/**Unit tests that test the US reconstruction plugin
 */
class ReconstructManagerTestFixture
{
public:
	ReconstructManagerTestFixture() { this->setUp(); }
	~ReconstructManagerTestFixture() { this->tearDown(); }
	void setUp();
	void tearDown();

	void testSlerpInterpolation();///< Test position matrix slerp interpolation
	void testConstructor();///< Test reconstructer constructor
	void testAngioReconstruction();///< Test reconstruction of US angio data (#318)
	void testThunderGPUReconstruction();///< Test Thunder GPU reconstruction
	void testDualAngio();

private:
	ssc::ReconstructManagerPtr createManager();
	void validateData(ssc::ImagePtr output);
	/** Validate the bmode data output from the specific data set used.
	  */
	void validateBModeData(ssc::ImagePtr bmodeOut);
	/** Validate the angio data output from the specific data set used.
	  */
	void validateAngioData(ssc::ImagePtr angioOut);
	int getValue(ssc::ImagePtr data, int x, int y, int z);
};

void ReconstructManagerTestFixture::setUp()
{
	ssc::MessageManager::initialize();
	cx::DataManager::initialize();
	// this stuff will be performed just before all tests in this class
}

void ReconstructManagerTestFixture::tearDown()
{
	cx::DataManager::shutdown();
	ssc::MessageManager::shutdown();
	// this stuff will be performed just after all tests in this class
}

void ReconstructManagerTestFixture::testSlerpInterpolation()
{
	//  ssc::ReconstructManagerPtr reconstructer(new ssc::ReconstructManager(ssc::XmlOptionFile(),""));

	//  ssc::ReconstructerPtr reconstructer(new ssc::Reconstructer(ssc::XmlOptionFile(),""));
	ssc::ReconstructCorePtr reconstructer(new ssc::ReconstructCore());

	ssc::Transform3D a;
	ssc::Transform3D b;

	Eigen::Matrix3d am;
	am =
			Eigen::AngleAxisd(0/*M_PI / 3.0*/, Eigen::Vector3d::UnitX()) * //60 deg
			Eigen::AngleAxisd(M_PI / 180.0*2.0, Eigen::Vector3d::UnitY()) * // 2 deg
			Eigen::AngleAxisd(0/*M_PI / 180.0*10.0*/, Eigen::Vector3d::UnitZ()); // 10 deg

	a.matrix().block<3, 3>(0, 0) = am;
	a.matrix().block<4, 1>(0, 3) = Eigen::Vector4d(0.0, 0.0, 0.0, 1.0);

	Eigen::Matrix3d bm;
	bm =
			Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX()) * //0 deg
			Eigen::AngleAxisd(0, Eigen::Vector3d::UnitY()) *
			Eigen::AngleAxisd(0, Eigen::Vector3d::UnitZ());

	b.matrix().block<3, 3>(0, 0) = bm;
	b.matrix().block<4, 1>(0, 3) = Eigen::Vector4d(10.0, 10.0, 10.0, 1.0);

	double t = 0.5;

	ssc::Transform3D c = cx::USReconstructInputDataAlgorithm::slerpInterpolate(a, b, t);
	//ssc::Transform3D c = reconstructer->interpolate(a, b, t);

	Eigen::Matrix3d goalm;
	goalm =
			Eigen::AngleAxisd(0/*M_PI / 6.0*/, Eigen::Vector3d::UnitX()) * //30 deg
			Eigen::AngleAxisd(M_PI / 180.0, Eigen::Vector3d::UnitY()) * // 1 deg
			Eigen::AngleAxisd(0/*M_PI / 180.0*5.0*/, Eigen::Vector3d::UnitZ()); // 5 deg

	ssc::Transform3D goal;
	goal.matrix().block<3, 3>(0, 0) = goalm;
	goal.matrix().block<4, 1>(0, 3) = Eigen::Vector4d(5.0, 5.0, 5.0, 1.0);

	if (!ssc::similar(c, goal))
	{
		std::cout << "result: "<< std::endl << c << std::endl;
		std::cout << "goal: "<< std::endl << goal << std::endl;
	}
	REQUIRE(ssc::similar(c, goal));

	// Test if normalized = the column lengths are 1
	double norm = goal.matrix().block<3, 1>(0, 0).norm();
	//	std::cout << "norm: " << norm << std::endl;
	REQUIRE(ssc::similar(norm, 1.0));
	norm = goal.matrix().block<3, 1>(0, 1).norm();
	REQUIRE(ssc::similar(norm, 1.0));
	norm = goal.matrix().block<3, 1>(0, 2).norm();
	REQUIRE(ssc::similar(norm, 1.0));
}

void ReconstructManagerTestFixture::testConstructor()
{
	ssc::ReconstructManagerPtr reconstructer(new ssc::ReconstructManager(ssc::XmlOptionFile(),""));
}

ssc::ReconstructManagerPtr ReconstructManagerTestFixture::createManager()
{
	//	std::cout << "testAngioReconstruction running" << std::endl;
	ssc::XmlOptionFile settings;
	ssc::ReconstructManagerPtr reconstructer(new ssc::ReconstructManager(settings,""));

	reconstructer->setOutputBasePath(cx::DataLocations::getTestDataPath() + "/temp/");
	reconstructer->setOutputRelativePath("Images");

	return reconstructer;
}

void ReconstructManagerTestFixture::validateData(ssc::ImagePtr output)
{
	REQUIRE(output->getModality().contains("US"));
	REQUIRE( output->getRange() != 0);//Just check if the output volume is empty

	vtkImageDataPtr volume = output->getGrayScaleBaseVtkImageData();
	unsigned char* volumePtr = reinterpret_cast<unsigned char*>(volume->GetScalarPointer());
	REQUIRE(volumePtr); //Check if the pointer != NULL
}

void ReconstructManagerTestFixture::validateAngioData(ssc::ImagePtr angioOut)
{
	this->validateData(angioOut);

	CHECK(angioOut->getImageType().contains("Angio"));

	// this is the wire phantom cross: fire samples along one line and one on the other.
	// visible in bmode, invisible in angio.
	CHECK(this->getValue(angioOut, 38, 144, 146) == 1);
	CHECK(this->getValue(angioOut, 94, 145, 132) == 1);
	CHECK(this->getValue(angioOut, 145, 149, 129) == 1);
	CHECK(this->getValue(angioOut, 237, 158, 118) == 1);
	CHECK(this->getValue(angioOut, 278, 158, 110) == 1);
	CHECK(this->getValue(angioOut, 242, 146, 202) == 1	);
	// black points at random positions outside cross
	CHECK(this->getValue(angioOut, 242, 125, 200) == 1);
	CHECK(this->getValue(angioOut, 233, 138, 141) == 1);
	// one sample in a flash and a black sample just outside it.
	CHECK(this->getValue(angioOut, 143, 152, 170)  > 1);
	CHECK(this->getValue(angioOut, 179, 142, 170) == 1);
	// two samples in a flash and three black samples just outside it.
	CHECK(this->getValue(angioOut, 343,  94,  84) > 240 );
	CHECK(this->getValue(angioOut, 319,  92,  84) > 240 );
	CHECK(this->getValue(angioOut, 316, 105,  72) == 1);
	CHECK(this->getValue(angioOut, 317,  98,  44) == 1);
	CHECK(this->getValue(angioOut, 316, 108,  65) == 1);
}

void ReconstructManagerTestFixture::validateBModeData(ssc::ImagePtr bmodeOut)
{
	this->validateData(bmodeOut);

	REQUIRE(bmodeOut->getImageType().contains("B-Mode"));

	// this is the wire phantom cross: fire samples along one line and one on the other.
	// visible in bmode, invisible in angio.
	CHECK(this->getValue(bmodeOut, 38, 144, 146) > 200);
	CHECK(this->getValue(bmodeOut, 94, 145, 132) > 200);
	CHECK(this->getValue(bmodeOut, 145, 149, 129) > 200);
	CHECK(this->getValue(bmodeOut, 237, 158, 118) > 200);
	CHECK(this->getValue(bmodeOut, 278, 158, 110) > 200);
	CHECK(this->getValue(bmodeOut, 242, 146, 202) > 200);
	// black points at random positions outside cross
	CHECK(this->getValue(bmodeOut, 242, 125, 200) == 1);
	CHECK(this->getValue(bmodeOut, 233, 138, 141) == 1);
	// one sample in a flash and a black sample just outside it.
	CHECK(this->getValue(bmodeOut, 143, 152, 170)  > 1);
	CHECK(this->getValue(bmodeOut, 179, 142, 170) == 1);
	// two samples in a flash and three black samples just outside it.
	CHECK(this->getValue(bmodeOut, 343,  94,  84) > 240 );
	CHECK(this->getValue(bmodeOut, 319,  92,  84) > 240 );
	CHECK(this->getValue(bmodeOut, 316, 105,  72) == 1);
	CHECK(this->getValue(bmodeOut, 317,  98,  44) == 1);
	CHECK(this->getValue(bmodeOut, 316, 108,  65) == 1);
}

int ReconstructManagerTestFixture::getValue(ssc::ImagePtr data, int x, int y, int z)
{
	vtkImageDataPtr volume = data->getGrayScaleBaseVtkImageData();
	int val = (int)*reinterpret_cast<unsigned char*>(volume->GetScalarPointer(x,y,z));
	return val;
}

void ReconstructManagerTestFixture::testAngioReconstruction()
{
	QString filename = cx::DataLocations::getTestDataPath() +
			"/testing/"
			"2012-10-24_12-39_Angio_i_US3.cx3/US_Acq/US-Acq_03_20121024T132330.mhd";

	ssc::ReconstructManagerPtr reconstructer = this->createManager();
	reconstructer->selectData(filename);
	reconstructer->getParams()->mAlgorithmAdapter->setValue("PNN");//default
	reconstructer->getParams()->mAngioAdapter->setValue(true);
	reconstructer->getParams()->mCreateBModeWhenAngio->setValue(false);

	// set an algorithm-specific parameter
	QDomElement algo = reconstructer->getSettings().getElement("algorithms", "PNN");
	boost::shared_ptr<ssc::PNNReconstructAlgorithm> algorithm;
	algorithm = boost::dynamic_pointer_cast<ssc::PNNReconstructAlgorithm>(reconstructer->createAlgorithm());
	REQUIRE(algorithm);// Check if we got the PNN algorithm
	algorithm->getInterpolationStepsOption(algo)->setValue(1);

	// run the reconstruction in the main thread
	ssc::ReconstructPreprocessorPtr preprocessor = reconstructer->createPreprocessor();
	std::vector<ssc::ReconstructCorePtr> cores = reconstructer->createCores();
	REQUIRE(cores.size()==1);
	preprocessor->initializeCores(cores);
	cores[0]->reconstruct();

	// check validity of output:
	this->validateAngioData(cores[0]->getOutput());
}

void ReconstructManagerTestFixture::testThunderGPUReconstruction()
{
	QString filename = cx::DataLocations::getTestDataPath() +
			"/testing/"
			"2012-10-24_12-39_Angio_i_US3.cx3/US_Acq/US-Acq_03_20121024T132330.mhd";

	ssc::ReconstructManagerPtr reconstructer = this->createManager();
	reconstructer->selectData(filename);
	reconstructer->getParams()->mAlgorithmAdapter->setValue("ThunderVNN");
	reconstructer->getParams()->mAngioAdapter->setValue(false);
	reconstructer->getParams()->mCreateBModeWhenAngio->setValue(false);

	// set an algorithm-specific parameter
	QDomElement algo = reconstructer->getSettings().getElement("algorithms", "VNN");
	boost::shared_ptr<ssc::ThunderVNNReconstructAlgorithm> algorithm;
	algorithm = boost::dynamic_pointer_cast<ssc::ThunderVNNReconstructAlgorithm>(reconstructer->createAlgorithm());
	REQUIRE(algorithm);// Check if we got the VNN algorithm
	algorithm->getProcessorOption(algo)->setValue("GPU");

	// run the reconstruction in the main thread
	ssc::ReconstructPreprocessorPtr preprocessor = reconstructer->createPreprocessor();
	std::vector<ssc::ReconstructCorePtr> cores = reconstructer->createCores();
	REQUIRE(cores.size()==1);
	preprocessor->initializeCores(cores);
	cores[0]->reconstruct();

	// check validity of output:
	this->validateBModeData(cores[0]->getOutput());
}

void ReconstructManagerTestFixture::testDualAngio()
{
	QString filename = cx::DataLocations::getTestDataPath() +
			"/testing/"
			"2012-10-24_12-39_Angio_i_US3.cx3/US_Acq/US-Acq_03_20121024T132330.mhd";
	ssc::ReconstructManagerPtr reconstructer = this->createManager();
	reconstructer->selectData(filename);
	reconstructer->getParams()->mAlgorithmAdapter->setValue("PNN");//default
	reconstructer->getParams()->mAngioAdapter->setValue(true);
	reconstructer->getParams()->mCreateBModeWhenAngio->setValue(true);

	// set an algorithm-specific parameter
	QDomElement algo = reconstructer->getSettings().getElement("algorithms", "PNN");
	boost::shared_ptr<ssc::PNNReconstructAlgorithm> algorithm;
	algorithm = boost::dynamic_pointer_cast<ssc::PNNReconstructAlgorithm>(reconstructer->createAlgorithm());
	REQUIRE(algorithm);// Check if we got the PNN algorithm
	algorithm->getInterpolationStepsOption(algo)->setValue(1);

	// start threaded reconstruction
	std::vector<ssc::ReconstructCorePtr> cores = reconstructer->startReconstruction();
	REQUIRE(cores.size()==2);
	std::set<cx::TimedAlgorithmPtr> threads = reconstructer->getThreadedReconstruction();
	REQUIRE(threads.size()==1);
	cx::TimedAlgorithmPtr thread = *threads.begin();
	QObject::connect(thread.get(), SIGNAL(finished()), qApp, SLOT(quit()));
	qApp->exec();

	// threaded exec is now complete

	// validate output
	REQUIRE(thread->isFinished());
	REQUIRE(!thread->isRunning());

	this->validateBModeData(cores[0]->getOutput());
	this->validateAngioData(cores[1]->getOutput());
}


TEST_CASE("ReconstructManager: Slerp Interpolation", "[usreconstruction][unit]")
{
	ReconstructManagerTestFixture fixture;
	fixture.testSlerpInterpolation();
}
TEST_CASE("ReconstructManager: Angio Reconstruction", "[usreconstruction][integration]")
{
	ReconstructManagerTestFixture fixture;
	fixture.testAngioReconstruction();
}
TEST_CASE("ReconstructManager: ThunderGPU Reconstruction", "[usreconstruction][integration]")
{
	ReconstructManagerTestFixture fixture;
	fixture.testThunderGPUReconstruction();
}
TEST_CASE("ReconstructManager: Dual Angio", "[usreconstruction][integration]")
{
	ReconstructManagerTestFixture fixture;
	fixture.testDualAngio();
}


} // namespace cx


