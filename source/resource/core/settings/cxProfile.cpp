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

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


Profile::Profile(QString path, SettingsPtr settings)
{
	mPath = path;
	mSettings = settings;
}

Profile::~Profile()
{
	this->getXmlSettings().save();
}

void Profile::activate()
{
	// this will trigger lots of change signals. Do after Profile object is in place.
	mSettings->resetFile(this->getSettingsFile());
}

XmlOptionFile Profile::getXmlSettings()
{
	QString filename = 	this->getSettingsPath() + "/settings.xml";
	return  XmlOptionFile(filename);
}

Settings* Profile::getSettings() const
{
	return mSettings.get();
}

QString Profile::getSettingsFile()
{
	QString filename = this->getSettingsPath() + "/settings.ini";
	return filename;
}

QString Profile::getName() const
{
	return this->getUid();
}

QString Profile::getUid() const
{
	return QFileInfo(mPath).fileName();
}

QString Profile::getPath() const
{
	return mPath;
}

QStringList Profile::getApplicationToolConfigPaths()
{
	return QStringList() << this->getPath() + "/tool/";
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


QString Profile::getDefaultSessionRootFolder() const
{
	QStringList path;
	path << QDir::homePath() << "Patients" << this->getName();
	return path.join("/");
}

QString Profile::getSessionRootFolder() const
{
	QString folder = this->getSettings()->value("globalPatientDataFolder",
									   this->getDefaultSessionRootFolder()).toString();

	// Create folders
	if (!QDir().exists(folder))
	{
		QDir().mkdir(folder);
		report("Made a new patient folder: " + folder);
	}

	return folder;
}

void Profile::setSessionRootFolder(QString path)
{
	this->getSettings()->setValueIfNotDefault("globalPatientDataFolder", path, this->getDefaultSessionRootFolder());
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

cxResource_EXPORT ProfilePtr profile()
{
	return ProfileManager::getInstance()->activeProfile();
}

ProfileManager *ProfileManager::mInstance = NULL;

ProfileManager* ProfileManager::getInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new ProfileManager();
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

	mSettings.reset(new Settings());

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
	QString filename = this->getCustomPath() + "/generic_settings.ini";
	return QSettingsPtr(new QSettings(filename, QSettings::IniFormat));
}

QStringList ProfileManager::getInstalledProfiles()
{
	QStringList configPaths = DataLocations::getRootConfigPaths();
	QStringList profiles;
	for (int i=0; i< configPaths.size(); ++i)
		profiles << getProfilesInFolder(configPaths[i]+"/profiles");
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
}

QStringList ProfileManager::getProfilesInFolder(QString folder)
{
	QDir dir(folder);
	return dir.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot);
}

QStringList ProfileManager::getProfiles()
{
	QStringList profiles = this->getInstalledProfiles();
	profiles << getProfilesInFolder(this->getCustomPath());
	profiles.removeDuplicates();
	return profiles;
}

void ProfileManager::newProfile(QString uid)
{
	QString path = this->getPathForCustom(uid);

	QDir dir(path);
	dir.mkpath(".");
	dir.mkpath("tool");

	this->profilesChanged();
}

void ProfileManager::copyProfile(QString base, QString uid)
{
	QString newPath = this->getPathForCustom(uid);

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
	if (mActive && mActive->getUid()==uid)
		return;

	if (!this->getCustomProfiles().contains(uid))
	{
		this->createCustomProfile(uid);
	}

	// uid now is guaranteed to exist in the custom folder

	mActive.reset(new Profile(this->getPathForCustom(uid), mSettings));
	this->getGenericSettings()->setValue("profile", mActive->getUid());
	mActive->activate();
	emit activeProfileChanged();
//	CX_LOG_INFO() << "Set profile " << mActive->getName();
}

void ProfileManager::createCustomProfile(QString uid)
{
	if (this->getInstalledProfiles().contains(uid))
	{
		QString path = this->getPathForInstalled(uid);
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
}



} // namespace cx
