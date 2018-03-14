/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include <cxLogicManager.h>

#include <QApplication>
#include <ctkPluginContext.h>
#include "cxLogger.h"
#include "cxVideoServiceProxy.h"
#include "cxStateService.h"
#include "cxGPUImageBuffer.h"
#include "cxSettings.h"
#include "cxSpaceProviderImpl.h"
#include "cxDataFactory.h"
#include "cxCoreServices.h"
#include "cxTypeConversions.h"
#include "cxSharedPointerChecker.h"
#include "cxPluginFramework.h"
#include "cxVideoServiceProxy.h"
#include "cxTrackingServiceProxy.h"
#include "cxPatientModelServiceProxy.h"
#include "cxStateServiceProxy.h"
#include "cxViewServiceProxy.h"
#include "cxSessionStorageServiceProxy.h"
#include "cxReporter.h"
#include "cxProfile.h"

namespace cx
{
// --------------------------------------------------------
LogicManager* LogicManager::mInstance = NULL; ///< static member
// --------------------------------------------------------

LogicManager* logicManager()
{
	return LogicManager::getInstance();
}

void LogicManager::initializeBasic()
{
	LogicManager::getInstance()->basicSetup();
}

void LogicManager::initialize(ApplicationComponentPtr component)
{
	LogicManager::getInstance()->initializeServices();
	LogicManager::getInstance()->setApplicationComponent(component);

	// we might want to use this one, in order to shutdown within the main loop
//	connect(qApp, &QApplication::aboutToQuit, LogicManager::getInstance(), &LogicManager::shutdown);
}

void LogicManager::shutdown()
{
	LogicManager::getInstance()->shutdownServices();

	delete mInstance;
	mInstance = NULL;
}

void LogicManager::initializeServices()
{
	CX_LOG_INFO() << " --- Initialize services for " << qApp->applicationName() << "...";

	this->basicSetup();

	mPluginFramework->loadState();

	if (mComponent)
		mComponent->create();

	CX_LOG_DEBUG() << " --- End initialize services.";
}

void LogicManager::basicSetup()
{
	ProfileManager::initialize();
	Reporter::initialize();

	mPluginFramework = PluginFrameworkManager::create();
	mPluginFramework->start();
	mPluginFramework->setSearchPaths(QStringList());

	this->createLegacyStoredServices();
}

void LogicManager::createLegacyStoredServices()
{
	// services layer
	ctkPluginContext* pc = this->getPluginContext();

	mTrackingService = TrackingServiceProxy::create(pc);
	mPatientModelService = PatientModelServiceProxy::create(pc);
	mVideoService = VideoServiceProxy::create(pc);
	mViewService = ViewServiceProxy::create(pc);
	connect(mPluginFramework.get(), &PluginFrameworkManager::aboutToStop, mViewService.get(), &ViewService::aboutToStop);
	mStateService = StateServiceProxy::create(pc);
	mSessionStorageService = SessionStorageServiceProxy::create(pc);

	mSpaceProvider.reset(new cx::SpaceProviderImpl(mTrackingService, mPatientModelService));
}

void LogicManager::setApplicationComponent(ApplicationComponentPtr component)
{
	if (mComponent)
		mComponent->destroy();

	mComponent = component;

	if (mComponent)
		mComponent->create();
}

void LogicManager::restartWithNewProfile(QString uid)
{
	QMetaObject::invokeMethod(this, "onRestartWithNewProfile",
							  Qt::QueuedConnection,
							  Q_ARG(QString, uid));
}

void LogicManager::onRestartWithNewProfile(QString uid)
{
	if (profile()->getUid()==uid)
		return;
    this->restartServicesWithProfile(uid);
}

void LogicManager::restartServicesWithProfile(QString uid)
{
    this->shutdownServices();
    ProfileManager::getInstance()->setActiveProfile(uid);
    this->initializeServices();
}

void LogicManager::shutdownServices()
{
	CX_LOG_INFO() << " --- Shutting down " << qApp->applicationName() << "...";

	this->getPatientModelService()->autoSave();

	if (mComponent)
		mComponent->destroy(); // this is the GUI - delete first

	mPluginFramework->stop();

	this->shutdownLegacyStoredServices();
	mPluginFramework.reset();
	GPUImageBufferRepository::shutdown();
	Reporter::shutdown();
	ProfileManager::shutdown();

	CX_LOG_DEBUG() << " --- End shutdown services";
}

void LogicManager::shutdownLegacyStoredServices()
{
	this->shutdownService(mSpaceProvider, "SpaceProvider"); // remove before patmodel and track
	this->shutdownService(mStateService, "StateService");
	this->shutdownService(mViewService, "ViewService");
	this->shutdownService(mTrackingService, "TrackingService");
	this->shutdownService(mPatientModelService, "PatientModelService");
	this->shutdownService(mVideoService, "VideoService");
	this->shutdownService(mSessionStorageService, "SessionStorageService");
}


template<class T>
void LogicManager::shutdownService(boost::shared_ptr<T>& service, QString name)
{
	requireUnique(service, name);
	service.reset();
}

PatientModelServicePtr LogicManager::getPatientModelService()
{
	return mPatientModelService;
}
TrackingServicePtr LogicManager::getTrackingService()
{
	return mTrackingService;
}
VideoServicePtr LogicManager::getVideoService()
{
	return mVideoService;
}
StateServicePtr LogicManager::getStateService()
{
	return mStateService;
}
SpaceProviderPtr LogicManager::getSpaceProvider()
{
	return mSpaceProvider;
}
ViewServicePtr LogicManager::getViewService()
{
	return mViewService;
}
SessionStorageServicePtr LogicManager::getSessionStorageService()
{
	return mSessionStorageService;
}
PluginFrameworkManagerPtr LogicManager::getPluginFramework()
{
	return mPluginFramework;
}
ctkPluginContext* LogicManager::getPluginContext()
{
	return this->getPluginFramework()->getPluginContext();
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
