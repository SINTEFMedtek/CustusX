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
#ifndef CXLOGICMANAGER_H_
#define CXLOGICMANAGER_H_

#include <boost/shared_ptr.hpp>
#include <QObject>
#include "cxForwardDeclarations.h"
#include "cxLegacySingletons.h"

class ctkPluginContext;

namespace cx
{
/**
* \file
* \addtogroup cx_logic
* @{
*/

typedef boost::shared_ptr<class PluginFrameworkManager> PluginFrameworkManagerPtr;
typedef boost::shared_ptr<class ServiceController> ServiceControllerPtr;
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
	ctkPluginContext* getPluginContext();

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
