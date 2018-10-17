/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFileReaderWriterServiceProxy.h"
#include "boost/bind.hpp"
#include "cxNullDeleter.h"

namespace cx {

FileReaderWriterServicePtr FileReaderWriterServiceProxy::create(ctkPluginContext *context)
{
	return FileReaderWriterServicePtr(new FileReaderWriterServiceProxy(context));
}

FileReaderWriterServiceProxy::FileReaderWriterServiceProxy(ctkPluginContext *context) :
	mPluginContext(context),
	mService(FileReaderWriterService::getNullObject())
{
}

bool FileReaderWriterServiceProxy::isNull()
{
	return mService->isNull();
}

bool FileReaderWriterServiceProxy::canRead(const QString &type, const QString &filename)
{
	return mService->canRead(type, filename);
}

std::vector<DataPtr> FileReaderWriterServiceProxy::read(const QString &filename)
{
	return mService->read(filename);
}

DataPtr FileReaderWriterServiceProxy::read(const QString &uid, const QString &filename)
{
	return mService->read(uid, filename);
}

QString FileReaderWriterServiceProxy::canReadDataType() const
{
	return mService->canReadDataType();
}

bool FileReaderWriterServiceProxy::readInto(DataPtr data, QString path)
{
	return mService->readInto(data, path);
}

void FileReaderWriterServiceProxy::write(DataPtr data, const QString &filename)
{
	mService->write(data, filename);
}


} //cx


QString cx::FileReaderWriterServiceProxy::canWriteDataType() const
{
	return mService->canReadDataType();
}

bool cx::FileReaderWriterServiceProxy::canWrite(const QString &type, const QString &filename) const
{
	return mService->canWrite(type, filename);
}

QString cx::FileReaderWriterServiceProxy::getName() const
{
	return mService->getName();
}

QString cx::FileReaderWriterServiceProxy::getFileSuffix() const
{
	return mService->getFileSuffix();
}

vtkImageDataPtr cx::FileReaderWriterServiceProxy::loadVtkImageData(QString filename)
{
	return mService->loadVtkImageData(filename);
}

vtkPolyDataPtr cx::FileReaderWriterServiceProxy::loadVtkPolyData(QString filename)
{
	return mService->loadVtkPolyData(filename);
}
