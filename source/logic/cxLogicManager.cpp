/*
 * cxLogicManager.cpp
 *
 *  \date Jun 1, 2011
 *      \author christiana
 */

#include <cxLogicManager.h>

#include "cxServiceController.h"
#include "cxReporter.h"
#include "cxPatientService.h"
#include "cxVideoService.h"
#include "cxToolManagerUsingIGSTK.h"
#include "cxViewManager.h"
#include "cxStateService.h"
#include "cxDataManagerImpl.h"
#include "cxRepManager.h"
#include "cxGPUImageBuffer.h"
#include "cxSettings.h"
#include "cxSpaceProviderImpl.h"
#include "cxDataFactory.h"
#include "cxVisualizationServiceBackend.h"
#include "cxVideoServiceBackend.h"
#include "cxStateServiceBackend.h"
#include "cxTypeConversions.h"
#include "cxSharedPointerChecker.h"
#include "cxPluginFramework.h"

namespace cx
{

struct LegacySingletons
{
	static TrackingServicePtr mToolManager;
	static DataServicePtr mDataManager;
	static SpaceProviderPtr mSpaceProvider;
	static PatientServicePtr mPatientService;
	static VideoServicePtr mVideoService;
	static VisualizationServicePtr mVisualizationService;
	static StateServicePtr mStateService;
};

TrackingServicePtr LegacySingletons::mToolManager;
DataServicePtr LegacySingletons::mDataManager;
SpaceProviderPtr LegacySingletons::mSpaceProvider;
PatientServicePtr LegacySingletons::mPatientService;
VideoServicePtr LegacySingletons::mVideoService;
VisualizationServicePtr LegacySingletons::mVisualizationService;
StateServicePtr LegacySingletons::mStateService;

ToolManager* toolManager()
{
	return LegacySingletons::mToolManager.get(); // TODO remove get()
}
DataManager* dataManager()
{
	return LegacySingletons::mDataManager.get(); // TODO remove get()
}
ViewManager* viewManager()
{
	return LegacySingletons::mVisualizationService.get();
}

TrackingServicePtr trackingService()
{
	return LegacySingletons::mToolManager;
}
SpaceProviderPtr spaceProvider()
{
	return LegacySingletons::mSpaceProvider;
}
PatientServicePtr patientService()
{
	return LegacySingletons::mPatientService;
}
DataServicePtr dataService()
{
	return LegacySingletons::mDataManager;
}
VideoServicePtr videoService()
{
	return LegacySingletons::mVideoService;
}
VisualizationServicePtr visualizationService()
{
	return LegacySingletons::mVisualizationService;
}
StateServicePtr stateService()
{
	return LegacySingletons::mStateService;
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
	Reporter::initialize();

	// services layer
	this->getPatientService();
	this->getTrackingService();
	this->getVideoService();
	this->getStateService();
	this->getVisualizationService();
	this->getSpaceProvider();

	mServiceController.reset(new ServiceController);

	// logic layer
	//cx::LogicManager::initialize();

	// gui layer:
	// inited by mainwindow construction in main()
}

void LogicManager::createTrackingService()
{
	mTrackingService = ToolManagerUsingIGSTK::create();
	LegacySingletons::mToolManager = mTrackingService;
}

void LogicManager::createInterconnectedDataAndSpace()
{
	// prerequisites:
	this->getTrackingService();

	// build object(s):
	mDataService = DataManagerImpl::create();
	LegacySingletons::mDataManager = mDataService;

	mSpaceProvider.reset(new cx::SpaceProviderImpl(mTrackingService,
												   mDataService));
	mDataService->setSpaceProvider(mSpaceProvider);
	LegacySingletons::mSpaceProvider= mSpaceProvider;

	mDataFactory.reset(new DataFactory(mDataService, mSpaceProvider));
	mDataService->setDataFactory(mDataFactory);
}

void LogicManager::createDataFactory()
{
	this->createInterconnectedDataAndSpace();
}

void LogicManager::createDataService()
{
	this->createInterconnectedDataAndSpace();
}

void LogicManager::createSpaceProvider()
{
	this->createInterconnectedDataAndSpace();
}

void LogicManager::createPatientService()
{
	// prerequisites:
	this->getDataService();
	// build object(s):
	mPatientService = PatientService::create(mDataService);
	LegacySingletons::mPatientService = mPatientService;
}

void LogicManager::createVideoService()
{
	// prerequisites:
	this->getTrackingService();
	this->getDataService();
	this->getSpaceProvider();

	// build object(s):
	VideoServiceBackendPtr videoBackend;
	videoBackend = VideoServiceBackend::create(mDataService,
											   mTrackingService,
											   mSpaceProvider);
	mVideoService = VideoService::create(videoBackend);
	LegacySingletons::mVideoService = mVideoService;
}

void LogicManager::createVisualizationService()
{
	// prerequisites:
	this->getTrackingService();
	this->getDataService();
	this->getVideoService();
	this->getSpaceProvider();

	// build object(s):
	VisualizationServiceBackendPtr backend;
	backend.reset(new VisualizationServiceBackend(mDataService,
												  mTrackingService,
												  mVideoService,
												  mSpaceProvider));
	mVisualizationService = ViewManager::create(backend);
	LegacySingletons::mVisualizationService = mVisualizationService;
}

void LogicManager::createStateService()
{
	// prerequisites:
	this->getTrackingService();
	this->getDataService();
	this->getPatientService();
	this->getVideoService();
	this->getSpaceProvider();

	// build object(s):
	StateServiceBackendPtr backend;
	backend.reset(new StateServiceBackend(mDataService,
										  mTrackingService,
										  mVideoService,
										  mSpaceProvider,
										  mPatientService));
	mStateService = StateService::create(backend);
	LegacySingletons::mStateService = mStateService;
}

void LogicManager::createPluginFramework()
{
	mPluginFramework = PluginFrameworkManager::create();
	mPluginFramework->start();

}

DataFactoryPtr LogicManager::getDataFactory()
{
	if (!mDataFactory)
		this->createPatientService();
	return mDataFactory;
}

DataServicePtr LogicManager::getDataService()
{
	if (!mDataService)
		this->createDataService();
	return mDataService;
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

PluginFrameworkManagerPtr LogicManager::getPluginFramework()
{
	if (!mPluginFramework)
		this->createPluginFramework();
	return mPluginFramework;
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

	this->shutdownPluginFramework();

	this->shutdownStateService();
	this->shutdownVisualizationService();
	this->shutdownVideoService();
	this->shutdownPatientService();
	this->shutdownInterconnectedDataAndSpace();
	this->shutdownTrackingService();

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
	Reporter::shutdown();
	Settings::destroyInstance();
}

void LogicManager::shutdownStateService()
{
	LegacySingletons::mStateService.reset();
	requireUnique(mStateService, "StateService");
	mStateService.reset();
}

void LogicManager::shutdownVisualizationService()
{
	LegacySingletons::mVisualizationService.reset();
	requireUnique(mVisualizationService, "VisualizationService");
	mVisualizationService.reset();
}


void LogicManager::shutdownVideoService()
{
	LegacySingletons::mVideoService.reset();
	requireUnique(mVideoService, "VideoService");
	mVideoService.reset();
}

void LogicManager::shutdownPatientService()
{
	LegacySingletons::mPatientService.reset();
	requireUnique(mPatientService, "PatientService");
	mPatientService.reset();
}

void LogicManager::shutdownInterconnectedDataAndSpace()
{
	// [HACK] break loop by removing connection to DataFactory and SpaceProvider
	mDataService->setSpaceProvider(SpaceProviderPtr());
	mDataService->setDataFactory(DataFactoryPtr());
	mDataService->clear();

	requireUnique(mDataFactory, "DataFactory");
	mDataFactory.reset();

	LegacySingletons::mSpaceProvider.reset();
	requireUnique(mSpaceProvider, "SpaceProvider");
	mSpaceProvider.reset();

	LegacySingletons::mDataManager.reset();
	requireUnique(mDataService, "DataService");
	mDataService.reset();
}

void LogicManager::shutdownTrackingService()
{
	LegacySingletons::mToolManager.reset();
	requireUnique(mTrackingService, "TrackingService");
	mTrackingService.reset();
}

void LogicManager::shutdownPluginFramework()
{
	requireUnique(mPluginFramework, "PluginFramework");
	mPluginFramework.reset();
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
