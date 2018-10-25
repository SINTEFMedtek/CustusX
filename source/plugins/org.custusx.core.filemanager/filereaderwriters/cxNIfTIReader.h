/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXNIFTIREADER_H
#define CXNIFTIREADER_H

#include "cxFileReaderWriterService.h"
#include "org_custusx_core_filemanager_Export.h"

namespace cx
{

/**\brief Reader for NIfTI files.
 *
 */
class org_custusx_core_filemanager_EXPORT NIfTIReader: public FileReaderWriterImplService
{
public:
	NIfTIReader(PatientModelServicePtr patientModelService);
	virtual ~NIfTIReader();

	bool isNull();

	bool canRead(const QString &type, const QString &filename);
	bool readInto(ImagePtr image, QString filename);
	virtual bool readInto(DataPtr data, QString path);
	QString canReadDataType() const;
	DataPtr read(const QString &uid, const QString &filename);
	std::vector<DataPtr> read(const QString &filename);

	QString canWriteDataType() const;
	bool canWrite(const QString &type, const QString &filename) const;
	void write(DataPtr data, const QString &filename);

	virtual vtkImageDataPtr loadVtkImageData(QString filename);

private:
	vtkMatrix4x4Ptr sform_matrix; ///< the sform stores a general affine transformation which can map the image coordinates into a standard coordinate system, like Talairach or MNI
};

}

#endif // CXNIFTIREADER_H
