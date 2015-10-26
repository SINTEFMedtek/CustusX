/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
