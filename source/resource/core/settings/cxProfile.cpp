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

#include "cxProfile.h"

#include "cxDataLocations.h"

#include <iostream>
#include <QApplication>
#include <QDir>
#include "cxConfig.h"
#include "cxSettings.h"
#include "cxFileHelpers.h"
#include "cxTypeConversions.h"
#include "cxXmlOptionItem.h"
#include "cxStringProperty.h"
#include "cxLogger.h"

namespace cx
{

//https://qt.gitorious.org/qt-creator/qt-creator/source/1a37da73abb60ad06b7e33983ca51b266be5910e:src/app/main.cpp#L13-189
// taken from utils/fileutils.cpp. We can not use utils here since that depends app_version.h.
static bool copyRecursively(const QString &srcFilePath,
							const QString &tgtFilePath)
{
	QFileInfo srcFileInfo(srcFilePath);
	if (srcFileInfo.isDir())
	{
		QDir targetDir(tgtFilePath);
		if (!targetDir.mkpath("."))
			return false;
//		targetDir.cdUp();
//		if (!targetDir.mkdir(QFileInfo(tgtFilePath).fileName()))
//			return false;
		QDir sourceDir(srcFilePath);
		QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
		foreach (const QString &fileName, fileNames)
		{
			const QString newSrcFilePath = srcFilePath + QLatin1Char('/') + fileName;
			const QString newTgtFilePath = tgtFilePath + QLatin1Char('/') + fileName;
			if (!copyRecursively(newSrcFilePath, newTgtFilePath))
				return false;
		}
	} else
	{
		if (!QFile::copy(srcFilePath, tgtFilePath))
			return false;
	}
	return true;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


Profile::Profile(QString path)
{
	mPath = path;
}

XmlOptionFile Profile::getXmlSettings()
{
	QString filename = 	this->getSettingsPath() + "/settings.xml";
	return  XmlOptionFile(filename);
}

Settings* Profile::getSettings()
{
	return Settings::getInstance();
//	QString filename = this->getSettingsPath() + "/settings.ini";
//	if (!mSettings)
//		mSettings.reset(new Settings(filename));
//	return mSettings.get();
}

QString Profile::getSettingsFile()
{
	QString filename = this->getSettingsPath() + "/settings.ini";
	return filename;
}

QString Profile::getName()
{
	return this->getUid();
}

QString Profile::getUid()
{
	return QFileInfo(mPath).fileName();
}

QString Profile::getPath()
{
	return mPath;
}

QStringList Profile::getApplicationToolConfigPaths()
{
	return QStringList() << this->getPath() + "/tool/";
//	QString suffix("/tool/" + settings()->value("globalApplicationName").toString());
//	QStringList root = getRootConfigPaths();
//	return appendStringToAllElements(root, suffix);
}

void Profile::setToolConfigFilePath(QString path)
{
	QString expectedPath = this->getApplicationToolConfigPaths().front();

	QFileInfo info(path);
	if (info.absolutePath() != expectedPath)
		CX_LOG_WARNING() << "Set ref to file " << path << ", should be in folder " << expectedPath;

	settings()->setValue("toolConfigFile", info.fileName());
}

QString Profile::getToolConfigFilePath()
{
	QString path = this->getApplicationToolConfigPaths().front();
	QString filename = this->getSettings()->value("toolConfigFile").toString();
	if (filename.isEmpty())
		return "";
	return path + "/" + filename;
//	QString relPath("/tool/" + settings()->value("globalApplicationName").toString());
//	QString filename = settings()->value("toolConfigFile").toString();
//	return getExistingConfigPath(relPath, "", filename);
}

QString Profile::getSettingsPath()
{
	return this->getPath() + "/settings";
}

QStringList Profile::getAllRootConfigPaths()
{
	QStringList retval = DataLocations::getRootConfigPaths();
	retval << this->getPath();
	return retval;
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

ProfileManager *ProfileManager::mInstance = NULL;

//Settings* profile()
//{
//	return Settings::getInstance();
//}
ProfileManager* ProfileManager::getInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new ProfileManager();
		mInstance->initialize();
	}
	return mInstance;
}

void ProfileManager::initialize()
{
	ProfileManager::getInstance();
}

void ProfileManager::shutdown()
{
	delete mInstance;
	mInstance = NULL;
}

ProfileManager::ProfileManager()
{
	QString defaultProfile = this->getDefaultProfileUid();
	defaultProfile = this->getGenericSettings()->value("profile", defaultProfile).toString();

	this->setActiveProfile(defaultProfile);
}

ProfileManager::~ProfileManager()
{

}

QString ProfileManager::getDefaultProfileUid()
{
	QStringList installed = this->getInstalledProfiles();
	if (installed.contains("Laboratory"))
	{
		return "Laboratory";
	}
	else if (!installed.isEmpty())
	{
		return installed.front();
	}
	else
	{
		return "default";
	}
}

QString ProfileManager::getSettingsPath()
{
	return DataLocations::getPersistentWritablePath()+"/settings";
}

QSettingsPtr ProfileManager::getGenericSettings()
{
	QString filename = this->getCustomPath() + "/settings.ini";
	return QSettingsPtr(new QSettings(filename, QSettings::IniFormat));
}

QStringList ProfileManager::getInstalledProfiles()
{
	QStringList configPaths = DataLocations::getRootConfigPaths();
	QStringList profiles;
	for (int i=0; i< configPaths.size(); ++i)
		profiles << getProfilesInFolder(configPaths[i]+"/profiles");
//	CX_LOG_CHANNEL_DEBUG("profile") << profiles.join("-");
	return profiles;
}
QStringList ProfileManager::getCustomProfiles()
{
	QStringList profiles;
	profiles << getProfilesInFolder(this->getCustomPath());
	return profiles;
}

QString ProfileManager::getCustomPath()
{
	return DataLocations::getPersistentWritablePath() + "/profiles";
//	QString homepath = QDir::homePath() + "/cx_settings/profiles";
//	return homepath;
}

QStringList ProfileManager::getProfilesInFolder(QString folder)
{
	QDir dir(folder);
	return dir.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot);
}

QStringList ProfileManager::getProfiles()
{
	QStringList profiles = this->getInstalledProfiles();
//	CX_LOG_CHANNEL_DEBUG("profile")  << profiles.join(" - ");
	profiles << getProfilesInFolder(this->getCustomPath());
//	CX_LOG_CHANNEL_DEBUG("profile") << profiles.join(" - ");
	profiles.removeDuplicates();
	return profiles;
}

void ProfileManager::newProfile(QString uid)
{
//	CX_LOG_CHANNEL_DEBUG("profile") << " new: " << uid;
	QString path = this->getPathForCustom(uid);

	QDir dir(path);
	dir.mkpath(".");
	dir.mkpath("tool");

	this->profilesChanged();
}

void ProfileManager::copyProfile(QString base, QString uid)
{
	QString newPath = this->getPathForCustom(uid);
//	CX_LOG_CHANNEL_DEBUG("profile") << base << " copy to " << newPath;

	if (!copyRecursively(base, newPath))
		CX_LOG_WARNING() << "Failed to copy profile " << base << " to " << newPath;


	this->profilesChanged();
}

ProfilePtr ProfileManager::activeProfile()
{
	return mActive;
}

QString ProfileManager::getPathForInstalled(QString uid)
{
	QStringList configPaths = DataLocations::getRootConfigPaths();
	QStringList profiles;
	for (int i=0; i< configPaths.size(); ++i)
	{
		QFileInfo info(configPaths[i]+"/profiles/"+uid);
		if (info.exists())
			return info.canonicalFilePath();
	}
	return "";
}

QString ProfileManager::getPathForCustom(QString uid)
{
	return this->getCustomPath() + "/" + uid;
}

void ProfileManager::setActiveProfile(QString uid)
{
//	CX_LOG_CHANNEL_DEBUG("profile") << " set: " << uid;

	if (mActive && mActive->getUid()==uid)
		return;

	if (!this->getCustomProfiles().contains(uid))
	{
		this->createCustomProfile(uid);
	}

//	CX_LOG_CHANNEL_DEBUG("profile") << " set2: " << uid;
	// uid now is guaranteed to exist in the custom folder

	mActive.reset(new Profile(this->getPathForCustom(uid)));
	this->getGenericSettings()->setValue("profile", mActive->getUid());
	if (mSelector)
		mSelector->setValue(mActive->getUid());
	emit activeProfileChanged();
}

void ProfileManager::createCustomProfile(QString uid)
{
	if (this->getInstalledProfiles().contains(uid))
	{
		QString path = this->getPathForInstalled(uid);
//		CX_LOG_CHANNEL_DEBUG("profile") << "getpathforisntalled: " << uid << " path " << path;
		this->copyProfile(path, uid);
	}
	else
	{
		if (mActive)
		{
			this->copyProfile(mActive->getPath(), uid);
		}
		else if (!this->getInstalledProfiles().isEmpty())
		{
			QString base = this->getInstalledProfiles().front();
			QString basePath = this->getPathForInstalled(base);
			this->copyProfile(basePath, uid);
		}
		else
		{
			this->newProfile(uid);
		}
	}
}

void ProfileManager::profilesChanged()
{
	if (mSelector)
		mSelector->setValueRange(this->getProfiles());
}

StringPropertyPtr ProfileManager::getProfileSelector()
{
	if (!mSelector)
	{
//		static StringPropertyPtr initialize(const QString& uid, QString name, QString help, QString value,
//		                                    QStringList range, QDomNode root = QDomNode());

//		StringPropertyPtr retval;
		QString defaultValue = mActive->getUid();
		mSelector = StringProperty::initialize("profile", "Profile",
											"Choose profile, containing settings and configuration",
											defaultValue, this->getProfiles(), QDomNode());

		connect(mSelector.get(), &StringProperty::valueWasSet, this, &ProfileManager::onProfileSelected);
		mSelector->setValueRange(this->getProfiles());
	}

	return mSelector;
}

void ProfileManager::onProfileSelected()
{
	this->setActiveProfile(mSelector->getValue());
}


} // namespace cx
