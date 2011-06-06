/*
 * cxLogicManager.h
 *
 *  Created on: Jun 1, 2011
 *      Author: christiana
 */

#ifndef CXLOGICMANAGER_H_
#define CXLOGICMANAGER_H_

#include <boost/shared_ptr.hpp>
#include <QObject>

namespace cx
{
typedef boost::shared_ptr<class ServiceController> ServiceControllerPtr;

/**LogicManager
 *
 * Responsible for the entire logic layer, all controllers
 * in this layers are embedded in this class.
 *
 * The service layer are managed via the ServiceController aggregate member.
 *
 */
class LogicManager : public QObject
{
	Q_OBJECT
public:
  static LogicManager* getInstance();

  static void initialize();
  static void shutdown();

//  ServiceControllerPtr getServiceController();

private:
  static LogicManager* mInstance;
  static void setInstance(LogicManager* instance);

  LogicManager();
	virtual ~LogicManager();

	LogicManager(LogicManager const&); // not implemented
	LogicManager& operator=(LogicManager const&); // not implemented

	ServiceControllerPtr mServiceController;
};

//LogicManager* logicManager(); // if necessary

}

#endif /* CXLOGICMANAGER_H_ */
