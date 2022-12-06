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
class QProgressDialog;

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
	std::vector<ImagePtr> importSeries(QString fileName, bool readBestSeries);
	QString getBestDICOMSeries(ctkDICOMDatabasePtr database);
	QStringList getAllDICOMSeries(ctkDICOMDatabasePtr database);
	std::vector<ImagePtr> importBestSeries(ctkDICOMDatabasePtr database);
	std::vector<ImagePtr> importAllSeries(ctkDICOMDatabasePtr database, QProgressDialog &progress);
	void stopDCMTKMessages();
	bool canReadDir(QString dirname, bool checkSubDirs);
	bool canReadFile(QString filename);
	void addFolderToDicomDatabase(ctkDICOMDatabasePtr database, QString folder);
	QStringList findAllSubfoldersWithDicomFiles(QString folder, QProgressDialog &progress);
	QStringList findAllSubDirs(QString folder);
};

} //cx
#endif // CXDICOMREADER_H
