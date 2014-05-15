#ifndef CXPATIENTMODELSERVICE_H_
#define CXPATIENTMODELSERVICE_H_

#include <QObject>
#include "boost/shared_ptr.hpp"

#define PatientModelService_iid "cx::PatientModelService"

namespace cx
{
typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;

/** \brief The virtual patient
 *
 * PatientModelService provides access to the Patient Specific Model (PaSM).
 *   - data entities
 *   	- volumes
 *   	- surfaces
 *   	- metrics
 *   	- etc
 *   - relations between entities in space, time and structure
 *   - load/save
 *
 * This service replaces the old DataManager and PatientData classes. They
 * are deprecated.
 *
 *  \ingroup cx_resource_core_data
 *  \date 2014-05-15
 *  \author Christian Askeland
 */
class PatientModelService : public QObject
{
	Q_OBJECT
public:
	virtual ~PatientModelService();
};

} // namespace cx
Q_DECLARE_INTERFACE(cx::PatientModelService, PatientModelService_iid)


#endif /* CXPATIENTMODELSERVICE_H_ */
