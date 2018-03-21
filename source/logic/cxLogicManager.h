/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXLOGICMANAGER_H_
#define CXLOGICMANAGER_H_

#include "cxLogicManagerExport.h"

#include <boost/shared_ptr.hpp>
#include <QObject>

class ctkPluginContext;

namespace cx
{
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;
typedef boost::shared_ptr<class TrackingService> TrackingServicePtr;
typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef boost::shared_ptr<class VideoService> VideoServicePtr;
typedef boost::shared_ptr<class StateService> StateServicePtr;
typedef boost::shared_ptr<class ViewService> ViewServicePtr;
typedef boost::shared_ptr<class AcquisitionService> AcquisitionServicePtr;
typedef boost::shared_ptr<class SessionStorageService> SessionStorageServicePtr;

/**
* \file
* \addtogroup cx_logic
* @{
*/

typedef boost::shared_ptr<class PluginFrameworkManager> PluginFrameworkManagerPtr;

typedef boost::shared_ptr<class ApplicationComponent> ApplicationComponentPtr;
/**
 * Class holding anything that can be created/destroyed and exist meanwhile.
 */
class ApplicationComponent
{
public:
	virtual ~ApplicationComponent() {}

	virtual void create() = 0;
	virtual bool exists() const = 0;
	virtual void destroy() = 0;
};

/** \brief Control the custusx backend.
 *  \ingroup cx_logic
 *
 *
 */
class cxLogicManager_EXPORT LogicManager : public QObject
{
	Q_OBJECT
public:
  static LogicManager* getInstance();

  /**
	* Initialize the minimal manager, no services.
	*
	* Add services manually afterwards using
	*   getPluginFramework()->start("org.custusx.plugin.name", ctkPlugin::START_TRANSIENT);
	*
	* In order to insert a main UI, remember to call
	*   setApplicationComponent()
	*
	*/
  static void initializeBasic();

  /**
	* Initialize the manager, including all services (calls initializeServices() ).
	*/
  static void initialize(ApplicationComponentPtr component=ApplicationComponentPtr());
  /**
	* Shutdown the manager, including all services (calls shutdownServices() ).
	*/
  static void shutdown();

  /**
   * Set an application component, intended to encapsulate the application's
   * main window or similar. Must be called after initialize.
   *
   * Component will be created here and destroyed in shutdown.
   */
  void setApplicationComponent(ApplicationComponentPtr component);

  /**
   * Do a complete restart of the system:
   *   Shutdown all components,
   *   Then restart all of them.
   * This is done asynchronolusly.
   */
  void restartWithNewProfile(QString uid);

  void restartServicesWithProfile(QString uid);

  PluginFrameworkManagerPtr getPluginFramework();
  ctkPluginContext* getPluginContext();

  TrackingServicePtr getTrackingService();
  SpaceProviderPtr getSpaceProvider();
  PatientModelServicePtr getPatientModelService();
  VideoServicePtr getVideoService();
  StateServicePtr getStateService();
  ViewServicePtr getViewService();
  SessionStorageServicePtr getSessionStorageService();

private slots:
  void onRestartWithNewProfile(QString uid);

private:
  /**
	* Initialize all system services, resources and other static objects.
	*/
  void initializeServices();
  /**
	* Shutdown all system services, resources and other static objects.
	*
	* Deallocate all global resources.
	* Assumes MainWindow already has been destroyed and the mainloop is exited.
	*/
  void shutdownServices();

  void basicSetup();
  void createLegacyStoredServices();
  void shutdownLegacyStoredServices();

  template<class T>
  void shutdownService(boost::shared_ptr<T>& service, QString name);

  static LogicManager* mInstance;
  static void setInstance(LogicManager* instance);

  LogicManager();
	virtual ~LogicManager();

	LogicManager(LogicManager const&); // not implemented
	LogicManager& operator=(LogicManager const&); // not implemented

// services:
	SpaceProviderPtr mSpaceProvider;
	PatientModelServicePtr mPatientModelService;
	TrackingServicePtr mTrackingService;
	VideoServicePtr mVideoService;
	StateServicePtr mStateService;
	ViewServicePtr mViewService;
	SessionStorageServicePtr mSessionStorageService;

	PluginFrameworkManagerPtr mPluginFramework;
	ApplicationComponentPtr mComponent;
};

cxLogicManager_EXPORT LogicManager* logicManager(); // access the singleton

/**
* @}
*/
}

#endif /* CXLOGICMANAGER_H_ */
