#include "cxTest_UsReconstruction.h"

#include <vtkImageData.h>
#include "sscReconstructer.h"
#include "sscImage.h"
#include "sscThunderVNNReconstructAlgorithm.h"
#include "sscPNNReconstructAlgorithm.h"
#include "cxDataLocations.h"

//#include "cxToolConfigurationParser.h"

void TestUsReconstruction::setUp()
{
  // this stuff will be performed just before all tests in this class
}

void TestUsReconstruction::tearDown()
{
  // this stuff will be performed just after all tests in this class
}

void TestUsReconstruction::testConstructor()
{
  ssc::ReconstructerPtr reconstructer(new ssc::Reconstructer(ssc::XmlOptionFile(),""));
}

void TestUsReconstruction::testAngioReconstruction()
{
	std::cout << "testAngioReconstruction running" << std::endl;
  ssc::ReconstructerPtr reconstructer(new ssc::Reconstructer(ssc::XmlOptionFile(),""));
  QString filename = cx::DataLocations::getTestDataPath() + "/testing/USAngioTest.cx3/US_Acq/US-Acq_01_20110520T121038/US-Acq_01_20110520T121038.mhd";
  //reconstructer->mAlgorithmAdapter->setValue("PNN");//default
  reconstructer->mAngioAdapter->setValue(true);

  CPPUNIT_ASSERT(boost::shared_dynamic_cast<ssc::PNNReconstructAlgorithm>(reconstructer->mAlgorithm));// Check if we got the PNN algorithm
  boost::shared_dynamic_cast<ssc::PNNReconstructAlgorithm>(reconstructer->mAlgorithm)->mInterpolationStepsOption->setValue(1);

  reconstructer->selectData(filename);
  reconstructer->reconstruct();

  ssc::ImagePtr output = reconstructer->getOutput();

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
  ssc::ReconstructerPtr reconstructer(new ssc::Reconstructer(ssc::XmlOptionFile(),""));
  QString filename = cx::DataLocations::getTestDataPath() + "/testing/USAngioTest.cx3/US_Acq/US-Acq_01_20110520T121038/US-Acq_01_20110520T121038.mhd";
  reconstructer->mAlgorithmAdapter->setValue("ThunderVNN");
  CPPUNIT_ASSERT(boost::shared_dynamic_cast<ssc::ThunderVNNReconstructAlgorithm>(reconstructer->mAlgorithm));
  boost::shared_dynamic_cast<ssc::ThunderVNNReconstructAlgorithm>(reconstructer->mAlgorithm)->mProcessorOption->setValue("GPU"); // Fails for AMD (most macs)
//  boost::shared_dynamic_cast<ssc::ThunderVNNReconstructAlgorithm>(reconstructer->mAlgorithm)->mProcessorOption->setValue("CPU");
  reconstructer->selectData(filename);
  reconstructer->reconstruct();

  ssc::ImagePtr output = reconstructer->getOutput();

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

