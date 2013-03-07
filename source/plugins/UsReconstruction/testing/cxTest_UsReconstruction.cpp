#include "cxTest_UsReconstruction.h"

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

void TestUsReconstruction::setUp()
{
	ssc::MessageManager::initialize();
	cx::DataManager::initialize();
	// this stuff will be performed just before all tests in this class
}

void TestUsReconstruction::tearDown()
{
	cx::DataManager::shutdown();
	ssc::MessageManager::shutdown();
	// this stuff will be performed just after all tests in this class
}

void TestUsReconstruction::testSlerpInterpolation()
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
	CPPUNIT_ASSERT(ssc::similar(c, goal));

	// Test if normalized = the column lengths are 1
	double norm = goal.matrix().block<3, 1>(0, 0).norm();
	//	std::cout << "norm: " << norm << std::endl;
	CPPUNIT_ASSERT(ssc::similar(norm, 1.0));
	norm = goal.matrix().block<3, 1>(0, 1).norm();
	CPPUNIT_ASSERT(ssc::similar(norm, 1.0));
	norm = goal.matrix().block<3, 1>(0, 2).norm();
	CPPUNIT_ASSERT(ssc::similar(norm, 1.0));
}

void TestUsReconstruction::testConstructor()
{
	ssc::ReconstructManagerPtr reconstructer(new ssc::ReconstructManager(ssc::XmlOptionFile(),""));
}

ssc::ReconstructManagerPtr TestUsReconstruction::createManager()
{
	//	std::cout << "testAngioReconstruction running" << std::endl;
	ssc::XmlOptionFile settings;
	ssc::ReconstructManagerPtr reconstructer(new ssc::ReconstructManager(settings,""));

	reconstructer->setOutputBasePath(cx::DataLocations::getTestDataPath() + "/temp/");
	reconstructer->setOutputRelativePath("Images");

	return reconstructer;
}

void TestUsReconstruction::validateData(ssc::ImagePtr output)
{
	CPPUNIT_ASSERT(output->getModality().contains("US"));
	CPPUNIT_ASSERT( output->getRange() != 0);//Just check if the output volume is empty

	vtkImageDataPtr volume = output->getGrayScaleBaseVtkImageData();
	unsigned char* volumePtr = reinterpret_cast<unsigned char*>(volume->GetScalarPointer());
	CPPUNIT_ASSERT(volumePtr); //Check if the pointer != NULL
}

void TestUsReconstruction::validateAngioData(ssc::ImagePtr angioOut)
{
	this->validateData(angioOut);

	CPPUNIT_ASSERT(angioOut->getImageType().contains("Angio"));

	// this is the wire phantom cross: fire samples along one line and one on the other.
	// visible in bmode, invisible in angio.
	CPPUNIT_ASSERT(this->getValue(angioOut, 38, 144, 146) == 1);
	CPPUNIT_ASSERT(this->getValue(angioOut, 94, 145, 132) == 1);
	CPPUNIT_ASSERT(this->getValue(angioOut, 145, 149, 129) == 1);
	CPPUNIT_ASSERT(this->getValue(angioOut, 237, 158, 118) == 1);
	CPPUNIT_ASSERT(this->getValue(angioOut, 278, 158, 110) == 1);
	CPPUNIT_ASSERT(this->getValue(angioOut, 242, 146, 202) == 1	);
	// black points at random positions outside cross
	CPPUNIT_ASSERT(this->getValue(angioOut, 242, 125, 200) == 1);
	CPPUNIT_ASSERT(this->getValue(angioOut, 233, 138, 141) == 1);
	// one sample in a flash and a black sample just outside it.
	CPPUNIT_ASSERT(this->getValue(angioOut, 143, 152, 170)  > 1);
	CPPUNIT_ASSERT(this->getValue(angioOut, 179, 142, 170) == 1);
	// two samples in a flash and three black samples just outside it.
	CPPUNIT_ASSERT(this->getValue(angioOut, 343,  94,  84) > 240 );
	CPPUNIT_ASSERT(this->getValue(angioOut, 319,  92,  84) > 240 );
	CPPUNIT_ASSERT(this->getValue(angioOut, 316, 105,  72) == 1);
	CPPUNIT_ASSERT(this->getValue(angioOut, 317,  98,  44) == 1);
	CPPUNIT_ASSERT(this->getValue(angioOut, 316, 108,  65) == 1);
}

void TestUsReconstruction::validateBModeData(ssc::ImagePtr bmodeOut)
{
	this->validateData(bmodeOut);

	CPPUNIT_ASSERT(bmodeOut->getImageType().contains("B-Mode"));

	// this is the wire phantom cross: fire samples along one line and one on the other.
	// visible in bmode, invisible in angio.
	CPPUNIT_ASSERT(this->getValue(bmodeOut, 38, 144, 146) > 200);
	CPPUNIT_ASSERT(this->getValue(bmodeOut, 94, 145, 132) > 200);
	CPPUNIT_ASSERT(this->getValue(bmodeOut, 145, 149, 129) > 200);
	CPPUNIT_ASSERT(this->getValue(bmodeOut, 237, 158, 118) > 200);
	CPPUNIT_ASSERT(this->getValue(bmodeOut, 278, 158, 110) > 200);
	CPPUNIT_ASSERT(this->getValue(bmodeOut, 242, 146, 202) > 200);
	// black points at random positions outside cross
	CPPUNIT_ASSERT(this->getValue(bmodeOut, 242, 125, 200) == 1);
	CPPUNIT_ASSERT(this->getValue(bmodeOut, 233, 138, 141) == 1);
	// one sample in a flash and a black sample just outside it.
	CPPUNIT_ASSERT(this->getValue(bmodeOut, 143, 152, 170)  > 1);
	CPPUNIT_ASSERT(this->getValue(bmodeOut, 179, 142, 170) == 1);
	// two samples in a flash and three black samples just outside it.
	CPPUNIT_ASSERT(this->getValue(bmodeOut, 343,  94,  84) > 240 );
	CPPUNIT_ASSERT(this->getValue(bmodeOut, 319,  92,  84) > 240 );
	CPPUNIT_ASSERT(this->getValue(bmodeOut, 316, 105,  72) == 1);
	CPPUNIT_ASSERT(this->getValue(bmodeOut, 317,  98,  44) == 1);
	CPPUNIT_ASSERT(this->getValue(bmodeOut, 316, 108,  65) == 1);
}

int TestUsReconstruction::getValue(ssc::ImagePtr data, int x, int y, int z)
{
	vtkImageDataPtr volume = data->getGrayScaleBaseVtkImageData();
	int val = (int)*reinterpret_cast<unsigned char*>(volume->GetScalarPointer(x,y,z));
	return val;
}

void TestUsReconstruction::testAngioReconstruction()
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
	algorithm = boost::shared_dynamic_cast<ssc::PNNReconstructAlgorithm>(reconstructer->createAlgorithm());
	CPPUNIT_ASSERT(algorithm);// Check if we got the PNN algorithm
	algorithm->getInterpolationStepsOption(algo)->setValue(1);

	// run the reconstruction in the main thread
	ssc::ReconstructPreprocessorPtr preprocessor = reconstructer->createPreprocessor();
	std::vector<ssc::ReconstructCorePtr> cores = reconstructer->createCores();
	CPPUNIT_ASSERT(cores.size()==1);
	preprocessor->initializeCores(cores);
	cores[0]->reconstruct();

	// check validity of output:
	this->validateAngioData(cores[0]->getOutput());
}

void TestUsReconstruction::testThunderGPUReconstruction()
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
	algorithm = boost::shared_dynamic_cast<ssc::ThunderVNNReconstructAlgorithm>(reconstructer->createAlgorithm());
	CPPUNIT_ASSERT(algorithm);// Check if we got the VNN algorithm
	algorithm->getProcessorOption(algo)->setValue("GPU");

	// run the reconstruction in the main thread
	ssc::ReconstructPreprocessorPtr preprocessor = reconstructer->createPreprocessor();
	std::vector<ssc::ReconstructCorePtr> cores = reconstructer->createCores();
	CPPUNIT_ASSERT(cores.size()==1);
	preprocessor->initializeCores(cores);
	cores[0]->reconstruct();

	// check validity of output:	
	this->validateBModeData(cores[0]->getOutput());
}

void TestUsReconstruction::testDualAngio()
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
	algorithm = boost::shared_dynamic_cast<ssc::PNNReconstructAlgorithm>(reconstructer->createAlgorithm());
	CPPUNIT_ASSERT(algorithm);// Check if we got the PNN algorithm
	algorithm->getInterpolationStepsOption(algo)->setValue(1);

	// start threaded reconstruction
	std::vector<ssc::ReconstructCorePtr> cores = reconstructer->startReconstruction();
	CPPUNIT_ASSERT(cores.size()==2);
	std::set<cx::TimedAlgorithmPtr> threads = reconstructer->getThreadedReconstruction();
	CPPUNIT_ASSERT(threads.size()==1);
	cx::TimedAlgorithmPtr thread = *threads.begin();
	QObject::connect(thread.get(), SIGNAL(finished()), qApp, SLOT(quit()));
	qApp->exec();

	// threaded exec is now complete

	// validate output
	CPPUNIT_ASSERT(thread->isFinished());
	CPPUNIT_ASSERT(!thread->isRunning());

	this->validateBModeData(cores[0]->getOutput());
	this->validateAngioData(cores[1]->getOutput());
}


