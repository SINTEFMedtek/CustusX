/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxProfile.h"

namespace cx
{

//---------------------------------------------------------
bool DataLocations::mTestMode = false;
bool DataLocations::mRunFromBuildFolder = false;
bool DataLocations::mBuildFolderChecked = false;
QString DataLocations::mWebsiteUrl = "";
//---------------------------------------------------------

void DataLocations::setTestMode()
{
	mTestMode = true;
	cx::removeNonemptyDirRecursively(getTestDataPath() + "/temp");
	cx::removeNonemptyDirRecursively(getPersistentWritablePath());
}

bool DataLocations::isTestMode()
{
	return mTestMode;
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
	QString homepath = QDir::homePath() + "/" + CX_SYSTEM_BASE_NAME + "_settings";

	if (mTestMode)
		homepath = homepath + "/temp";

	return homepath;
}

void DataLocations::deletePersistentWritablePath()
{
	QString pathToDelete = DataLocations::getPersistentWritablePath();
	QDir dir(pathToDelete);
	CX_LOG_INFO() << "Going to delete:" << dir.absolutePath();
	dir.removeRecursively();
}


QString DataLocations::getBundlePath()
{
	// This method is becoming problematic (2015-11-30/CA):
	// the APPLE case returns path to folder enclosing the bundle
	// while the LINUX/WIN case returns path to bin folder.
	// This is not symmetric - it should be. Try to migrate away from this
	// method, using applicationDirPath instead, and remove it.
	//
#ifdef __APPLE__
	QString path(qApp->applicationDirPath()+"/../../..");
	QString bundle = QDir(qApp->applicationDirPath()+"/../..").canonicalPath();
	if (QFileInfo(bundle).isBundle())
		return QDir(path).canonicalPath();
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

	if(!isRunFromBuildFolder())
	{
		QString appPath(qApp->applicationDirPath());

		QString installLocation = appPath;
#ifndef CX_WINDOWS
		installLocation = appPath + "/plugins";
#endif
		if (QFile(installLocation).exists())
			retval << installLocation;

		QString fallbackInstallLocation = appPath;
		if (QFile(fallbackInstallLocation).exists())
			retval << fallbackInstallLocation;	}
	else
	{
		QString bundlePath = DataLocations::getBundlePath();

		QString buildLocation = bundlePath;
#ifndef CX_WINDOWS
		buildLocation = bundlePath + "/plugins";
#endif
		if (QFile(buildLocation).exists())
			retval <<  buildLocation;
	}

	return retval;
}

QString DataLocations::getRootConfigPath()
{
	QStringList paths = getRootConfigPaths();
	if (paths.empty())
		return "";
	// Those who ask for a single (legacy) config path need
	// the default CX path, not the override.
	return paths.back();
}

QStringList DataLocations::getRootConfigPaths()
{
	if(!isRunFromBuildFolder())
	{
		// look for installed location
		QString path = QString("%1/%2").arg(qApp->applicationDirPath()).arg(CX_CONFIG_ROOT_RELATIVE_INSTALLED);
		if (QDir(path).exists())
			return QStringList() << QDir(path).canonicalPath();
		else
		{
			std::cout << "DataLocations::getRootConfigPaths(): Cannot find config root path: " << path << std::endl;
			return QStringList();
		}
	}

	// add folders with the most important first: If the same file exists in both locations,
	// the first should be prefered.
	QStringList retval;
	if (QDir(CX_OPTIONAL_CONFIG_ROOT).exists()) // look for override folder in source code
		retval << QDir(CX_OPTIONAL_CONFIG_ROOT).canonicalPath();
	if (QDir(CX_CONFIG_ROOT).exists()) // look for default folder in source code
		retval << QDir(CX_CONFIG_ROOT).canonicalPath();

	return retval;
}

QString DataLocations::getDocPath()
{
	if(!isRunFromBuildFolder())
	{
		QString path = QString("%1/%2").arg(qApp->applicationDirPath()).arg(CX_DOC_ROOT_RELATIVE_INSTALLED);
		//		QString path = getBundlePath() + "/" + CX_DOC_ROOT_RELATIVE_INSTALLED; // look for installed location
		if (QDir(path).exists())
			return QDir(path).canonicalPath();
		else
		{
			CX_LOG_ERROR() << QString("Cannot find doc path: ") << path;
			return "";
		}
	}

	if (QDir(CX_DOC_ROOT).exists()) // look for folder in source code
		return QDir(CX_DOC_ROOT).canonicalPath();
	else
	{
		CX_LOG_ERROR() << QString("Cannot find doc path: ") << CX_DOC_ROOT;
		return "";
	}
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
} //namespace

QString DataLocations::getCachePath()
{
	QString path(getPersistentWritablePath()+"/cache");
	return path;
}

QString DataLocations::findConfigFolder(QString pathRelativeToConfigRoot, QString alternativeAbsolutePath)
{
	return findConfigPath("", pathRelativeToConfigRoot, alternativeAbsolutePath);
}

QString DataLocations::findConfigPath(QString fileName, QString pathRelativeToConfigRoot, QString alternativeAbsolutePath)
{
	QFileInfo filePath(findConfigFilePath(fileName, pathRelativeToConfigRoot, alternativeAbsolutePath));
	return filePath.absolutePath() + "/";
}

QString DataLocations::findConfigFilePath(QString fileName, QString pathRelativeToConfigRoot, QString alternativeAbsolutePath)
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
	//#ifdef __APPLE__
	//	// run from installed folder on mac
	//	result = DataLocations::checkExecutableExist(qApp->applicationDirPath(), filename);
	//	if (!result.isEmpty())
	//		return result;
	//#endif
	// run from installed or build bin folder
	result = DataLocations::checkExecutableExist(qApp->applicationDirPath(), filename);
	if (!result.isEmpty())
		return result;

	return result;
}

void DataLocations::setWebsiteURL(QString websiteUrl)
{
	mWebsiteUrl = websiteUrl;
}

QString DataLocations::getWebsiteURL()
{
	return mWebsiteUrl;
}

QString DataLocations::getUploadsUrl()
{
	return QString("http://custusx.org/uploads");
}

QString DataLocations::getWebsiteUserDocumentationURL()
{
	QString version(CustusX_VERSION_STRING);
	if (version.contains("dev"))
		version = "nightly";
	QString url = QString("%1/user_doc/%2")
			.arg(DataLocations::getUploadsUrl())
			.arg(version);
	return url;
}

bool DataLocations::isRunFromBuildFolder()
{
	if(!mBuildFolderChecked)
	{
		QString bundlePath = DataLocations::getBundlePath();

		//Check if cxConfig.h file exists relative to the run application
		QString pathToConfigFile = bundlePath + "/../source/resource/core/settings/cxConfig.h";
		if (QFile(pathToConfigFile).exists())
		{
			std::cout << "Using paths from build folder" << std::endl;
			mRunFromBuildFolder = true;
		}
		else
			mRunFromBuildFolder = false;
		mBuildFolderChecked = true;
	}

	return mRunFromBuildFolder;
}

} // namespace cx
