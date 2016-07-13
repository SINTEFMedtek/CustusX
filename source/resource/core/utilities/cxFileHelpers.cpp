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

#include "cxFileHelpers.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>

namespace cx
{

//From http://stackoverflow.com/questions/2536524/copy-directory-using-qt
bool copyRecursively(QString sourceDir, QString destinationDir, bool overWriteDirectory)
{
	QDir originDirectory(sourceDir);

	if (! originDirectory.exists())
	{
		return false;
	}

	QDir destinationDirectory(destinationDir);

	if(destinationDirectory.exists() && !overWriteDirectory)
	{
		return false;
	}
	else if(destinationDirectory.exists() && overWriteDirectory)
	{
		destinationDirectory.removeRecursively();
	}

	originDirectory.mkpath(destinationDir);

	foreach (QString directoryName, originDirectory.entryList(QDir::Dirs | \
															  QDir::NoDotAndDotDot))
	{
		QString destinationPath = destinationDir + "/" + directoryName;
		originDirectory.mkpath(destinationPath);
		copyRecursively(sourceDir + "/" + directoryName, destinationPath, overWriteDirectory);
	}

	foreach (QString fileName, originDirectory.entryList(QDir::Files))
	{
		QFile::copy(sourceDir + "/" + fileName, destinationDir + "/" + fileName);
	}

	/*! Possible race-condition mitigation? */
	QDir finalDestination(destinationDir);
	finalDestination.refresh();

	if(finalDestination.exists())
	{
		return true;
	}

	return false;
}

bool removeNonemptyDirRecursively(const QString & dirName)
{
	bool result = false;
	QDir dir(dirName);

	if (dir.exists(dirName)) {
		Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
			if (info.isDir()) {
				result = removeNonemptyDirRecursively(info.absoluteFilePath());
			}
			else {
				result = QFile::remove(info.absoluteFilePath());
			}

			if (!result) {
				return result;
			}
		}
		result = dir.rmdir(dirName);
	}
	return result;
}

QFileInfoList getDirs(QString path)
{
	QDir dir(path);
	dir.setFilter(QDir::AllDirs|QDir::NoDotAndDotDot);
	QFileInfoList retval = dir.entryInfoList();
	return retval;
}

QStringList getAbsolutePathToFiles(QString path, QStringList nameFilters, bool includeSubDirs)
{
	QStringList retval;
	QDir dir(path);
	dir.setFilter(QDir::Files);
	dir.setNameFilters(nameFilters);

	foreach(QFileInfo file, dir.entryInfoList())
		retval << file.absoluteFilePath();

	if (includeSubDirs)
		foreach(QFileInfo directory, getDirs(path))
			retval << getAbsolutePathToFiles(directory.absoluteFilePath(), nameFilters, includeSubDirs);

	return retval;
}

QStringList getAbsolutePathToXmlFiles(QString path, bool includeSubDirs)
{
	return getAbsolutePathToFiles(path, QStringList("*.xml"), includeSubDirs);
}

} // namespace cx
