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
	std::vector<FileReaderWriterServicePtr>  retval;
	for (std::set<FileReaderWriterServicePtr>::iterator iter = mDataReaders.begin(); iter != mDataReaders.end(); ++iter)
	{
		if (dataType.compare((*iter)->canWriteDataType()) == 0)
			retval.push_back(*iter);
	}

	return retval;
}

std::vector<FileReaderWriterServicePtr> FileManagerServiceBase::getImportersForDataType(QString dataType)
{
	std::vector<FileReaderWriterServicePtr>  retval;
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

} // cx
