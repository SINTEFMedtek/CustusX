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
	//The bottom 3 lines still cause tests on Ubuntu 20.04 to fail
	//83 unit tests (many seg.faults), when running:
	// ninja && python3 ../CX/install/cxRunTests.py --run_catch --wrap_in_ctest [unit]~[hide] -t Debug
	//Example:
//#0  0x00007f1fc0ee6528 in QOpenGLContext::surface() const () at /lib/x86_64-linux-gnu/libQt5Gui.so.5
//#1  0x00007f1fc0eb6160 in QSurface::~QSurface() () at /lib/x86_64-linux-gnu/libQt5Gui.so.5
//#2  0x00007f1fc0eb3bbf in QWindow::~QWindow() () at /lib/x86_64-linux-gnu/libQt5Gui.so.5
//#3  0x00007f1fc255a72d in  () at /lib/x86_64-linux-gnu/libQt5Widgets.so.5
//#4  0x00007f1fc252fa4c in QWidgetPrivate::deleteTLSysExtra() () at /lib/x86_64-linux-gnu/libQt5Widgets.so.5
//#5  0x00007f1fc2534568 in QWidget::destroy(bool, bool) () at /lib/x86_64-linux-gnu/libQt5Widgets.so.5
//#6  0x00007f1fc24fe207 in QApplication::~QApplication() () at /lib/x86_64-linux-gnu/libQt5Widgets.so.5
//#7  0x00007f1fc3f1b50a in cx::Application::~Application() (this=0x7fffdb7454c0, __in_chrg=<optimized out>) at /home/olevs/dev/cx/CX/CX/source/resource/core/./utilities/cxApplication.h:36
//#8  0x00007f1fc3f034f3 in cxtest::CatchImpl::run(int, char**) (this=0x7fffdb745520, argc=2, argv=0x7fffdb745648) at /home/olevs/dev/cx/CX/CX/source/testing/cxtestCatchImpl.cpp:30
//#9  0x0000000000406926 in main(int, char**) (argc=2, argv=0x7fffdb745648) at /home/olevs/dev/cx/CX/CX/source/testing/cxtestCatchMain.cpp:59


	//Running
	//ninja && ./bin/Catch [unit]~[hide]
	//cause only 5 tests to fail. No segfaults after the current code fixes

	//Replacing the 3 lines with this seems to fix the test seg.faults on Ubuntu 20.04, but may still cause issues with the other platforms (not verified yet)
	//LogicManager::getInstance()->shutdownLegacyStoredServices();

	//These 3 lines cause tests on Ubuntu 20.04 to fail, but are needed for the other platforms (Windows, Mac, Ubuntu 16.04)
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
