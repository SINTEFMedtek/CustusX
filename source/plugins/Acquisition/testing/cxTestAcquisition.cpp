#include "cxTestAcquisition.h"

#include <vtkImageData.h>
#include "sscReconstructManager.h"
#include "sscImage.h"
#include "cxDataLocations.h"
#include "cxDataManager.h"

//#include "cxToolConfigurationParser.h"

void TestAcquisition::setUp()
{
	ssc::MessageManager::initialize();
	cx::DataManager::initialize();
	// this stuff will be performed just before all tests in this class
}

void TestAcquisition::tearDown()
{
	cx::DataManager::shutdown();
	ssc::MessageManager::shutdown();
	// this stuff will be performed just after all tests in this class
}

void TestAcquisition::testConstructor()
{
//	ssc::ReconstructManagerPtr reconstructer(new ssc::ReconstructManager(ssc::XmlOptionFile(),""));
}
