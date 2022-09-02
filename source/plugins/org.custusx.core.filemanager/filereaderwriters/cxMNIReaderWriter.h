/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMNIREADERWRITER_H
#define CXMNIREADERWRITER_H

#include "org_custusx_core_filemanager_Export.h"
#include "cxFileReaderWriterService.h"
#include "cxForwardDeclarations.h"
class QColor;
class ctkPluginContext;

namespace cx
{

class org_custusx_core_filemanager_EXPORT MNIReaderWriter : public FileReaderWriterImplService

{
public:
	MNIReaderWriter(PatientModelServicePtr patientModelService, ViewServicePtr viewService);

	bool isNull();
	QString canReadDataType() const;
	bool canRead(const QString &type, const QString &filename);
    DataPtr read(const QString &uid, const QString &filename);
    std::vector<DataPtr> read(const QString &filename);
	bool readInto(DataPtr data, QString path);

	QString canWriteDataType() const;
	bool canWrite(const QString &type, const QString &filename) const;
	void write(DataPtr data, const QString &filename);

	void setVolumeUidsRelatedToPointsInMNIPointFile(std::vector<QString> volumeUids);

private:
	QColor getRandomColor();
	std::vector<QString> dialogForSelectingVolumesForImportedMNITagFile( int number_of_volumes, QString description);
	bool validateKnownVolumeUids(int numberOfVolumesInFile) const;

	PatientModelServicePtr mPatientModelServicePrivate;
	ViewServicePtr mViewService;

	std::vector<QString> mVolumeUids;
};
}

#endif // CXMNIREADERWRITER_H
