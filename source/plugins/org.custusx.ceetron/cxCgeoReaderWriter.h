/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCGEOREADERWRITER_H
#define CXCGEOREADERWRITER_H

#include "org_custusx_ceetron_Export.h"
#include "cxFileReaderWriterService.h"

namespace cx
{
/**
 * @brief Write triangular mesh to .cgeo format for Ceetron.
 *
 * \date May 19, 2017
 * \author Erlend F Hofstad
 */

class org_custusx_ceetron_EXPORT CgeoReaderWriter : public FileReaderWriterImplService
{
public:
	Q_INTERFACES(cx::FileReaderWriterService)

	CgeoReaderWriter(PatientModelServicePtr patientModelService);

	bool isNull();

	bool canRead(const QString &type, const QString &filename);
	std::vector<DataPtr> read(const QString &filename);
	DataPtr read(const QString &uid, const QString &filename);
	QString canReadDataType() const;
	bool readInto(DataPtr data, QString path);

	QString canWriteDataType() const;
	bool canWrite(const QString &type, const QString &filename) const;
	void write(DataPtr data, const QString &filename);
	QByteArray convertToQByteArray(DataPtr data);
private:
	void writeToStream(DataPtr data, QDataStream &out);
};

}
#endif // CXCGEOREADERWRITER_H
