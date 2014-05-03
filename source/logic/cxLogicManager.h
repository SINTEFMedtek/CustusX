/*
 * cxLogicManager.h
 *
 *  \date Jun 1, 2011
 *      \author christiana
 */

#ifndef CXLOGICMANAGER_H_
#define CXLOGICMANAGER_H_

#include <boost/shared_ptr.hpp>
#include <QObject>
#include "cxForwardDeclarations.h"
#include "cxLegacySingletons.h"

namespace cx
{
/**
* \file
* \addtogroup cx_logic
* @{
*/

typedef boost::shared_ptr<class PluginFrameworkManager> PluginFrameworkManagerPtr;

//typedef class ToolManager TrackingService;
//typedef class ViewManager VisualizationService;

typedef boost::shared_ptr<class ServiceController> ServiceControllerPtr;
//typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;
//typedef boost::shared_ptr<class DataFactory> DataFactoryPtr;

//typedef boost::shared_ptr<class PatientService> PatientServicePtr;
//typedef boost::shared_ptr<class ToolManager> TrackingServicePtr;
//typedef boost::shared_ptr<class VideoService> VideoServicePtr;
//typedef boost::shared_ptr<class ViewManager> VisualizationServicePtr;
//typedef boost::shared_ptr<class StateService> StateServicePtr;

//typedef boost::shared_ptr<class DataManager> DataServicePtr;
typedef boost::shared_ptr<class DataManagerImpl> DataManagerImplPtr;

/**\brief Responsible for the entire logic layer.
 * \ingroup cx_logic
 *
 * All controllers in this layers are embedded in this class.
 * The service layer are managed via the ServiceController aggregate member.
 *
 */
class LogicManager : public QObject
{
	Q_OBJECT
public:
  static LogicManager* getInstance();

  /**
	* Initialize the manager, including all services (calls initializeServices() ).
	*/
  static void initialize();
  /**
	* Shutdown the manager, including all services (calls shutdownServices() ).
	*/
  static void shutdown();

  PluginFrameworkManagerPtr getPluginFramework();

//  SpaceProviderPtr getSpaceProvider();
  DataFactoryPtr getDataFactory();
  DataServicePtr getDataService();
  TrackingServicePtr getTrackingService();
  SpaceProviderPtr getSpaceProvider();
  PatientServicePtr getPatientService();
  VideoServicePtr getVideoService();
  VisualizationServicePtr getVisualizationService();
  StateServicePtr getStateService();

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

  void createTrackingService();
  void createPatientService();
  void createDataService();
  void createDataFactory();
  void createSpaceProvider();
  void createVideoService();
  void createVisualizationService();
  void createStateService();
  void createPluginFramework();

  void createInterconnectedDataAndSpace();

  void shutdownStateService();
  void shutdownVisualizationService();
  void shutdownVideoService();
  void shutdownPatientService();
  void shutdownInterconnectedDataAndSpace();
  void shutdownTrackingService();
  void shutdownPluginFramework();

  static LogicManager* mInstance;
  static void setInstance(LogicManager* instance);

  LogicManager();
	virtual ~LogicManager();

	LogicManager(LogicManager const&); // not implemented
	LogicManager& operator=(LogicManager const&); // not implemented

	ServiceControllerPtr mServiceController;

// services:
	SpaceProviderPtr mSpaceProvider;
	DataFactoryPtr mDataFactory;

	PatientServicePtr mPatientService;
	TrackingServicePtr mTrackingService;
	VideoServicePtr mVideoService;
	VisualizationServicePtr mVisualizationService;
	StateServicePtr mStateService;
	DataManagerImplPtr mDataService;

	PluginFrameworkManagerPtr mPluginFramework;
};

LogicManager* logicManager(); // if necessary

/**
* @}
*/
}

#endif /* CXLOGICMANAGER_H_ */
