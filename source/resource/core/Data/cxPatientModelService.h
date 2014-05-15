// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXPATIENTMODELSERVICE_H_
#define CXPATIENTMODELSERVICE_H_

#include <QObject>
#include "boost/shared_ptr.hpp"

namespace cx
{
typedef boost::shared_ptr<class Data> DataPtr;
}

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

	virtual void insertData(DataPtr data) = 0;
};

} // namespace cx
Q_DECLARE_INTERFACE(cx::PatientModelService, PatientModelService_iid)


#endif /* CXPATIENTMODELSERVICE_H_ */
