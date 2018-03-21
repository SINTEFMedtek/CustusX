/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTransformFile.h"

#include <QFileInfo>
#include <QFile>

#include "cxTypeConversions.h"
#include "cxLogger.h"

namespace cx
{

TransformFile::TransformFile(QString fileName) :
	mFileName(fileName)
{
}

/**
 * Reads a whitespace separated 4x4 matrix from file
 * \param ok success flag. If NULL, logger warnings are emitted instead
 * \return  The matrix
 */
Transform3D TransformFile::read(bool* ok)
{
	if (ok)
		*ok = false;
	Transform3D retval = Transform3D::Identity();
	QFile file(mFileName);
	if (!QFileInfo(mFileName).exists() || !file.open(QIODevice::ReadOnly))
	{
		if (!ok)
			reportWarning(QString("Failed to open file %1 for reading ").arg(file.fileName()));
		return retval;
	}
	bool localOk = true;
    if(!file.atEnd())
    {
        QString positionString = file.readLine();
        positionString += " " + file.readLine();
        positionString += " " + file.readLine();
        positionString += " " + file.readLine();
        retval = Transform3D::fromString(positionString, &localOk);
    }
	if (!localOk)
	{
		if (!ok)
			reportWarning(QString("Invalid format in file %1. Values: [%2]").arg(mFileName).arg(qstring_cast(retval(0, 0))));
		return retval;
	}
	if (ok)
		*ok = true;
	return retval;
}

void TransformFile::write(const Transform3D& transform)
{
	QFile file(mFileName);
	if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text))
	{
		reportWarning(QString("Failed to open file %1 for writing.").arg(file.fileName()));
		return;
	}
	file.write(qstring_cast(transform).toLatin1());
}

}
