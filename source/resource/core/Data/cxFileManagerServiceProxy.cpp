/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFileManagerServiceProxy.h"
#include "boost/bind.hpp"
#include "cxNullDeleter.h"

namespace cx
{

FileManagerServicePtr FileManagerServiceProxy::create(ctkPluginContext *context)
{
	return FileManagerServicePtr(new FileManagerServiceProxy(context));
}

FileManagerServiceProxy::FileManagerServiceProxy(ctkPluginContext *context) :
	mPluginContext(context),
	mService(FileManagerService::getNullObject())
{
	this->initServiceListener();
}


FileManagerServiceProxy::~FileManagerServiceProxy()
{}

bool FileManagerServiceProxy::isNull()
{
	return mService->isNull();
}

bool FileManagerServiceProxy::canLoad(const QString &type, const QString &filename)
{
	return mService->canLoad(type, filename);
}

DataPtr FileManagerServiceProxy::load(const QString &uid, const QString &filename)
{
	return mService->load(uid, filename);
}

QString FileManagerServiceProxy::canLoadDataType() const
{
	return mService->canLoadDataType();
}

std::vector<DataPtr> FileManagerServiceProxy::read(const QString &filename)
{
	return mService->read(filename);
}

bool FileManagerServiceProxy::readInto(DataPtr data, QString path)
{
	return mService->readInto(data, path);
}

QString FileManagerServiceProxy::findDataTypeFromFile(QString filename)
{
	return mService->findDataTypeFromFile(filename);
}

void FileManagerServiceProxy::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<FileManagerService>(
								 mPluginContext,
								 boost::bind(&FileManagerServiceProxy::onServiceAdded, this, _1),
								 boost::function<void (FileManagerService*)>(),
								 boost::bind(&FileManagerServiceProxy::onServiceRemoved, this, _1)
								 ));
	mServiceListener->open();

}

void FileManagerServiceProxy::onServiceAdded(FileManagerService *service)
{
	mService.reset(service, null_deleter());
	//std::cout << "FileManagerServiceProxy added: " << service->objectName().toStdString() << " this (filemanagerservice): " << service << std::endl;
}

void FileManagerServiceProxy::onServiceRemoved(FileManagerService *service)
{
	mService = FileManagerService::getNullObject();
	//std::cout << "FileManagerServiceProxy removed" << std::endl;
}

vtkImageDataPtr FileManagerServiceProxy::loadVtkImageData(QString filename)
{
	return mService->loadVtkImageData(filename);
}

vtkPolyDataPtr FileManagerServiceProxy::loadVtkPolyData(QString filename)
{
	return mService->loadVtkPolyData(filename);
}

void FileManagerServiceProxy::save(DataPtr data, const QString &filename)
{
	mService->save(data, filename);
}

void FileManagerServiceProxy::addFileReaderWriter(FileReaderWriterService *service)
{
	mService->addFileReaderWriter(service);
}

void FileManagerServiceProxy::removeFileReaderWriter(FileReaderWriterService *service)
{
	mService->removeFileReaderWriter(service);
}

std::vector<cx::FileReaderWriterServicePtr> FileManagerServiceProxy::getExportersForDataType(QString dataType)
{
	return mService->getExportersForDataType(dataType);
}

std::vector<FileReaderWriterServicePtr> FileManagerServiceProxy::getImportersForDataType(QString dataType)
{
	return mService->getImportersForDataType(dataType);
}

QString FileManagerServiceProxy::getFileReaderName(const QString &filename)
{
	return mService->getFileReaderName(filename);
}
}
