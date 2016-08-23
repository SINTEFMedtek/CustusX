/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
	 this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
	 this list of conditions and the following disclaimer in the documentation
	 and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
	 may be used to endorse or promote products derived from this software
	 without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

	static int count = 0;
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
