/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILEREADERWRITERSERVICEPROXY_H
#define CXFILEREADERWRITERSERVICEPROXY_H

#include "cxResourceExport.h"
#include "cxFileReaderWriterService.h"
class ctkPluginContext;

namespace cx
{

class cxResource_EXPORT FileReaderWriterServiceProxy : public FileReaderWriterService
{
	Q_OBJECT
public:
	static FileReaderWriterServicePtr create(ctkPluginContext *context);

	FileReaderWriterServiceProxy(ctkPluginContext *context);
	virtual ~FileReaderWriterServiceProxy() {}

	virtual bool isNull();

	QString getName() const;
	QString getFileSuffix() const;

	bool canRead(const QString &type, const QString &filename);
	std::vector<DataPtr> read(const QString &filename);
	DataPtr read(const QString &uid, const QString &filename);
	QString canReadDataType() const;
	bool readInto(DataPtr data, QString path);

	QString canWriteDataType() const;
	bool canWrite(const QString &type, const QString &filename) const;
	void write(DataPtr data, const QString &filename);

	vtkImageDataPtr loadVtkImageData(QString filename);
	vtkPolyDataPtr loadVtkPolyData(QString filename);

private:
	ctkPluginContext *mPluginContext;
	FileReaderWriterServicePtr mService;
};
}

#endif // CXFILEREADERWRITERSERVICEPROXY_H
