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

	/**
	 * @brief Simple check if file is a DICOM file.
	 * DICOM files can have several different endings (. .dcm .dicom .ima),
	 * so we instead look at the characters at position 0x80.
	 * These characters should spell DICM, as specified by DICOM standard PS3.10
	 * "Media Storage and File Format for Media Interchange"
	 * @param type
	 * @param filename
	 * @return
	 */
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
