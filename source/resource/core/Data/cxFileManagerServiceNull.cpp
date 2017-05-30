#include "cxFileManagerServiceNull.h"

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

bool FileManagerServiceNull::canLoad(const QString &type, const QString &filename)
{
	return false;
}

DataPtr FileManagerServiceNull::load(const QString &uid, const QString &filename)
{
	return DataPtr();
}

QString FileManagerServiceNull::canLoadDataType() const
{
	return "";
}

bool FileManagerServiceNull::readInto(DataPtr data, QString path)
{
	return false;
}

QString FileManagerServiceNull::findDataTypeFromFile(QString filename)
{
	return "";
}

vtkPolyDataPtr FileManagerServiceNull::loadVtkPolyData(QString filename)
{
	return vtkPolyDataPtr();
}

vtkImageDataPtr FileManagerServiceNull::loadVtkImageData(QString filename)
{
	return vtkImageDataPtr();
}

void FileManagerServiceNull::save(DataPtr data, const QString &filename)
{

}

void FileManagerServiceNull::addPort(FileReaderWriterService *service)
{

}

void FileManagerServiceNull::removePort(FileReaderWriterService *service)
{

}

}
