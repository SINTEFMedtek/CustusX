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

#ifndef CXPATIENTMODELIMPLSERVICE_H_
#define CXPATIENTMODELIMPLSERVICE_H_

#include "cxPatientModelService.h"
#include "org_custusx_patientmodel_Export.h"

namespace cx
{

/**
 * Implementation of PatientModelService.
 *
 * \ingroup org_custusx_patientmodel
 *
 * \date 2014-05-15
 * \author Christian Askeland
 */
class org_custusx_patientmodel_EXPORT PatientModelImplService : public PatientModelService
{
	Q_INTERFACES(cx::PatientModelService)
public:
		PatientModelImplService();
	virtual ~PatientModelImplService();


};
typedef boost::shared_ptr<PatientModelImplService> PatientModelImplServicePtr;

} /* namespace cx */

#endif /* CXPATIENTMODELIMPLSERVICE_H_ */

