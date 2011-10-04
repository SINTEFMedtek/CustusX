#include "cxTest_UsReconstruction.h"

#include "sscReconstructManager.h"
#include "sscImage.h"
#include "sscThunderVNNReconstructAlgorithm.h"
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

void TestUsReconstruction::testConstructor()
{
  ssc::ReconstructManagerPtr reconstructer(new ssc::ReconstructManager(ssc::XmlOptionFile(),""));
}

void TestUsReconstruction::testAngioReconstruction()
{
	std::cout << "testAngioReconstruction running" << std::endl;
  ssc::ReconstructManagerPtr reconstructer(new ssc::ReconstructManager(ssc::XmlOptionFile(),""));
  //QString filename = cx::DataLocations::getTestDataPath() + "/testing/USAngioTest.cx3/US_Acq/US-Acq_02_20110222T120553/US-Acq_02_20110222T120553.mhd";
  //QString filename = "/Users/olevs/Patients/USAngioTest.cx3/US_Acq/US-Acq_02_20110222T120553/US-Acq_02_20110222T120553.mhd";
  QString filename = cx::DataLocations::getTestDataPath() + "/testing/USAngioTest.cx3/US_Acq/US-Acq_01_20110520T121038/US-Acq_01_20110520T121038.mhd";
  reconstructer->getParams()->mAngioAdapter->setValue(true);
  reconstructer->selectData(filename);
  reconstructer->reconstruct();

  ssc::ImagePtr output = reconstructer->getOutput();

  CPPUNIT_ASSERT( output->getRange() != 0);//Just check if the output volume is empty
}

void TestUsReconstruction::testThunderGPUReconstruction()
{
  ssc::ReconstructManagerPtr reconstructer(new ssc::ReconstructManager(ssc::XmlOptionFile(),""));
  QString filename = cx::DataLocations::getTestDataPath() + "/testing/USAngioTest.cx3/US_Acq/US-Acq_01_20110520T121038/US-Acq_01_20110520T121038.mhd";
  reconstructer->getParams()->mAlgorithmAdapter->setValue("ThunderVNN");
  CPPUNIT_ASSERT(boost::shared_dynamic_cast<ssc::ThunderVNNReconstructAlgorithm>(reconstructer->getAlgorithm()));
  boost::shared_dynamic_cast<ssc::ThunderVNNReconstructAlgorithm>(reconstructer->getAlgorithm())->mProcessorOption->setValue("GPU");
  reconstructer->selectData(filename);
  reconstructer->reconstruct();

  ssc::ImagePtr output = reconstructer->getOutput();

  CPPUNIT_ASSERT( output->getRange() != 0);//Just check if the output volume is empty
}

