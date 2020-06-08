/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTESTPATIENTMODELSERVICEMOCK_H
#define CXTESTPATIENTMODELSERVICEMOCK_H

#include "cxtestresource_export.h"

#include "cxPatientModelServiceNull.h"

namespace cxtest
{
typedef boost::shared_ptr<class PatientModelServiceMock> PatientModelServiceMockPtr;

class CXTESTRESOURCE_EXPORT PatientModelServiceMock : public cx::PatientModelServiceNull
{
public:
	PatientModelServiceMock();
	virtual ~PatientModelServiceMock() {}

	virtual void insertData(cx::DataPtr data, bool overWrite = false);
	virtual cx::DataPtr createData(QString type, QString uid, QString name="");
	virtual std::map<QString, cx::DataPtr> getDatas(DataFilter filter) const;
	virtual cx::DataPtr importDataMock(QString fileName, QString &infoText, cx::FileManagerServicePtr filemanager);
	virtual cx::RegistrationHistoryPtr get_rMpr_History() const;

private:
	std::map<QString, cx::DataPtr> mData;
	cx::RegistrationHistoryPtr m_rMpr;
};

}

#endif // CXTESTPATIENTMODELSERVICEMOCK_H
