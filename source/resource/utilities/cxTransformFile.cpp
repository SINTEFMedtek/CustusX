// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxTransformFile.h"

#include <QFileInfo>
#include <QFile>

#include "sscTypeConversions.h"
#include "sscMessageManager.h"

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
ssc::Transform3D TransformFile::read(bool* ok)
{
	if (ok)
		*ok = false;
	ssc::Transform3D retval = ssc::Transform3D::Identity();
	QFile file(mFileName);
	if (!QFileInfo(mFileName).exists() || !file.open(QIODevice::ReadOnly))
	{
		if (!ok)
			ssc::messageManager()->sendWarning(QString("Failed to open file %1 for reading ").arg(file.fileName()));
		return retval;
	}
	bool localOk = true;
	QString positionString = file.readLine();
	positionString += " " + file.readLine();
	positionString += " " + file.readLine();
	positionString += " " + file.readLine();
	retval = ssc::Transform3D::fromString(positionString, &localOk);
	if (!localOk)
	{
		if (!ok)
			ssc::messageManager()->sendWarning(QString("Invalid format in file %1. Values: [%2]").arg(mFileName).arg(qstring_cast(retval(0, 0))));
		return retval;
	}
	if (ok)
		*ok = true;
	return retval;
}

void TransformFile::write(const ssc::Transform3D& transform)
{
	QFile file(mFileName);
	if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text))
	{
		ssc::messageManager()->sendWarning(QString("Failed to open file %1 for writing.").arg(file.fileName()));
		return;
	}
	file.write(qstring_cast(transform).toAscii());
}

}
