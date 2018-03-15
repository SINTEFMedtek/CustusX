/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestPatientModelServiceMock.h"
#include "cxData.h"

#include "cxSpaceProvider.h"
#include "cxDataFactory.h"
#include "cxDataReaderWriter.h"
#include "cxNullDeleter.h"
#include "cxSpaceProviderImpl.h"
#include "cxTrackingService.h"
#include "cxRegistrationTransform.h"

namespace cxtest
{

PatientModelServiceMock::PatientModelServiceMock()
{
	m_rMpr.reset(new cx::RegistrationHistory());
	connect(m_rMpr.get(), &cx::RegistrationHistory::currentChanged, this, &cx::PatientModelService::rMprChanged);
}

void PatientModelServiceMock::insertData(cx::DataPtr data)
{
	mData[data->getUid()] = data;
}

cx::DataPtr PatientModelServiceMock::createData(QString type, QString uid, QString name)
{
	cx::PatientModelServicePtr self(cx::PatientModelServicePtr(this, cx::null_deleter()));
	cx::SpaceProviderPtr space(new cx::SpaceProviderImpl(cx::TrackingService::getNullObject(), self));

	static int count = 1;
	if (uid.contains("%"))
		uid = uid.arg(count++);
	if (name.contains("%"))
		name = name.arg(count);

	cx::DataFactory factory = cx::DataFactory(self, space);
	cx::DataPtr data = factory.create(type, uid, name);
	return data;
}

std::map<QString, cx::DataPtr> PatientModelServiceMock::getDatas(DataFilter filter) const
{
	return mData;
}

cx::DataPtr PatientModelServiceMock::importData(QString fileName, QString &infoText)
{
	QString type = cx::DataReaderWriter().findDataTypeFromFile(fileName);
	cx::DataPtr data = this->createData(type, fileName, fileName);
	data->load(fileName);
	this->insertData(data);
	return data;
}

cx::RegistrationHistoryPtr PatientModelServiceMock::get_rMpr_History() const
{
	return m_rMpr;
}


}
