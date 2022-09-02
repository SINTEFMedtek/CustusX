/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILEMANAGERSERVICE_H
#define CXFILEMANAGERSERVICE_H

#include <QObject>
#include "cxResourceExport.h"
#include "boost/shared_ptr.hpp"
#include "cxData.h"

#define FileManagerService_iid "cx::FileManagerService"

namespace cx
{

typedef boost::shared_ptr<class FileManagerService> FileManagerServicePtr;


class cxResource_EXPORT FileManagerService : public QObject
{
	Q_OBJECT
public:
	virtual ~FileManagerService() {}

	virtual bool isNull() = 0;
	static FileManagerServicePtr getNullObject();

	//TODO harmonize with filereaderwriter (read and write...)
	//read
	virtual bool canLoad(const QString& type, const QString& filename) = 0;
	virtual QString canLoadDataType() const = 0;
	virtual std::vector<DataPtr> read(const QString &filename) = 0;

	//--- TODO remove
	virtual DataPtr load(const QString& uid, const QString& filename) = 0;
	virtual bool readInto(DataPtr data, QString path) = 0;
	//---

	//write
	virtual void save(DataPtr data, const QString& filename) = 0;

	virtual vtkImageDataPtr loadVtkImageData(QString filename) = 0;
	virtual vtkPolyDataPtr loadVtkPolyData(QString filename) = 0;

	virtual void addFileReaderWriter(FileReaderWriterService *service) = 0;
	virtual void removeFileReaderWriter(FileReaderWriterService *service) = 0;

	virtual QString findDataTypeFromFile(QString filename) = 0;
	virtual std::vector<FileReaderWriterServicePtr> getExportersForDataType(QString dataType) = 0;
	virtual std::vector<FileReaderWriterServicePtr> getImportersForDataType(QString dataType) = 0;

	virtual QString getFileReaderName(const QString &filename) = 0;
};

}
Q_DECLARE_INTERFACE(cx::FileManagerService, FileManagerService_iid)

#endif // CXFILEMANAGERSERVICE_H
