/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILEMANAGERLSERVICEBASE_H
#define CXFILEMANAGERLSERVICEBASE_H

#include "cxFileManagerService.h"
#include "cxFileReaderWriterService.h"
#include "cxResourceExport.h"
#include "cxServiceTrackerListener.h"
class ctkPluginContext;

namespace cx
{

/**
 * Implementation of the FileManagerService
 *
 * \ingroup org_custusx_core_filemanager
 */
class cxResource_EXPORT FileManagerServiceBase : public FileManagerService
{
public:
	Q_INTERFACES(cx::FileManagerService)

	FileManagerServiceBase();
	virtual ~FileManagerServiceBase();
	virtual bool isNull();

	//read
	QString canLoadDataType() const;
	bool canLoad(const QString& type, const QString& filename);
	DataPtr load(const QString& uid, const QString& filename);
	bool readInto(DataPtr data, QString path);
	std::vector<DataPtr> read(const QString &filename);

	//write
	void save(DataPtr data, const QString& filename);

	//utility
	vtkImageDataPtr loadVtkImageData(QString filename);
	vtkPolyDataPtr loadVtkPolyData(QString filename);

	QString findDataTypeFromFile(QString filename);
	std::vector<FileReaderWriterServicePtr> getExportersForDataType(QString dataType);
	std::vector<FileReaderWriterServicePtr> getImportersForDataType(QString dataType);

	void addFileReaderWriter(FileReaderWriterService *service);
	void removeFileReaderWriter(FileReaderWriterService *service);

	QString getFileReaderName(const QString &filename);
private:
	FileReaderWriterServicePtr findReader(const QString& path, const QString& type="unknown");
	FileReaderWriterServicePtr findWriter(const QString& path, const QString& type="unknown");
	std::set<FileReaderWriterServicePtr> mDataReaders;
};

} //cx

#endif // CXFILEMANAGERLSERVICEBASE_H
