/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFileReaderWriterService.h"
#include "cxFileReaderWriterServiceNull.h"
#include "cxNullDeleter.h"
#include <QFileInfo>
#include <QUuid>
#include "cxPatientModelServiceProxy.h"
#include "cxTime.h"
#include "cxUtilHelpers.h"

namespace cx
{

FileReaderWriterServicePtr FileReaderWriterService::getNullObject()
{
	static FileReaderWriterServicePtr mNull;
	if (!mNull)
		mNull.reset(new FileReaderWriterServiceNull, null_deleter());
	return mNull;
}

//----------------------------

bool sameSuffix(QString suffix1, QString suffix2)
{
	return (suffix1.compare(suffix2, Qt::CaseInsensitive) == 0 || suffix1.compare(suffix2, Qt::CaseInsensitive) == 0);
}

FileReaderWriterImplService::FileReaderWriterImplService(QString name, QString canReadDataType, QString canWriteDataType, QString fileSuffix, PatientModelServicePtr patientModelService) :
	mName(name),
	mCanReadDataType(canReadDataType),
	mCanWriteDataType(canWriteDataType),
	mFileSuffix(fileSuffix),
	mPatientModelService(patientModelService)
{
	this->setObjectName(mName);
}

QString FileReaderWriterImplService::getName() const
{
	return mName;
}

QString FileReaderWriterImplService::getFileSuffix() const
{
	return mFileSuffix;
}

bool FileReaderWriterImplService::canReadInternal(const QString &type, const QString &filename) const
{
	QString fileType = QFileInfo(filename).suffix();
	bool rightFileType = sameSuffix(fileType, mFileSuffix);
	bool rightDataType = (type == mCanReadDataType);
	return rightFileType;
}

bool FileReaderWriterImplService::canWriteInternal(const QString &type, const QString &filename) const
{
	QString fileType = QFileInfo(filename).suffix();
	bool rightFileType = sameSuffix(fileType, mFileSuffix);
	bool rightDataType = (type == mCanWriteDataType);
	return rightFileType;
}

DataPtr FileReaderWriterImplService::createData(QString type, QString filename, QString name) const
{
	QFileInfo fileInfo(filename);
	QString strippedFilename = changeExtension(fileInfo.fileName(), "");
	QString unique = QUuid::createUuid().toString();
	//std::cout << "unique: " << unique.toStdString().c_str() << std::endl;
	QString uid = strippedFilename + "_" + unique + "_" +QDateTime::currentDateTime().toString(timestampSecondsFormat());
	if(name.isEmpty())
		name = strippedFilename;

	DataPtr data = mPatientModelService->createData(type, uid, name);

	return data;
}


}
