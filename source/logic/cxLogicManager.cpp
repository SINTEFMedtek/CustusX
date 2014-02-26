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
#include "cxSettings.h"
#include "cxSpaceProviderImpl.h"
//#include "sscDataReaderWriter.h"
#include "cxDataFactory.h"
#include "cxVisualizationServiceBackend.h"
#include "cxVideoServiceBackend.h"

namespace cx
{

// --------------------------------------------------------
LogicManager* LogicManager::mInstance = NULL; ///< static member
// --------------------------------------------------------

LogicManager* logicManager()
{
	return LogicManager::getInstance();
}

void LogicManager::initialize()
{
//	LogicManager::initializeServices();
	LogicManager::getInstance()->initializeServices();
}

void LogicManager::shutdown()
{
	LogicManager::getInstance()->shutdownServices();

	delete mInstance;
	mInstance = NULL;
}

void LogicManager::initializeServices()
{
	// resources layer
	MessageManager::initialize();

	// services layer
	cx::cxDataManager::initialize();
	cx::PatientService::initialize();
	cx::cxToolManager::initializeObject();


	VideoServiceBackendPtr videoBackend;
	videoBackend = VideoServiceBackend::create(cx::DataManager::getInstance(),
											   cx::ToolManager::getInstance(),
											   mSpaceProvider);
	cx::VideoService::initialize(videoBackend);

//	cx::SpaceProviderPtr spaceProvider;
	mSpaceProvider.reset(new cx::SpaceProviderImpl(cx::cxToolManager::getInstance(),
												  cx::DataManager::getInstance()));
	cx::cxDataManager::getInstance()->setSpaceProvider(mSpaceProvider);

	mDataFactory.reset(new DataFactory(cx::cxDataManager::getInstance(), mSpaceProvider));
	cx::cxDataManager::getInstance()->setDataFactory(mDataFactory);

	VisualizationServiceBackendPtr vsBackend;
	vsBackend.reset(new VisualizationServiceBackend(cx::DataManager::getInstance(),
													cx::ToolManager::getInstance(),
													cx::VideoService::getInstance(),
													mSpaceProvider));
	cx::ViewManager::createInstance(vsBackend);
	cx::StateService::getInstance();
	// init stateservice....

	mServiceController.reset(new ServiceController);

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
	cx::cxToolManager::shutdown();
	cx::cxDataManager::shutdown();
	cx::PatientService::shutdown();

	GPUImageBufferRepository::shutdown();
	MessageManager::shutdown();
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
}

LogicManager::~LogicManager()
{

}

SpaceProviderPtr LogicManager::getSpaceProvider()
{
	return mSpaceProvider;
}
DataFactoryPtr LogicManager::getDataFactory()
{
	return mDataFactory;
}

}
