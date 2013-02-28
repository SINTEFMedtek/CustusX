/*
 * cxLogicManager.cpp
 *
 *  \date Jun 1, 2011
 *      \author christiana
 */

#include <cxLogicManager.h>

#include "cxServiceController.h"
#include "sscMessageManager.h"
#include "cxPatientService.h"
#include "cxVideoService.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxStateService.h"
#include "cxDataManager.h"
#include "cxRepManager.h"
#include "sscGPUImageBuffer.h"

namespace cx
{

// --------------------------------------------------------
LogicManager* LogicManager::mInstance = NULL; ///< static member
// --------------------------------------------------------

void LogicManager::initialize()
{
	LogicManager::initializeServices();
	LogicManager::getInstance();
}

void LogicManager::shutdown()
{
  delete mInstance;
  mInstance = NULL;

  LogicManager::shutdownServices();
}

void LogicManager::initializeServices()
{
	// resources layer
	ssc::MessageManager::initialize();

	// services layer
	cx::PatientService::initialize();
	cx::DataManager::initialize();
	cx::ToolManager::initializeObject();
	cx::VideoService::initialize();
	cx::ViewManager::createInstance();
	cx::StateService::getInstance();
	// init stateservice....

	// logic layer
	//cx::LogicManager::initialize();

	// gui layer:
	// inited by mainwindow construction in main()
}

void LogicManager::shutdownServices()
{
	// gui layer
	// already destroyed by mainwindow

	// old stuff - high level
	StateService::destroyInstance();
	ViewManager::destroyInstance();
	//  RegistrationManager::shutdown();
	RepManager::destroyInstance();

	// logic layer
	//cx::LogicManager::shutdown();

	// service layer
	cx::VideoService::shutdown();
	cx::ToolManager::shutdown();
	cx::DataManager::shutdown();
	cx::PatientService::shutdown();

	ssc::GPUImageBufferRepository::shutdown();
	ssc::MessageManager::shutdown();
	Settings::destroyInstance();
}


LogicManager* LogicManager::getInstance()
{
	if (!mInstance)
	{
		mInstance = new LogicManager;
	}
	return mInstance;
}


LogicManager::LogicManager()
{
	mServiceController.reset(new ServiceController);
}

LogicManager::~LogicManager()
{

}


}
