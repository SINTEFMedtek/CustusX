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

namespace cx
{
/**
* \file
* \addtogroup cxLogic
* @{
*/

typedef boost::shared_ptr<class ServiceController> ServiceControllerPtr;
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;
typedef boost::shared_ptr<class DataFactory> DataFactoryPtr;

/**\brief Responsible for the entire logic layer.
 * \ingroup cxLogic
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

  SpaceProviderPtr getSpaceProvider();
  DataFactoryPtr getDataFactory();

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

};

LogicManager* logicManager(); // if necessary

/**
* @}
*/
}

#endif /* CXLOGICMANAGER_H_ */
