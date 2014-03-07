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
#include "cxDataFactory.h"
#include "cxVisualizationServiceBackend.h"
#include "cxVideoServiceBackend.h"
#include "cxStateServiceBackend.h"

namespace cx
{

struct LegacySingletons
{
	static TrackingServicePtr mToolManager;
	static DataServicePtr mDataManager;
	static SpaceProviderPtr mSpaceProvider;
};

TrackingServicePtr LegacySingletons::mToolManager;
DataServicePtr LegacySingletons::mDataManager;
SpaceProviderPtr LegacySingletons::mSpaceProvider;

ToolManager* toolManager()
{
	return LegacySingletons::mToolManager.get(); // TODO remove get()
}
DataManager* dataManager()
{
	return LegacySingletons::mDataManager.get(); // TODO remove get()
}

TrackingServicePtr trackingService()
{
	return LegacySingletons::mToolManager;
}
SpaceProviderPtr spaceProvider()
{
	return LegacySingletons::mSpaceProvider;
}



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
//	cx::cxDataManager::initialize();
//	cx::PatientService::initialize();
//	cx::cxToolManager::initializeObject();

	this->getPatientService();
	this->getTrackingService();
	this->getVideoService();
	this->getStateService();
	this->getVisualizationService();
	this->getSpaceProvider();

//	VideoServiceBackendPtr videoBackend;
//	videoBackend = VideoServiceBackend::create(cx::DataManager::getInstance(),
//											   cx::ToolManager::getInstance(),
//											   mSpaceProvider);
//	cx::VideoService::initialize(videoBackend);

//	cx::SpaceProviderPtr spaceProvider;
//	mSpaceProvider.reset(new cx::SpaceProviderImpl(cx::cxToolManager::getInstance(),
//												  cx::DataManager::getInstance()));
//	cx::cxDataManager::getInstance()->setSpaceProvider(mSpaceProvider);

//	mDataFactory.reset(new DataFactory(cx::cxDataManager::getInstance(), mSpaceProvider));
//	cx::cxDataManager::getInstance()->setDataFactory(mDataFactory);

//	VisualizationServiceBackendPtr vsBackend;
//	vsBackend.reset(new VisualizationServiceBackend(cx::DataManager::getInstance(),
//													cx::ToolManager::getInstance(),
//													cx::VideoService::getInstance(),
//													mSpaceProvider));
//	cx::ViewManager::createInstance(vsBackend);

//	StateServiceBackendPtr ssBackend;
//	ssBackend.reset(new StateServiceBackend(cx::DataManager::getInstance(),
//													cx::ToolManager::getInstance(),
//													cx::VideoService::getInstance(),
//													mSpaceProvider,
//											cx::PatientService::getInstance()));
//	cx::StateService::createInstance(ssBackend);
//	// init stateservice....

	mServiceController.reset(new ServiceController);

	// logic layer
	//cx::LogicManager::initialize();

	// gui layer:
	// inited by mainwindow construction in main()
}

void LogicManager::createTrackingService()
{
	mTrackingService = cxToolManager::create();
	LegacySingletons::mToolManager = mTrackingService;
}

void LogicManager::createInterconnectedDataAndSpace()
{
	// prerequisites:
	this->getTrackingService();

	// build object(s):
//	mPatientService = PatientService::create();
//	LegacySingletons::mDataManager = mPatientService->getDataService();

//	cx::cxDataManager::initialize();
//	cx::PatientService::initialize();
//	cx::cxToolManager::initializeObject();

	mSpaceProvider.reset(new cx::SpaceProviderImpl(mTrackingService,
												  cx::DataManager::getInstance()));
	cx::cxDataManager::getInstance()->setSpaceProvider(mSpaceProvider);

	mDataFactory.reset(new DataFactory(cx::cxDataManager::getInstance(), mSpaceProvider));
	cx::cxDataManager::getInstance()->setDataFactory(mDataFactory);
}

void LogicManager::createDataFactory()
{
	this->createInterconnectedDataAndSpace();
}

void LogicManager::createPatientService()
{
	this->createInterconnectedDataAndSpace();
}

void LogicManager::createSpaceProvider()
{
	this->createInterconnectedDataAndSpace();
}

void LogicManager::createVideoService()
{
	// prerequisites:
	this->getTrackingService();
	this->getPatientService();
	this->getSpaceProvider();

	// build object(s):

	VideoServiceBackendPtr videoBackend;
	videoBackend = VideoServiceBackend::create(cx::DataManager::getInstance(),
											   mTrackingService,
											   mSpaceProvider);
	cx::VideoService::initialize(videoBackend);

}

void LogicManager::createVisualizationService()
{
	// prerequisites:
	this->getTrackingService();
	this->getPatientService();
	this->getVideoService();
	this->getSpaceProvider();

	// build object(s):

	VisualizationServiceBackendPtr vsBackend;
	vsBackend.reset(new VisualizationServiceBackend(cx::DataManager::getInstance(),
													mTrackingService,
													cx::VideoService::getInstance(),
													mSpaceProvider));
	cx::ViewManager::createInstance(vsBackend);
}

void LogicManager::createStateService()
{
	// prerequisites:
	this->getTrackingService();
	this->getPatientService();
	this->getVideoService();
	this->getSpaceProvider();

	// build object(s):
	StateServiceBackendPtr ssBackend;
	ssBackend.reset(new StateServiceBackend(cx::DataManager::getInstance(),
													mTrackingService,
													cx::VideoService::getInstance(),
													mSpaceProvider,
											cx::PatientService::getInstance()));
	cx::StateService::createInstance(ssBackend);
	// init stateservice....

}



DataFactoryPtr LogicManager::getDataFactory()
{
	if (!mDataFactory)
		this->createPatientService();
	return mDataFactory;
}

PatientServicePtr LogicManager::getPatientService()
{
	if (!mPatientService)
		this->createPatientService();
	return mPatientService;
}

TrackingServicePtr LogicManager::getTrackingService()
{
	if (!mTrackingService)
		this->createTrackingService();
	return mTrackingService;
}

VideoServicePtr LogicManager::getVideoService()
{
	if (!mVideoService)
		this->createVideoService();
	return mVideoService;
}

StateServicePtr LogicManager::getStateService()
{
	if (!mStateService)
		this->createStateService();
	return mStateService;
}

VisualizationServicePtr LogicManager::getVisualizationService()
{
	if (!mVisualizationService)
		this->createVisualizationService();
	return mVisualizationService;
}

SpaceProviderPtr LogicManager::getSpaceProvider()
{
	if (!mSpaceProvider)
		this->createSpaceProvider();
	return mSpaceProvider;
}

//void LogicManager::resetService(mStateService)
//{

//}

void LogicManager::shutdownServices()
{
	// gui layer
	// already destroyed by mainwindow

//	this->resetService(mStateService);
//	this->resetService(mVisualizationService);
//	this->resetService(mVideoService);
//	this->resetService(mSpaceProvider);
//	this->resetService(mDataFactory);
//	this->resetService(mPatientService);
//	this->resetService(mTrackingService);

	mStateService.reset();
	mVisualizationService.reset();
	mVideoService.reset();
	mSpaceProvider.reset();
	mDataFactory.reset();
	mPatientService.reset();
	mTrackingService.reset();

//	// old stuff - high level
//	StateService::destroyInstance();
//	ViewManager::destroyInstance();
//	RepManager::destroyInstance();

//	// service layer
//	cx::VideoService::shutdown();
//	cx::cxToolManager::shutdown();
//	cx::cxDataManager::shutdown();
//	cx::PatientService::shutdown();

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

//SpaceProviderPtr LogicManager::getSpaceProvider()
//{
//	return mSpaceProvider;
//}

}
