/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFileManagerServiceNull.h"

#include "cxLogger.h"

namespace cx
{
FileManagerServiceNull::FileManagerServiceNull()
{

}

FileManagerServiceNull::~FileManagerServiceNull()
{}

bool FileManagerServiceNull::isNull()
{
	return true;
}

std::vector<DataPtr> FileManagerServiceNull::read(const QString &filename)
{
	printWarning();
	return std::vector<DataPtr>();
}

bool FileManagerServiceNull::canLoad(const QString &type, const QString &filename)
{
	printWarning();
	return false;
}

DataPtr FileManagerServiceNull::load(const QString &uid, const QString &filename)
{
	printWarning();
	return DataPtr();
}

QString FileManagerServiceNull::canLoadDataType() const
{
	printWarning();
	return "";
}

bool FileManagerServiceNull::readInto(DataPtr data, QString path)
{
	printWarning();
	return false;
}

QString FileManagerServiceNull::findDataTypeFromFile(QString filename)
{
	printWarning();
	return "";
}

vtkPolyDataPtr FileManagerServiceNull::loadVtkPolyData(QString filename)
{
	printWarning();
	return vtkPolyDataPtr();
}

vtkImageDataPtr FileManagerServiceNull::loadVtkImageData(QString filename)
{
	printWarning();
	return vtkImageDataPtr();
}

void FileManagerServiceNull::save(DataPtr data, const QString &filename)
{
	printWarning();printWarning();
}

void FileManagerServiceNull::addFileReaderWriter(FileReaderWriterService *service)
{
	printWarning();
}

void FileManagerServiceNull::removeFileReaderWriter(FileReaderWriterService *service)
{
	printWarning();
}

std::vector<FileReaderWriterServicePtr> FileManagerServiceNull::getExportersForDataType(QString dataType)
{
	printWarning();
	return std::vector<FileReaderWriterServicePtr>();
}

std::vector<FileReaderWriterServicePtr> FileManagerServiceNull::getImportersForDataType(QString dataType)
{
	printWarning();
	return std::vector<FileReaderWriterServicePtr>();
}

QString FileManagerServiceNull::getFileReaderName(const QString &filename)
{
	printWarning();
	return QString();
}

void FileManagerServiceNull::printWarning() const
{
	reportWarning("Trying to use FileManagerServiceNull.");
}


}
