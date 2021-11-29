/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFileManagerServiceBase.h"
#include <QFileInfo>
#include "cxTypeConversions.h"
#include "cxUtilHelpers.h"
#include "cxNullDeleter.h"
#include "cxLogger.h"
#include "boost/bind.hpp"
#include "cxCoreServices.h"

namespace cx
{
FileManagerServiceBase::FileManagerServiceBase()
{}

FileManagerServiceBase::~FileManagerServiceBase()
{
}

bool FileManagerServiceBase::isNull()
{
	return false;
}

QString FileManagerServiceBase::canLoadDataType() const
{
	std::cout << "[TODO] error FileManagerServiceBase::canLoadDataType not implemented yet." << std::endl;
	return QString("TODO");
}

bool FileManagerServiceBase::canLoad(const QString &type, const QString &filename)
{
	FileReaderWriterServicePtr reader = this->findReader(filename);
	if (reader)
		return reader->canRead(type, filename);
	else
		return false;
}

DataPtr FileManagerServiceBase::load(const QString &uid, const QString &filename)
{
	FileReaderWriterServicePtr reader = this->findReader(filename);
	if (reader)
		return reader->read(uid, filename);
	else
		return DataPtr();
}

FileReaderWriterServicePtr FileManagerServiceBase::findReader(const QString& path, const QString& type)
{
	for (std::set<FileReaderWriterServicePtr>::iterator iter = mDataReaders.begin(); iter != mDataReaders.end(); ++iter)
	{
		if ((*iter)->canRead(type, path))
			return *iter;
	}
	return FileReaderWriterServicePtr();
}

FileReaderWriterServicePtr FileManagerServiceBase::findWriter(const QString& path, const QString& type)
{
	//TODO refactor with the findreader function..
	for (std::set<FileReaderWriterServicePtr>::iterator iter = mDataReaders.begin(); iter != mDataReaders.end(); ++iter)
	{
		if ((*iter)->canWrite(type, path))
			return *iter;
	}
	return FileReaderWriterServicePtr();
}

vtkImageDataPtr FileManagerServiceBase::loadVtkImageData(QString filename)
{
	vtkImageDataPtr retval = vtkImageDataPtr();
	FileReaderWriterServicePtr reader = this->findReader(filename);
	if (reader)
	{
		retval = reader->loadVtkImageData(filename);
	}
	return retval;
}

vtkPolyDataPtr FileManagerServiceBase::loadVtkPolyData(QString filename)
{
	FileReaderWriterServicePtr reader = this->findReader(filename);
	if (reader)
		return reader->loadVtkPolyData(filename);
	return vtkPolyDataPtr();
}

std::vector<FileReaderWriterServicePtr> FileManagerServiceBase::getExportersForDataType(QString dataType)
{
	std::vector<FileReaderWriterServicePtr> retval;
	for (std::set<FileReaderWriterServicePtr>::iterator iter = mDataReaders.begin(); iter != mDataReaders.end(); ++iter)
	{
		if (dataType.compare((*iter)->canWriteDataType()) == 0)
			retval.push_back(*iter);
	}

	return retval;
}

std::vector<FileReaderWriterServicePtr> FileManagerServiceBase::getImportersForDataType(QString dataType)
{
	std::vector<FileReaderWriterServicePtr> retval;
	for (std::set<FileReaderWriterServicePtr>::iterator iter = mDataReaders.begin(); iter != mDataReaders.end(); ++iter)
	{
		if (dataType.compare((*iter)->canReadDataType()) == 0)
			retval.push_back(*iter);
	}

	return retval;

}

QString FileManagerServiceBase::findDataTypeFromFile(QString filename)
{
	FileReaderWriterServicePtr reader = this->findReader(filename);
	if (reader)
		return reader->canReadDataType();
	CX_LOG_WARNING() << "FileManagerServiceBase::findDataTypeFromFile: Cannot find a file reader";
	return "";
}

bool FileManagerServiceBase::readInto(DataPtr data, QString path)
{
	bool success = false;
	FileReaderWriterServicePtr reader = this->findReader(path, data->getType());
	if (reader)
		success = reader->readInto(data, path);

	if(data)
	{
		QFileInfo fileInfo(qstring_cast(path));
		data->setName(changeExtension(fileInfo.fileName(), ""));
		//data->setFilename(path); // need path even when not set explicitly: nice for testing
	}
	return success;

}

std::vector<DataPtr> FileManagerServiceBase::read(const QString &filename)
{
	std::vector<DataPtr> retval;
	FileReaderWriterServicePtr reader = this->findReader(filename);
	if (reader)
		retval = reader->read(filename);
	return retval;
}

void FileManagerServiceBase::save(DataPtr data, const QString &filename)
{
	FileReaderWriterServicePtr writer = this->findWriter(filename);
	if (writer)
		return writer->write(data, filename);
	else
		CX_LOG_ERROR() << "Could not find writer.";
}

void FileManagerServiceBase::addFileReaderWriter(FileReaderWriterService *service)
{
	// adding a service inside a smartpointer... not so smart, think it is fixed with null_deleter
	mDataReaders.insert(FileReaderWriterServicePtr(service, null_deleter()));
	CX_LOG_DEBUG() << "Adding a reader/writer: " << service->objectName() << " to: " << this;
}

void FileManagerServiceBase::removeFileReaderWriter(FileReaderWriterService *service)
{
	for(std::set<FileReaderWriterServicePtr>::iterator it = mDataReaders.begin(); it != mDataReaders.end(); )
	{
		if (service->getName() == (*it)->getName())
		{
			mDataReaders.erase(it++);
		}
		else
			++it;
	}
}

QString FileManagerServiceBase::getFileReaderName(const QString &filename)
{
	FileReaderWriterServicePtr reader = this->findReader(filename);
	if (reader)
		return reader->getName();
	else
		return QString();
}

} // cx
