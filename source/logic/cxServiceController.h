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

#include "sscTool.h"
#include "cxProbe.h"

namespace cx
{
/**
* \file
* \addtogroup cxLogic
* @{
*/

/**\brief Handles all connections between the existing
 * services, in order to keep them independent of each other.
 * \inggroup cxLogic
 *
 * Owned by the LogicManager.
 */
class ServiceController: public QObject
{
Q_OBJECT

public:
	ServiceController();
	virtual ~ServiceController();

private slots:
	void updateVideoConnections();

	void patientChangedSlot();
	void clearPatientSlot();
	void duringSavePatientSlot();
	void duringLoadPatientSlot();

private:
	ssc::ToolPtr findSuitableProbe();
	void connectVideoToProbe(ssc::ToolPtr probe);
};

typedef boost::shared_ptr<ServiceController> ServiceControllerPtr;

/**
* @}
*/
}

#endif /* CXSERVICECONTROLLER_H_ */
