/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFileHelpers.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include "cxLogger.h"

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

void forceNewlineBeforeEof(QString path)
{
	QFile the_file(path);
	the_file.open(QFile::ReadWrite);
	QByteArray all = the_file.readAll();
	if(!all.endsWith("\n"))
	{
		CX_LOG_WARNING() << "File does not end with whitespace, adding newline to the file: " << path;
		QTextStream out(&the_file);
		out << endl;
	}
	the_file.close();

}

} // namespace cx
