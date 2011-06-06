/*
 * cxServiceController.h
 *
 *  Created on: Jun 1, 2011
 *      Author: christiana
 */

#ifndef CXSERVICECONTROLLER_H_
#define CXSERVICECONTROLLER_H_

#include <boost/shared_ptr.hpp>
#include <QObject>

namespace cx
{

/**ServiceController handles all connections between the existing
 * services, in order to keep them independent of each other.
 *
 * Owned by the LogicManager.
 */
class ServiceController : public QObject
{
	Q_OBJECT

public:
	ServiceController();
	virtual ~ServiceController();
};

typedef boost::shared_ptr<ServiceController> ServiceControllerPtr;

}

#endif /* CXSERVICECONTROLLER_H_ */
