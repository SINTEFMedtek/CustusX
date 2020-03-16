/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILEMANAGERSERVICENULL_H
#define CXFILEMANAGERSERVICENULL_H

#include "cxFileManagerService.h"

namespace cx
{

class cxResource_EXPORT FileManagerServiceNull : public FileManagerService
{
	Q_OBJECT
public:
	FileManagerServiceNull();
	virtual ~FileManagerServiceNull();

	virtual bool isNull();

	bool canLoad(const QString &type, const QString &filename);
	DataPtr load(const QString &uid, const QString &filename);
	QString canLoadDataType() const;
	std::vector<DataPtr> read(const QString &filename);

	bool readInto(DataPtr data, QString path);
	QString findDataTypeFromFile(QString filename);
	vtkPolyDataPtr loadVtkPolyData(QString filename);
	vtkImageDataPtr loadVtkImageData(QString filename);

	void save(DataPtr data, const QString &filename);
	void addFileReaderWriter(FileReaderWriterService *service);
	void removeFileReaderWriter(FileReaderWriterService *service);

	std::vector<FileReaderWriterServicePtr> getExportersForDataType(QString dataType);
	std::vector<FileReaderWriterServicePtr> getImportersForDataType(QString dataType);
	virtual QString getFileReaderName(const QString &filename);
private:
	void printWarning() const;
};

}

#endif // CXFILEMANAGERSERVICENULL_H
