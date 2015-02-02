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
#include "cxDataLocations.h"

#include <iostream>
#include <QApplication>
#include <QDir>
#include "cxConfig.h"
#include "cxSettings.h"
#include "cxFileHelpers.h"
#include "cxTypeConversions.h"
#include "cxReporter.h"

namespace cx
{

//---------------------------------------------------------
bool DataLocations::mTestMode = false;
//---------------------------------------------------------

void DataLocations::setTestMode()
{
	mTestMode = true;
	cx::removeNonemptyDirRecursively(getTestDataPath() + "/temp");
//	cx::removeNonemptyDirRecursively(getTestDataPath() + "/temp/settings");
}

QString DataLocations::getTestDataPath()
{
	QString settingsPath = cx::DataLocations::getRootConfigPath() + "/settings";
	QString dataRootConfigFile = settingsPath + "/data_root_location.txt";
	if (QFileInfo(dataRootConfigFile).exists())
	{
		return readTestDataPathFromFile(dataRootConfigFile);
	}
	else
	{
		return CX_DATA_ROOT;
	}
}

QString DataLocations::getLargeTestDataPath()
{
	QString settingsPath = cx::DataLocations::getRootConfigPath() + "/settings";
	QString dataRootConfigFile = settingsPath + "/large_data_root_location.txt";
	if (QFileInfo(dataRootConfigFile).exists())
	{
		return readTestDataPathFromFile(dataRootConfigFile);
	}
	else
	{
		return CX_LARGE_DATA_ROOT;
	}
}

QString DataLocations::getExistingTestData(QString pathRelativeToTestDataRoot, QString filename)
{
	QString path;

	path = QString("%1/%2/%3").arg(getTestDataPath()).arg(pathRelativeToTestDataRoot).arg(filename);
	if (QFileInfo(path).exists())
		return path;

	path = QString("%1/%2/%3").arg(getLargeTestDataPath()).arg(pathRelativeToTestDataRoot).arg(filename);
	if (QFileInfo(path).exists())
		return path;

	return "";
}

QString DataLocations::readTestDataPathFromFile(QString filename)
{
	QFile file(filename);
	file.open(QFile::ReadOnly);
	QString cxDataRoot(file.readAll());
	return cxDataRoot;
}

QString DataLocations::getPersistentWritablePath()
{
	QString homepath = QDir::homePath() + "/cx_settings";

	if (mTestMode)
	{
//		QString bundlepath = getBundlePath();
//		bundlepath.replace(":","_");
//		if (bundlepath.startsWith("/"))
//			bundlepath.remove(0, 1);
//		homepath += "/test/" + bundlepath;
		homepath = getTestDataPath() + "/temp";
	}

	return homepath;
}


QString DataLocations::getBundlePath()
{
#ifdef __APPLE__
  QString path(qApp->applicationDirPath()+"/../../..");
  QString bundle = QDir(qApp->applicationDirPath()+"/../..").canonicalPath();
  if (QFileInfo(bundle).isBundle())
	  return path;
  else
	  return qApp->applicationDirPath();
#else
  QString path(qApp->applicationDirPath());
  return path;
#endif
}

QStringList DataLocations::getDefaultPluginsPath()
{
	QStringList retval;
	QString bundlePath = DataLocations::getBundlePath();
	QString appPath(qApp->applicationDirPath());

	QString buildLocation = bundlePath + "/plugins";
	if (QFile(buildLocation).exists())
		retval <<  buildLocation;

	QString installLocation = appPath + "/plugins";
	if (QFile(installLocation).exists())
		retval << installLocation;

	QString fallbackInstallLocation = appPath;
	if (QFile(fallbackInstallLocation).exists())
		retval << fallbackInstallLocation;

	return retval;
}

QString DataLocations::getRootConfigPath()
{
	QStringList paths = getRootConfigPaths();
	if (paths.empty())
		return "";
	return paths.front();
}

QStringList DataLocations::getRootConfigPaths()
{
  QString path = getBundlePath() + "/" + CX_CONFIG_ROOT_RELATIVE_INSTALLED; // look for installed location
  if (QDir(path).exists())
	return QStringList() << QDir(path).canonicalPath();

  QStringList retval;
  if (QDir(CX_CONFIG_ROOT).exists()) // look for folder in source code
	retval << QDir(CX_CONFIG_ROOT).canonicalPath();
  if (QDir(CX__OPTIONAL_CONFIG_ROOT).exists()) // look for folder in source code
	retval << QDir(CX__OPTIONAL_CONFIG_ROOT).canonicalPath();

  return retval;
}

QString DataLocations::getDocPath()
{
  QString path = getBundlePath() + "/" + CX_DOC_ROOT_RELATIVE_INSTALLED; // look for installed location
  if (QDir(path).exists())
	return QDir(path).canonicalPath();

  if (QDir(CX_DOC_ROOT).exists()) // look for folder in source code
	return QDir(CX_DOC_ROOT).canonicalPath();

  return "";
}

QStringList DataLocations::appendStringToAllElements(QStringList root, QString suffix)
{
	QStringList retval;
	for (int i=0; i<root.size(); ++i)
		retval << root[i] + suffix;
	return retval;
}

namespace
{
QString changeExtension(QString name, QString ext)
{
  QStringList splitName = name.split(".");
  splitName[splitName.size()-1] = ext;
  return splitName.join(".");
}
}

QString DataLocations::getCachePath()
{
	QString path(getPersistentWritablePath()+"/cache/");
    return path;
}

QString DataLocations::findConfigFile(QString fileName, QString pathRelativeToConfigRoot, QString alternativeAbsolutePath)
{
	QStringList paths;
	foreach (QString root, getRootConfigPaths())
	{
		QString path = root + "/" + pathRelativeToConfigRoot + "/" + fileName;
		paths << path;
		if (QFileInfo(path).exists())
			return path;
	}

	QString path = QString(alternativeAbsolutePath + "/" + fileName);
	paths << path;
	if (QFileInfo(path).exists())
		return path;

	reportWarning("DataLocations::findConfigFile. Error: Can't find " + fileName + " in any of\n" + paths.join("  \n"));
	return "";
}

QString DataLocations::checkExecutableExist(QString path, QString filename)
{
	QStringList retval;
	path = QDir::cleanPath(path);
	if (QDir(path).exists(filename))
		return QDir(DataLocations::getBundlePath()).absoluteFilePath(path + "/" + filename);
	return "";
}

QString DataLocations::findExecutableInStandardLocations(QString filename)
{
	QString result;
#ifdef __APPLE__
	// run from installed folder on mac
	result = DataLocations::checkExecutableExist(qApp->applicationDirPath(), filename);
	if (!result.isEmpty())
		return result;
#endif
	// run from installed or build bin folder
	result = DataLocations::checkExecutableExist(DataLocations::getBundlePath(), filename);
	if (!result.isEmpty())
		return result;

	return result;
}

} // namespace cx
