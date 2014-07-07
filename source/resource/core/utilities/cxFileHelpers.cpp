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

#include "cxFileHelpers.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>

namespace cx
{

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
