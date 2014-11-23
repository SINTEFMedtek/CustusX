/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#include <cxLogicManager.h>

#include <ctkPluginContext.h>
#include "cxServiceController.h"
#include "cxReporter.h"
#include "cxVideoServiceProxy.h"
#include "cxViewManager.h"
#include "cxStateService.h"
#include "cxRepManager.h"
#include "cxGPUImageBuffer.h"
#include "cxSettings.h"
#include "cxSpaceProviderImpl.h"
#include "cxDataFactory.h"
#include "cxVisualizationServiceBackend.h"
#include "cxTypeConversions.h"
#include "cxSharedPointerChecker.h"
#include "cxPluginFramework.h"
#include "cxVideoServiceProxy.h"
#include "cxTrackingServiceProxy.h"
#include "cxPatientModelServiceProxy.h"
#include "cxStateServiceProxy.h"

namespace cx
{

struct LegacySingletons
{
	static TrackingServicePtr mTrackingService;
	static SpaceProviderPtr mSpaceProvider;
	static PatientModelServicePtr mPatientService;
	static VideoServicePtr mVideoService;
	static VisualizationServiceOldPtr mVisualizationService;
	static StateServicePtr mStateService;
};

TrackingServicePtr LegacySingletons::mTrackingService;
SpaceProviderPtr LegacySingletons::mSpaceProvider;
PatientModelServicePtr LegacySingletons::mPatientService;
VideoServicePtr LegacySingletons::mVideoService;
VisualizationServiceOldPtr LegacySingletons::mVisualizationService;
StateServicePtr LegacySingletons::mStateService;

ViewManager* viewManager()
{
	return LegacySingletons::mVisualizationService.get();
}

TrackingServicePtr trackingService()
{
	return LegacySingletons::mTrackingService;
}
SpaceProviderPtr spaceProvider()
{
	return LegacySingletons::mSpaceProvider;
}
PatientModelServicePtr patientService()
{
	return LegacySingletons::mPatientService;
}
VideoServicePtr videoService()
{
	return LegacySingletons::mVideoService;
}
VisualizationServiceOldPtr visualizationService()
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

	mPluginFramework = PluginFrameworkManager::create();
	mPluginFramework->start();

	// services layer
	this->getPatientModelService();
	this->getTrackingService();
	this->getVideoService();
	this->getVisualizationService();
	this->getStateService();
	this->getSpaceProvider();

	mServiceController.reset(new ServiceController);

	mPluginFramework->loadState();
}

void LogicManager::createTrackingService()
{	
	mTrackingService = TrackingServiceProxy::create(this->getPluginContext());
	LegacySingletons::mTrackingService = mTrackingService;
}

void LogicManager::createInterconnectedDataAndSpace()
{
	// build object(s):
	mPatientModelService = PatientModelServiceProxy::create(this->getPluginContext());
	LegacySingletons::mPatientService = mPatientModelService;
}

void LogicManager::createSpaceProvider()
{
	mSpaceProvider.reset(new cx::SpaceProviderImpl(mTrackingService,
												   mPatientModelService));
	LegacySingletons::mSpaceProvider= mSpaceProvider;
}

void LogicManager::createPatientModelService()
{
	mPatientModelService = PatientModelServiceProxy::create(this->getPluginContext());
	LegacySingletons::mPatientService = mPatientModelService;
}

void LogicManager::createVideoService()
{
	mVideoService = VideoServiceProxy::create(this->getPluginContext());
	LegacySingletons::mVideoService = mVideoService;
}

void LogicManager::createVisualizationService()
{
	// prerequisites:
	this->getTrackingService();
	this->createPatientModelService();
	this->getVideoService();
	this->getSpaceProvider();

	// build object(s):
	VisualizationServiceBackendPtr backend;
	backend.reset(new VisualizationServiceBackend(mPatientModelService,
												  mTrackingService,
													mVideoService,
												  mSpaceProvider));
	mVisualizationService = ViewManager::create(backend);
	LegacySingletons::mVisualizationService = mVisualizationService;
}

void LogicManager::createStateService()
{
	mStateService = StateServiceProxy::create(this->getPluginContext());
	LegacySingletons::mStateService = mStateService;
}

void LogicManager::createPluginFramework()
{
//	mPluginFramework = PluginFrameworkManager::create();
//	mPluginFramework->start();

}

PatientModelServicePtr LogicManager::getPatientModelService()
{
	if (!mPatientModelService)
		this->createPatientModelService();
	return mPatientModelService;
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

VisualizationServiceOldPtr LogicManager::getVisualizationService()
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

ctkPluginContext* LogicManager::getPluginContext()
{
	return this->getPluginFramework()->getPluginContext();
}

void LogicManager::shutdownServices()
{
    mPluginFramework->stop();

	this->shutdownStateService();
	this->shutdownVisualizationService();
	this->shutdownVideoServiceOld();
	this->shutdownPatientService();
	this->shutdownInterconnectedDataAndSpace();
	this->shutdownTrackingService();

	this->shutdownPluginFramework();

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


void LogicManager::shutdownVideoServiceOld()
{
	LegacySingletons::mVideoService.reset();
	requireUnique(mVideoService, "VideoServiceOld");
	mVideoService.reset();
}

void LogicManager::shutdownPatientService()
{
	LegacySingletons::mPatientService.reset();
	mPatientModelService.reset();
}

void LogicManager::shutdownInterconnectedDataAndSpace()
{
	LegacySingletons::mSpaceProvider.reset();
	requireUnique(mSpaceProvider, "SpaceProvider");
	mSpaceProvider.reset();
}

void LogicManager::shutdownTrackingService()
{
	LegacySingletons::mTrackingService.reset();
	requireUnique(mTrackingService, "TrackingService (converted to plugin)");
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

}
