/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTSESSIONSTORAGETESTFIXTURE_H
#define CXTESTSESSIONSTORAGETESTFIXTURE_H

#include "cxtest_org_custusx_core_patientmodel_export.h"

#include <QString>
#include <boost/shared_ptr.hpp>
#include "cxForwardDeclarations.h"
#include "cxImage.h"
#include "cxMesh.h"
#include <ctkPluginContext.h>

#include "cxtestTestDataStructures.h"

namespace cx
{
typedef boost::shared_ptr<class SessionStorageService> SessionStorageServicePtr;
typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;
}

namespace cxtest
{
typedef boost::shared_ptr<class SessionStorageTestFixture> SessionStorageTestFixturePtr;

class CXTEST_ORG_CUSTUSX_CORE_PATIENTMODEL_EXPORT SessionStorageTestFixture
{
public:
	SessionStorageTestFixture();

	~SessionStorageTestFixture();

	void createSessions();
	void loadSession1();
	void loadSession2();
	void reloadSession1();
	void reloadSession2();
	void saveSession();

	cx::SessionStorageServicePtr mSessionStorageService;
	cx::PatientModelServicePtr mPatientModelService;
	cx::VisServicesPtr mServices;
    ctkPluginContext* mContext;
private:
	bool mSessionsCreated;
	QString mSession1;
	QString mSession2;
};

} //cxtest
#endif // CXTESTSESSIONSTORAGETESTFIXTURE_H
