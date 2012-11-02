#include "cxTest_UsReconstruction.h"

#include <vtkImageData.h>
#include "sscReconstructer.h"
#include "sscReconstructManager.h"
#include "sscImage.h"
#include "sscThunderVNNReconstructAlgorithm.h"
#include "sscPNNReconstructAlgorithm.h"
#include "cxDataLocations.h"
#include "sscReconstructer.h"

//#include "cxToolConfigurationParser.h"

void TestUsReconstruction::setUp()
{
  // this stuff will be performed just before all tests in this class
}

void TestUsReconstruction::tearDown()
{
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

	ssc::Transform3D c = reconstructer->slerpInterpolate(a, b, t);
	//ssc::Transform3D c = reconstructer->interpolate(a, b, t);

	Eigen::Matrix3d goalm;
	goalm =
			Eigen::AngleAxisd(0/*M_PI / 6.0*/, Eigen::Vector3d::UnitX()) * //30 deg
			Eigen::AngleAxisd(M_PI / 180.0, Eigen::Vector3d::UnitY()) * // 1 deg
			Eigen::AngleAxisd(0/*M_PI / 180.0*5.0*/, Eigen::Vector3d::UnitZ()); // 5 deg

	ssc::Transform3D goal;
	goal.matrix().block<3, 3>(0, 0) = goalm;
	goal.matrix().block<4, 1>(0, 3) = Eigen::Vector4d(5.0, 5.0, 5.0, 1.0);

	//if (!ssc::similar(c, goal))
	//{
		std::cout << "result: "<< std::endl << c << std::endl;
		std::cout << "goal: "<< std::endl << goal << std::endl;
	//}
	CPPUNIT_ASSERT(ssc::similar(c, goal));

	// Test if normalized = the column lengths are 1
	double norm = goal.matrix().block<3, 1>(0, 0).norm();
	std::cout << "norm: " << norm << std::endl;
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

void TestUsReconstruction::testAngioReconstruction()
{
	std::cout << "testAngioReconstruction running" << std::endl;
	ssc::XmlOptionFile settings;
  ssc::ReconstructManagerPtr reconstructer(new ssc::ReconstructManager(settings,""));
	QString filename = cx::DataLocations::getTestDataPath() + "/testing/USAngioTest.cx3/US_Acq/US-Acq_01_20110520T121038/US-Acq_01_20110520T121038.mhd";
  reconstructer->getParams()->mAlgorithmAdapter->setValue("PNN");//default
  reconstructer->getParams()->mAngioAdapter->setValue(true);

	QDomElement algo = settings.getElement("algorithms", "PNN");
//  CPPUNIT_ASSERT(boost::shared_dynamic_cast<ssc::PNNReconstructAlgorithm>(reconstructer->getAlgorithm()));// Check if we got the PNN algorithm
//  boost::shared_dynamic_cast<ssc::PNNReconstructAlgorithm>(reconstructer->getAlgorithm())->getInterpolationStepsOption(algo)->setValue(1);

  reconstructer->selectData(filename);
//  reconstructer->reconstruct();
  ssc::ReconstructCorePtr core = reconstructer->getReconstructer()->createCore();

  boost::shared_ptr<ssc::PNNReconstructAlgorithm> algorithm = boost::shared_dynamic_cast<ssc::PNNReconstructAlgorithm>(core->createAlgorithm("PNN"));
  CPPUNIT_ASSERT(algorithm);// Check if we got the PNN algorithm
  algorithm->getInterpolationStepsOption(algo)->setValue(1);

  ssc::ImagePtr output = core->reconstruct();

  CPPUNIT_ASSERT( output->getRange() != 0);//Just check if the output volume is empty

  vtkImageDataPtr volume = output->getGrayScaleBaseVtkImageData();
  unsigned char* volumePtr = reinterpret_cast<unsigned char*>(volume->GetScalarPointer());
  CPPUNIT_ASSERT(volumePtr); //Check if the pointer != NULL

  int* dimensions = volume->GetDimensions();
  // inside angio area
  int x = 133;
  int y = 130;
  int z = 81;
  int position = x + y * dimensions[0] + z * dimensions[0]*dimensions[1];
  CPPUNIT_ASSERT( volumePtr[position] !=0 );//Check if the voxel value is != zero inside the angio area

  // outside angio
  x = 60;
  y = 60;
  z = 146;
  position = x + y * dimensions[0] + z * dimensions[0]*dimensions[1];
  CPPUNIT_ASSERT( volumePtr[position] ==0 );//Check if the voxel value is zero outside the angio area
}

void TestUsReconstruction::testThunderGPUReconstruction()
{
	ssc::XmlOptionFile settings;
  ssc::ReconstructManagerPtr reconstructer(new ssc::ReconstructManager(settings,""));
  QString filename = cx::DataLocations::getTestDataPath() + "/testing/USAngioTest.cx3/US_Acq/US-Acq_01_20110520T121038/US-Acq_01_20110520T121038.mhd";
  reconstructer->getParams()->mAlgorithmAdapter->setValue("ThunderVNN");
//  CPPUNIT_ASSERT(boost::shared_dynamic_cast<ssc::ThunderVNNReconstructAlgorithm>(reconstructer->getAlgorithm()));

	QDomElement algo = settings.getElement("algorithms", "ThunderVNN");
//  boost::shared_dynamic_cast<ssc::ThunderVNNReconstructAlgorithm>(reconstructer->getAlgorithm())->getProcessorOption(algo)->setValue("GPU");// Fails for AMD (most macs)
//  boost::shared_dynamic_cast<ssc::ThunderVNNReconstructAlgorithm>(reconstructer->getAlgorithm)->mProcessorOption->setValue("CPU");
  reconstructer->selectData(filename);
//  reconstructer->reconstruct();
  ssc::ReconstructCorePtr core = reconstructer->getReconstructer()->createCore();

  boost::shared_ptr<ssc::ThunderVNNReconstructAlgorithm> algorithm;
  algorithm = boost::shared_dynamic_cast<ssc::ThunderVNNReconstructAlgorithm>(core->createAlgorithm("ThunderVNN"));
  CPPUNIT_ASSERT(algorithm);// Check if we got the PNN algorithm
  algorithm->getProcessorOption(algo)->setValue("GPU");

  ssc::ImagePtr output = core->reconstruct();

//  ssc::ImagePtr output = reconstructer->getOutput();

  CPPUNIT_ASSERT( output->getRange() != 0);//Just check if the output volume is empty

  // Check two voxel values
  vtkImageDataPtr volume = output->getGrayScaleBaseVtkImageData();
  unsigned char* volumePtr = reinterpret_cast<unsigned char*>(volume->GetScalarPointer());
  CPPUNIT_ASSERT(volumePtr); //Check if the pointer != NULL

  int* dimensions = volume->GetDimensions();
  // outside angio
  int x = 60;
  int y = 60;
  int z = 146;
  int position = x + y * dimensions[0] + z * dimensions[0]*dimensions[1];
  CPPUNIT_ASSERT( volumePtr[position] !=0 );//Check if the voxel value is != zero outside the angio area

  // inside angio
  x = 133;
  y = 130;
  z = 81;
  position = x + y * dimensions[0] + z * dimensions[0]*dimensions[1];
  CPPUNIT_ASSERT( volumePtr[position] !=0 );//Check if the voxel value is != zero inside the angio area
}

