/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXDICOMREADER_H
#define CXDICOMREADER_H

#include "cxFileReaderWriterService.h"
#include "org_custusx_core_filemanager_Export.h"

typedef QSharedPointer<class ctkDICOMDatabase> ctkDICOMDatabasePtr;

namespace cx {

class org_custusx_core_filemanager_EXPORT DICOMReader : public FileReaderWriterImplService
{
public:
	Q_INTERFACES(cx::FileReaderWriterService)
	DICOMReader(PatientModelServicePtr patientModelService);
	virtual ~DICOMReader() {}

	bool isNull(){return false;}

	virtual bool canRead(const QString& type, const QString& filename);
	virtual bool readInto(DataPtr data, QString path);
	bool readInto(ImagePtr image, QString filename);
	virtual QString canReadDataType() const;
	virtual DataPtr read(const QString& uid, const QString& filename);
	std::vector<DataPtr> read(const QString &filename);

	void write(DataPtr data, const QString &filename) {}
	QString canWriteDataType() const;
	bool canWrite(const QString &type, const QString &filename) const;

	virtual vtkImageDataPtr loadVtkImageData(QString filename);

protected:
	ImagePtr importSeries(QString fileName);
	QString getBestDICOMSeries(ctkDICOMDatabasePtr database);
};

} //cx
#endif // CXDICOMREADER_H
