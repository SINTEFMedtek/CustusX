/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include <cxLogicManager.h>

#ifndef CX_WINDOWS
#include <sys/utsname.h>
#endif

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
#include "cxFileManagerServiceProxy.h"
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
	//CX_LOG_DEBUG() << "Ubuntu 20.04 identifyed - skipping some shutdown procedures in LogicManager";
	//CX_LOG_DEBUG() << "Skipping some shutdown procedures in LogicManager, because of CTK issues";
	LogicManager::getInstance()->shutdownServicesLight();

	//Replacing these 3 lines with the above line seems to fix the test seg. faults on Ubuntu 20.04
	//Now the same shutdown code is running on all platforms, and not only Ubuntu 20.04
	//Old shutdown sequence cause seg. faults with new CTK - Qt combinations
	//LogicManager::getInstance()->shutdownServices();
	//delete mInstance;
	//mInstance = NULL;
}

bool LogicManager::isUbuntu2004()
{
#ifdef CX_WINDOWS
	return false;
#else
	struct utsname uname_pointer;
	uname(&uname_pointer);
	CX_LOG_DEBUG() << "System info: " << uname_pointer.sysname << ", " << uname_pointer.version << ", " << uname_pointer.release;

	QString systemVersion(uname_pointer.version);
	//CX_LOG_DEBUG() << "System version: " << systemVersion;

	if(!systemVersion.contains("Ubuntu"))
		return false;

	// 5.4 is the system kernel for Ubuntu 20.04, but for some reason later installations comes with 5.11 (the kernel for 21.04)
	// In this case the uname_pointer.version string seems to contain 20.04
	QString systemKernel(uname_pointer.release);
	if(systemVersion.contains("20.04")) //For new installations of 20.04, with the "21.04 kernel"
		return true;
	else if (systemKernel.contains("5.4.")) //For old installations, with the original kernel
		return true;
	else
		return false;
#endif
}

void LogicManager::initializeServices()
{
	CX_LOG_INFO() << " --- Initialize services for " << qApp->applicationName() << "...";

	this->basicSetup();

	mPluginFramework->loadState();

	if (mComponent)
		mComponent->create();

	mShutdown = false;
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

	//mFileManagerService = FileManagerServiceProxy::create(pc);
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
	if(mShutdown)
	{
		CX_LOG_ERROR() << "Trying to shutdown logicmanager when it already shutdown. Aborting shutdown, fix code.";
		return;
	}

	CX_LOG_INFO() << " --- Shutting down " << qApp->applicationName() << "...";
	CX_LOG_DEBUG() << "Skipping some shutdown procedures in LogicManager, because of CTK issues";

	this->getPatientModelService()->autoSave();

	if (mComponent)
		mComponent->destroy(); // this is the GUI - delete first

	mPluginFramework->stop();

	this->shutdownLegacyStoredServices();
	mPluginFramework.reset();
	GPUImageBufferRepository::shutdown();
	Reporter::shutdown();
	ProfileManager::shutdown();

	mShutdown = true;
	CX_LOG_DEBUG() << " --- End shutdown services";
}

void LogicManager::shutdownServicesLight()
{
	if(mShutdown)
	{
		CX_LOG_ERROR() << "Trying to shutdown logicmanager when it already shutdown. Aborting shutdown, fix code.";
		return;
	}

	CX_LOG_INFO() << " --- Shutting down (Light) " << qApp->applicationName() << "...";

	this->getPatientModelService()->autoSave();

	if (mComponent)
		mComponent->destroy(); // this is the GUI - delete first

	this->shutdownLegacyStoredServices();

	GPUImageBufferRepository::shutdown();
	Reporter::shutdown();
	ProfileManager::shutdown();

	mShutdown = true;
	CX_LOG_DEBUG() << " --- End (Light) shutdown services";
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

//FileManagerServicePtr LogicManager::getFileManagerService()
//{
//	return mFileManagerService;
//}

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
