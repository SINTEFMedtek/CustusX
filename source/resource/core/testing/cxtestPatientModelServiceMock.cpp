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
#include "cxNullDeleter.h"
#include "cxSpaceProviderImpl.h"
#include "cxTrackingService.h"
#include "cxRegistrationTransform.h"
#include "cxFileManagerService.h"
#include "catch.hpp"

namespace cxtest
{

PatientModelServiceMock::PatientModelServiceMock()
{
	m_rMpr.reset(new cx::RegistrationHistory());
	connect(m_rMpr.get(), &cx::RegistrationHistory::currentChanged, this, &cx::PatientModelService::rMprChanged);
}

void PatientModelServiceMock::insertData(cx::DataPtr data, bool overWrite)
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

cx::DataPtr PatientModelServiceMock::importDataMock(QString fileName, QString &infoText, cx::FileManagerServicePtr filemanager)
{
	QString type = filemanager->findDataTypeFromFile(fileName);
	REQUIRE_FALSE(type.isEmpty());
	cx::DataPtr data = this->createData(type, fileName, fileName);
	REQUIRE(data);
	data->load(fileName, filemanager);
	this->insertData(data);
	return data;
}

cx::RegistrationHistoryPtr PatientModelServiceMock::get_rMpr_History() const
{
	return m_rMpr;
}


}
