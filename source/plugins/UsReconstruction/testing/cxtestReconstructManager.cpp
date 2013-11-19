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
#include "sscDataAdapter.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"

#include "recConfig.h"
#ifdef SSC_USE_OpenCL
	#include "TordReconstruct/TordTest.h"
  #include "TordReconstruct/cxSimpleSyntheticVolume.h"
#endif // SSC_USE_OpenCL


#include "catch.hpp"

#include "cxtestUtilities.h"
#include "sscUSFrameData.h"
#include "sscDummyTool.h"
#include "cxImageDataContainer.h"
#include "cxUsReconstructionFileMaker.h"

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
#ifdef SSC_USE_OpenCL
	void testTordTest(); // Test Tord GPU VNN implementation
#endif // SSC_USE_OpenCL

	cx::ReconstructManagerPtr createManager();

private:
	void validateData(cx::ImagePtr output);
	/** Validate the bmode data output from the specific data set used.
	  */
	void validateBModeData(cx::ImagePtr bmodeOut);
	/** Validate the angio data output from the specific data set used.
	  */
	void validateAngioData(cx::ImagePtr angioOut);
	int getValue(cx::ImagePtr data, int x, int y, int z);
};

void ReconstructManagerTestFixture::setUp()
{
	cx::MessageManager::initialize();
	cx::cxDataManager::initialize();
	// this stuff will be performed just before all tests in this class
}

void ReconstructManagerTestFixture::tearDown()
{
	cx::cxDataManager::shutdown();
	cx::MessageManager::shutdown();
	// this stuff will be performed just after all tests in this class
}

void ReconstructManagerTestFixture::testSlerpInterpolation()
{
	//  ReconstructManagerPtr reconstructer(new ReconstructManager(XmlOptionFile(),""));

	//  ReconstructerPtr reconstructer(new Reconstructer(XmlOptionFile(),""));
	cx::ReconstructCorePtr reconstructer(new cx::ReconstructCore());

	cx::Transform3D a;
	cx::Transform3D b;

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

	cx::Transform3D c = cx::USReconstructInputDataAlgorithm::slerpInterpolate(a, b, t);
	//Transform3D c = reconstructer->interpolate(a, b, t);

	Eigen::Matrix3d goalm;
	goalm =
			Eigen::AngleAxisd(0/*M_PI / 6.0*/, Eigen::Vector3d::UnitX()) * //30 deg
			Eigen::AngleAxisd(M_PI / 180.0, Eigen::Vector3d::UnitY()) * // 1 deg
			Eigen::AngleAxisd(0/*M_PI / 180.0*5.0*/, Eigen::Vector3d::UnitZ()); // 5 deg

	cx::Transform3D goal;
	goal.matrix().block<3, 3>(0, 0) = goalm;
	goal.matrix().block<4, 1>(0, 3) = Eigen::Vector4d(5.0, 5.0, 5.0, 1.0);

	if (!cx::similar(c, goal))
	{
		std::cout << "result: "<< std::endl << c << std::endl;
		std::cout << "goal: "<< std::endl << goal << std::endl;
	}
	REQUIRE(cx::similar(c, goal));

	// Test if normalized = the column lengths are 1
	double norm = goal.matrix().block<3, 1>(0, 0).norm();
	//	std::cout << "norm: " << norm << std::endl;
	REQUIRE(cx::similar(norm, 1.0));
	norm = goal.matrix().block<3, 1>(0, 1).norm();
	REQUIRE(cx::similar(norm, 1.0));
	norm = goal.matrix().block<3, 1>(0, 2).norm();
	REQUIRE(cx::similar(norm, 1.0));
}

void ReconstructManagerTestFixture::testConstructor()
{
	cx::ReconstructManagerPtr reconstructer(new cx::ReconstructManager(cx::XmlOptionFile(),""));
}

cx::ReconstructManagerPtr ReconstructManagerTestFixture::createManager()
{
	//	std::cout << "testAngioReconstruction running" << std::endl;
	cx::XmlOptionFile settings;
	cx::ReconstructManagerPtr reconstructer(new cx::ReconstructManager(settings,""));

	reconstructer->setOutputBasePath(cx::DataLocations::getTestDataPath() + "/temp/");
	reconstructer->setOutputRelativePath("Images");

	return reconstructer;
}

void ReconstructManagerTestFixture::validateData(cx::ImagePtr output)
{
	REQUIRE(output->getModality().contains("US"));
	REQUIRE( output->getRange() != 0);//Just check if the output volume is empty

	vtkImageDataPtr volume = output->getGrayScaleVtkImageData();
	unsigned char* volumePtr = reinterpret_cast<unsigned char*>(volume->GetScalarPointer());
	REQUIRE(volumePtr); //Check if the pointer != NULL
}

void ReconstructManagerTestFixture::validateAngioData(cx::ImagePtr angioOut)
{
	this->validateData(angioOut);

	CHECK(angioOut->getImageType().contains("Angio"));

	// this is the wire phantom cross: fire samples along one line and one on the other.
	// visible in bmode, invisible in angio.
	CHECK(this->getValue(angioOut, 38, 146, 146) == 1);
	CHECK(this->getValue(angioOut, 94, 148, 135) == 1);
	CHECK(this->getValue(angioOut, 144, 152, 130) == 1);
	CHECK(this->getValue(angioOut, 237, 161, 119) == 1);
	CHECK(this->getValue(angioOut, 278, 160, 113) == 1);
	CHECK(this->getValue(angioOut, 248, 149, 200) == 1);

	// black points at random positions outside cross
	CHECK(this->getValue(angioOut, 242, 125, 200) == 1);
	CHECK(this->getValue(angioOut, 233, 138, 141) == 1);
	// one sample in a flash and a black sample just outside it.
	CHECK(this->getValue(angioOut, 143, 152, 172)  > 1); // correction
	CHECK(this->getValue(angioOut, 179, 142, 170) == 1); //
	// two samples in a flash and three black samples just outside it.
	CHECK(this->getValue(angioOut, 334,  96,  86) > 200 );
	CHECK(this->getValue(angioOut, 319,  95,  85) > 200 );
	CHECK(this->getValue(angioOut, 316, 105,  72) == 1);
	CHECK(this->getValue(angioOut, 317,  98,  44) == 1);
	CHECK(this->getValue(angioOut, 316, 108,  65) == 1);
}

void ReconstructManagerTestFixture::validateBModeData(cx::ImagePtr bmodeOut)
{
	this->validateData(bmodeOut);

	REQUIRE(bmodeOut->getImageType().contains("B-Mode"));

	// this is the wire phantom cross: fire samples along one line and one on the other.
	// visible in bmode, invisible in angio.
	CHECK(this->getValue(bmodeOut, 38, 146, 146) > 200);
	CHECK(this->getValue(bmodeOut, 94, 148, 135) > 200);
	CHECK(this->getValue(bmodeOut, 144, 152, 130) > 200);
	CHECK(this->getValue(bmodeOut, 237, 161, 119) > 200);
	CHECK(this->getValue(bmodeOut, 278, 160, 113) > 200);
	CHECK(this->getValue(bmodeOut, 248, 149, 200) > 200);

	// black points at random positions outside cross
	CHECK(this->getValue(bmodeOut, 242, 125, 200) == 1);
	CHECK(this->getValue(bmodeOut, 233, 138, 141) == 1);
	// one sample in a flash and a black sample just outside it.
	CHECK(this->getValue(bmodeOut, 143, 152, 172)  > 1); // correction
	CHECK(this->getValue(bmodeOut, 179, 142, 170) == 1); //
	// two samples in a flash and three black samples just outside it.
	CHECK(this->getValue(bmodeOut, 334,  96,  86) > 200 );
	CHECK(this->getValue(bmodeOut, 319,  95,  85) > 200 );
	CHECK(this->getValue(bmodeOut, 316, 105,  72) == 1);
	CHECK(this->getValue(bmodeOut, 317,  98,  44) == 1);
	CHECK(this->getValue(bmodeOut, 316, 108,  65) == 1);
}

int ReconstructManagerTestFixture::getValue(cx::ImagePtr data, int x, int y, int z)
{
	vtkImageDataPtr volume = data->getGrayScaleVtkImageData();
	int val = (int)*reinterpret_cast<unsigned char*>(volume->GetScalarPointer(x,y,z));
	return val;
}

void ReconstructManagerTestFixture::testAngioReconstruction()
{
	QString filename = cx::DataLocations::getTestDataPath() +
			"/testing/"
			"2012-10-24_12-39_Angio_i_US3.cx3/US_Acq/US-Acq_03_20121024T132330.mhd";

	cx::ReconstructManagerPtr reconstructer = this->createManager();
	reconstructer->selectData(filename);
	reconstructer->getParams()->mAlgorithmAdapter->setValue("PNN");//default
	reconstructer->getParams()->mAngioAdapter->setValue(true);
	reconstructer->getParams()->mCreateBModeWhenAngio->setValue(false);

	// set an algorithm-specific parameter
	QDomElement algo = reconstructer->getSettings().getElement("algorithms", "PNN");
	boost::shared_ptr<cx::PNNReconstructAlgorithm> algorithm;
	algorithm = boost::dynamic_pointer_cast<cx::PNNReconstructAlgorithm>(reconstructer->createAlgorithm());
	REQUIRE(algorithm);// Check if we got the PNN algorithm
	algorithm->getInterpolationStepsOption(algo)->setValue(1);

	// run the reconstruction in the main thread
	cx::ReconstructPreprocessorPtr preprocessor = reconstructer->createPreprocessor();
	std::vector<cx::ReconstructCorePtr> cores = reconstructer->createCores();
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

	cx::ReconstructManagerPtr reconstructer = this->createManager();
	reconstructer->selectData(filename);
	reconstructer->getParams()->mAlgorithmAdapter->setValue("ThunderVNN");
	reconstructer->getParams()->mAngioAdapter->setValue(false);
	reconstructer->getParams()->mCreateBModeWhenAngio->setValue(false);

	// set an algorithm-specific parameter
	QDomElement algo = reconstructer->getSettings().getElement("algorithms", "ThunderVNN");
	boost::shared_ptr<cx::ThunderVNNReconstructAlgorithm> algorithm;
	algorithm = boost::dynamic_pointer_cast<cx::ThunderVNNReconstructAlgorithm>(reconstructer->createAlgorithm());
	REQUIRE(algorithm);// Check if we got the VNN algorithm
	algorithm->getProcessorOption(algo)->setValue("GPU");

	// run the reconstruction in the main thread
	cx::ReconstructPreprocessorPtr preprocessor = reconstructer->createPreprocessor();
	std::vector<cx::ReconstructCorePtr> cores = reconstructer->createCores();
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
	cx::ReconstructManagerPtr reconstructer = this->createManager();
	reconstructer->selectData(filename);
	reconstructer->getParams()->mAlgorithmAdapter->setValue("PNN");//default
	reconstructer->getParams()->mAngioAdapter->setValue(true);
	reconstructer->getParams()->mCreateBModeWhenAngio->setValue(true);

	// set an algorithm-specific parameter
	QDomElement algo = reconstructer->getSettings().getElement("algorithms", "PNN");
	boost::shared_ptr<cx::PNNReconstructAlgorithm> algorithm;
	algorithm = boost::dynamic_pointer_cast<cx::PNNReconstructAlgorithm>(reconstructer->createAlgorithm());
	REQUIRE(algorithm);// Check if we got the PNN algorithm
	algorithm->getInterpolationStepsOption(algo)->setValue(1);

	// start threaded reconstruction
	std::vector<cx::ReconstructCorePtr> cores = reconstructer->startReconstruction();
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

#ifdef SSC_USE_OpenCL
void ReconstructManagerTestFixture::testTordTest()
{

	QString filename = cx::DataLocations::getTestDataPath() +
			"/testing/"
			"2012-10-24_12-39_Angio_i_US3.cx3/US_Acq/US-Acq_03_20121024T132330.mhd";

	cx::ReconstructManagerPtr reconstructer = this->createManager();
	reconstructer->selectData(filename);
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

	SECTION("Closest")
	{
		algorithm->getMethodOption(algo)->setValue("VNN");
		algorithm->getPlaneMethodOption(algo)->setValue("Closest");
		algorithm->getMaxPlanesOption(algo)->setValue(8);
	}
	
	// run the reconstruction in the main thread
	cx::ReconstructPreprocessorPtr preprocessor = reconstructer->createPreprocessor();
	std::vector<cx::ReconstructCorePtr> cores = reconstructer->createCores();
	REQUIRE(cores.size()==1);
	preprocessor->initializeCores(cores);
	cores[0]->reconstruct();
	
	// check validity of output:
	this->validateBModeData(cores[0]->getOutput());
}
#endif // SSC_USE_OpenCL


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
TEST_CASE("ReconstructManager: ThunderGPU Reconstruction", "[usreconstruction][integration][thunder][not_win32][not_apple]")
{
	ReconstructManagerTestFixture fixture;
	fixture.testThunderGPUReconstruction();
}
TEST_CASE("ReconstructManager: Dual Angio", "[usreconstruction][integration]")
{
	ReconstructManagerTestFixture fixture;
	fixture.testDualAngio();
}

#ifdef SSC_USE_OpenCL
// Note 20131017/CA: hidden beacuse it fails on jenkins. Fix test and unhide.
TEST_CASE("ReconstructManager: TordTest", "[usreconstruction][integration][tordtest][hide]")
{
	ReconstructManagerTestFixture fixture;
	fixture.testTordTest();
}
#endif // SSC_USE_OpenCL

void drawLineInImage(vtkImageDataPtr image, int value)
{
	unsigned char *imagePointer = static_cast<unsigned char*>(image->GetScalarPointer());
	unsigned int xDim = image->GetDimensions()[0];
	unsigned int yDim = image->GetDimensions()[1];
	unsigned int numFrames = image->GetDimensions()[2];
	for (unsigned i=0; i < numFrames; ++i)
	{
		unsigned int x = i%xDim;
		unsigned y = i%yDim;
		imagePointer[x + y*xDim + i*xDim*yDim] = value;
	}
}

cx::USReconstructInputData generateSyntheticUSBMode()
{
	cx::USReconstructInputData retval;
	retval.mFilename = "Synthetic_US_BMode";

	unsigned numFrames = 10;
	unsigned dim = 5;
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(dim,dim,numFrames), 100);

	drawLineInImage(image->getBaseVtkImageData(), 200);

//	cx::cxDataManager::initialize();
//	cx::DataLocations::setTestMode();
//	image->setFilePath("test.mhd");
//	cx::dataManager()->saveImage(image, cx::DataLocations::getTestDataPath());
//	std::cout << "Saved input test file to: " << cx::DataLocations::getTestDataPath() << "/" << image->getFilePath() << std::endl;
//	cx::DataManager::shutdown();

	cx::USFrameDataPtr us;
	us = cx::USFrameData::create(image);
	cx::ImageDataContainerPtr images = us->getImageContainer();
	REQUIRE(images->size() == numFrames);

	retval.mUsRaw = us;


	for (unsigned i=0; i < numFrames; ++i)
	{
		cx::TimedPosition frame;
		//Need to move along the x-axis. See sscTool.h
		cx::Transform3D transform = cx::createTransformTranslate(cx::Vector3D(i, 0, 0));
//		std::cout << "transform: " << transform << std::endl;
		frame.mTime = i;
		frame.mPos = transform;
		retval.mPositions.push_back(frame);
		retval.mFrames.push_back(frame);

		//TODO: Make sure retval.mFrames have correct values
//		cx::USReconstructInputDataAlgorithm::interpolateFramePositionsFromTracking(&retval);
		cx::USReconstructInputDataAlgorithm::transformFramePositionsTo_rMu(&retval);
	}

	cx::ProbeSector probeSector;
	double probeSize = dim-1;
	cx::ProbeData probeData = cx::DummyToolTestUtilities::createProbeDataLinear(probeSize, probeSize, Eigen::Array2i(dim, dim));
	probeSector.setData(probeData);
	retval.mProbeData = probeSector;

	retval.mProbeUid = "Synthetic test probe";
	retval.rMpr = cx::Transform3D::Identity();

	return retval;
}

TEST_CASE("ReconstructManager: Reconstructing using TordTest Anisotropic on syntetic data", "[usreconstruction][plugins][unit][tordtest][hide]")
{
	cx::USReconstructInputData inputData = generateSyntheticUSBMode();

	CHECK(inputData.getMask()->GetDimensions()[0] == inputData.mUsRaw->getDimensions()[0]);
	CHECK(inputData.getMask()->GetDimensions()[1] == inputData.mUsRaw->getDimensions()[1]);
//	CHECK(inputData.getMask()->GetDimensions()[2] == inputData.mUsRaw->getDimensions()[2]);

	ReconstructManagerTestFixture fixture;
	cx::ReconstructManagerPtr reconstructer = fixture.createManager();
	reconstructer->selectData(inputData);
	reconstructer->getParams()->mAlgorithmAdapter->setValue("TordTest");

	std::vector<DataAdapterPtr> options = reconstructer->getAlgoOptions();
	std::vector<DataAdapterPtr>::iterator it;
	for(it = options.begin(); it != options.end(); ++it)
	{
		if(it->get()->getValueName() == "Method")
		{
			cx::StringDataAdapterXmlPtr x = boost::dynamic_pointer_cast<cx::StringDataAdapterXml>(*it);
			x->setValue("Anisotropic");
		}
		if(it->get()->getValueName() == "PlaneMethod")
		{
			cx::StringDataAdapterXmlPtr x = boost::dynamic_pointer_cast<cx::StringDataAdapterXml>(*it);
			x->setValue("Heuristic");
		}
		if(it->get()->getValueName() == "MaxPlanes")
		{
			cx::DoubleDataAdapterXmlPtr x = boost::dynamic_pointer_cast<cx::DoubleDataAdapterXml>(*it);
			x->setValue(8);
		}
		if(it->get()->getValueName() == "Radius")
		{
			cx::DoubleDataAdapterXmlPtr x = boost::dynamic_pointer_cast<cx::DoubleDataAdapterXml>(*it);
			x->setValue(1);
		}
	}


	reconstructer->getParams()->mAngioAdapter->setValue(false);
	reconstructer->getParams()->mCreateBModeWhenAngio->setValue(false);

	reconstructer->createAlgorithm();

	// run the reconstruction in the main thread
	cx::ReconstructPreprocessorPtr preprocessor = reconstructer->createPreprocessor();
	std::vector<cx::ReconstructCorePtr> cores = reconstructer->createCores();
	REQUIRE(cores.size()==1);
	preprocessor->initializeCores(cores);
	cores[0]->reconstruct();

	cx::ImagePtr output = cores[0]->getOutput();

	REQUIRE(output);
}

TEST_CASE("ReconstructManager: B-Mode with synthetic data", "[usreconstruction][plugins][unit]")
{
	cx::USReconstructInputData inputData = generateSyntheticUSBMode();

	CHECK(inputData.getMask()->GetDimensions()[0] == inputData.mUsRaw->getDimensions()[0]);
	CHECK(inputData.getMask()->GetDimensions()[1] == inputData.mUsRaw->getDimensions()[1]);
//	CHECK(inputData.getMask()->GetDimensions()[2] == inputData.mUsRaw->getDimensions()[2]);

	ReconstructManagerTestFixture fixture;
	cx::ReconstructManagerPtr reconstructer = fixture.createManager();
	reconstructer->selectData(inputData);
	reconstructer->getParams()->mAlgorithmAdapter->setValue("PNN");//default
	reconstructer->getParams()->mAngioAdapter->setValue(false);
	reconstructer->getParams()->mCreateBModeWhenAngio->setValue(false);

	reconstructer->createAlgorithm();

	// run the reconstruction in the main thread
	cx::ReconstructPreprocessorPtr preprocessor = reconstructer->createPreprocessor();
	std::vector<cx::ReconstructCorePtr> cores = reconstructer->createCores();
	REQUIRE(cores.size()==1);
	preprocessor->initializeCores(cores);
	cores[0]->reconstruct();

	cx::ImagePtr output = cores[0]->getOutput();

	REQUIRE(output);
//	std::cout << "dims: " << Eigen::Array3i(output->getBaseVtkImageData()->GetDimensions()) << std::endl;
//	std::cout << "Num not zero voxels: " << Utilities::getNumberOfVoxelsAboveThreshold(output->getBaseVtkImageData(), 1) << std::endl;
//	std::cout << "Num mid intensity voxels: " << Utilities::getNumberOfVoxelsAboveThreshold(output->getBaseVtkImageData(), 99) << std::endl;
//	std::cout << "Num high intensity voxels: " << Utilities::getNumberOfVoxelsAboveThreshold(output->getBaseVtkImageData(), 199) << std::endl;

//	cx::cxDataManager::initialize();
//	cx::DataLocations::setTestMode();

//	cx::UsReconstructionFileMakerPtr fileMaker;
//	fileMaker.reset(new cx::UsReconstructionFileMaker("testFile"));
//	fileMaker->setReconstructData(inputData);
//	fileMaker->writeToNewFolder(cx::DataLocations::getTestDataPath() ,false);

//	cx::dataManager()->saveImage(output, cx::DataLocations::getTestDataPath());
//	std::cout << "Saved test file to: " << cx::DataLocations::getTestDataPath() << "/" << output->getFilePath() << std::endl;
//	cx::DataManager::shutdown();
}

#ifdef SSC_USE_OpenCL
TEST_CASE("ReconstructManager: With generated synthetic data","[usreconstruction][synthetic][hide]")
{
	
	Eigen::Array3i dims(100, 100, 100);
	cx::cxSimpleSyntheticVolume volume(dims);
	cx::TordTest algorithm;

	// FIXME: This should probably use the ReconstructManager somehow
	
	QDomDocument domDoc;
	QDomElement root = domDoc.createElement("TordTest");

	SECTION("VNN")
	{
		std::cerr << "Testing VNN\n";
		algorithm.getMethodOption(root)->setValue("VNN");
		algorithm.getPlaneMethodOption(root)->setValue("Heuristic");
		algorithm.getMaxPlanesOption(root)->setValue(8);
		algorithm.getRadiusOption(root)->setValue(1);
	}	
	SECTION("VNN2")
	{
		std::cerr << "Testing VNN2\n";
		algorithm.getMethodOption(root)->setValue("VNN2");
		algorithm.getPlaneMethodOption(root)->setValue("Heuristic");
		algorithm.getMaxPlanesOption(root)->setValue(8);
		algorithm.getRadiusOption(root)->setValue(1);
	}
	
	SECTION("DW")
	{
		std::cerr << "Testing DW\n";
		algorithm.getMethodOption(root)->setValue("DW");
		algorithm.getPlaneMethodOption(root)->setValue("Heuristic");
		algorithm.getMaxPlanesOption(root)->setValue(8);
		algorithm.getRadiusOption(root)->setValue(1);
	}
	SECTION("Anisotropic")
	{
		std::cerr << "Testing Anisotropic\n";
		algorithm.getMethodOption(root)->setValue("Anisotropic");
		algorithm.getPlaneMethodOption(root)->setValue("Heuristic");
		algorithm.getMaxPlanesOption(root)->setValue(8);
		algorithm.getRadiusOption(root)->setValue(1);
	}

	std::vector<cx::Transform3D> planes;

	for(int i = 0; i < 100; i++)
	{
		cx::Transform3D transform = cx::Transform3D::Identity();
		cx::Vector3D translation(0,0,i);
		transform.translation() = translation;
		transform.rotate(Eigen::AngleAxisd((double)(i-50)/100 *M_PI/8, Eigen::Vector3d::UnitY()));
		planes.push_back(transform);
	}
	Eigen::Array2f pixelSpacing(0.5f, 0.5f);
	Eigen::Array2i us_dims(200, 200);
	std::cout << "Starting samping\n";
	cx::ProcessedUSInputDataPtr usData = volume.sampleUsData(planes,
	                                                         pixelSpacing,
	                                                         us_dims,
	                                                         0.0,
	                                                         0.0);
	std::cout << "Done sampling\n";

	REQUIRE(usData);
	vtkImageDataPtr outputData = vtkImageDataPtr::New();
	outputData->SetExtent(0, 99, 0, 99, 0, 99);
	outputData->SetSpacing(1, 1, 1);
	std::cout << "Reconstructing\n";
	algorithm.reconstruct(usData,
	                      outputData,
	                      root);
	std::cout << "Reconstruction done\n";

	float sse = volume.computeRMSError(outputData);

	std::cout << "RMS value: " << sse << std::endl;
	REQUIRE(sse < 15.0f);

}
#endif

} // namespace cxtest


