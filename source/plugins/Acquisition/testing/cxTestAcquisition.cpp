#include "cxTestAcquisition.h"

#include <vtkImageData.h>
#include "sscReconstructManager.h"
#include "sscImage.h"
#include "cxDataLocations.h"
#include "cxDataManager.h"

#include "cxVideoService.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxLogicManager.h"
#include "cxStateService.h"
#include "cxRepManager.h"
#include "cxTestAcqController.h"

//#include "cxToolConfigurationParser.h"

void TestAcquisition::setUp()
{
	cx::LogicManager::initialize();
//	ssc::MessageManager::initialize();

//	// services layer
//	cx::VideoService::initialize();
//	cx::DataManager::initialize();
//	cx::ToolManager::initializeObject();
//	cx::ViewManager::createInstance();

//	// logic layer
//	cx::LogicManager::initialize();
}

void TestAcquisition::tearDown()
{
//	// old stuff - high level
//	cx::StateService::destroyInstance();
////	cx::ViewManager::destroyInstance();
//	//  RegistrationManager::shutdown();
//	cx::RepManager::destroyInstance();

	// logic layer
	cx::LogicManager::shutdown();

//	// service layer
//	cx::ToolManager::shutdown();
//	cx::DataManager::shutdown();
//	cx::VideoService::shutdown();

////	ssc::GPUImageBufferRepository::shutdown();
//	ssc::MessageManager::shutdown();
//	cx::Settings::destroyInstance();
}

void TestAcquisition::testConstructor()
{
//	ssc::ReconstructManagerPtr reconstructer(new ssc::ReconstructManager(ssc::XmlOptionFile(),""));
}

void TestAcquisition::testStoreMHDSource()
{
	TestAcqController controller(NULL);
	controller.initialize();
	QTimer::singleShot(20*1000,   qApp, SLOT(quit()) );
	qApp->exec();
	// start mhd file video source
	// start dummy probe ?
	// start acquisition
	// look for saved stuff (both inmem and ondisk)
	// turn on autoreconstruct and check that stuff is generated
	std::cout << "done" << std::endl;

}
