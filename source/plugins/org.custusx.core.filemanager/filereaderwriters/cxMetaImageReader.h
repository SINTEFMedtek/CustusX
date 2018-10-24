/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMETAIMAGEREADER_H
#define CXMETAIMAGEREADER_H

#include "cxFileReaderWriterService.h"
#include "org_custusx_core_filemanager_Export.h"
#include <QFileInfo>

class ctkPluginContext;

namespace cx
{

class org_custusx_core_filemanager_EXPORT MetaImageReader : public FileReaderWriterImplService
{
public:
	Q_INTERFACES(cx::FileReaderWriterService)

	MetaImageReader(PatientModelServicePtr patientModelService);
	virtual ~MetaImageReader(){}
	bool isNull();

	virtual bool canRead(const QString& type, const QString& filename);
	virtual QString canReadDataType() const;
	virtual bool readInto(DataPtr data, QString path);
	virtual bool readInto(ImagePtr image, QString filename);
	virtual DataPtr read(const QString& uid, const QString& filename);
	std::vector<DataPtr> read(const QString &filename);
	virtual vtkImageDataPtr loadVtkImageData(QString filename);

	QString canWriteDataType() const;
	bool canWrite(const QString &type, const QString &filename) const;
	virtual void write(DataPtr data, const QString& filename);
};

}
#endif // CXMETAIMAGEREADER_H
