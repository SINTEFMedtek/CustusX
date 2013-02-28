/*
 * cxServiceController.h
 *
 *  \date Jun 1, 2011
 *      \author christiana
 */

#ifndef CXSERVICECONTROLLER_H_
#define CXSERVICECONTROLLER_H_

#include <boost/shared_ptr.hpp>
#include <QObject>
#include "sscForwardDeclarations.h"

namespace cx
{
/**
* \file
* \addtogroup cxLogic
* @{
*/

/**\brief Handles all connections between the existing
 * services, in order to keep them independent of each other.
 * \ingroup cxLogic
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
//	void updateVideoConnections();

	void patientChangedSlot();
	void clearPatientSlot();
	void duringSavePatientSlot();
	void duringLoadPatientSlot();

//private:
//	void connectVideoToProbe(ssc::ToolPtr probe);
};

typedef boost::shared_ptr<ServiceController> ServiceControllerPtr;

/**
* @}
*/
}

#endif /* CXSERVICECONTROLLER_H_ */
