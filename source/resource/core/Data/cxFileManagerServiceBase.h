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

private:
	FileReaderWriterServicePtr findReader(const QString& path, const QString& type="unknown");
	FileReaderWriterServicePtr findWriter(const QString& path, const QString& type="unknown");
	std::set<FileReaderWriterServicePtr> mDataReaders;
};

} //cx

#endif // CXFILEMANAGERLSERVICEBASE_H
