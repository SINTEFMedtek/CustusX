/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFileReaderWriterServiceNull.h"
#include "cxLogger.h"

namespace cx
{

FileReaderWriterServiceNull::FileReaderWriterServiceNull() : FileReaderWriterService()
{
}

bool FileReaderWriterServiceNull::isNull()
{
	return true;
}

bool FileReaderWriterServiceNull::canRead(const QString &type, const QString &filename)
{
	return false;
}

std::vector<DataPtr> FileReaderWriterServiceNull::read(const QString &filename)
{
	return std::vector<DataPtr>();
}

DataPtr FileReaderWriterServiceNull::read(const QString &uid, const QString &filename)
{
	return DataPtr();
}

QString FileReaderWriterServiceNull::canReadDataType() const
{
	return "";
}

bool FileReaderWriterServiceNull::readInto(DataPtr data, QString path)
{
	return false;
}

void FileReaderWriterServiceNull::write(DataPtr data, const QString &filename)
{

}


} // cx


QString cx::FileReaderWriterServiceNull::canWriteDataType() const
{
	return "";
}

bool cx::FileReaderWriterServiceNull::canWrite(const QString &type, const QString &filename) const
{
	return false;
}


QString cx::FileReaderWriterServiceNull::getName() const
{
	return "FileReaderWriterServiceNull";
}

QString cx::FileReaderWriterServiceNull::getFileSuffix() const
{
	return "";
}

vtkImageDataPtr cx::FileReaderWriterServiceNull::loadVtkImageData(QString filename)
{
	return vtkImageDataPtr();
}

vtkPolyDataPtr cx::FileReaderWriterServiceNull::loadVtkPolyData(QString filename)
{
	return vtkPolyDataPtr();
}
