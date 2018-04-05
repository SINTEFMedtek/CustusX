/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

QString Profile::getPatientTemplatePath()
{
	return DataLocations::getRootConfigPath() + "/patient_templates";
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
		if(QDir().mkpath(folder))
			report("Made a new patient folder: " + folder);
		else
			reportWarning("Cannot make new patient folder: " + folder);
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

ProfileManager* ProfileManager::getInstance(QString defaultProfile)
{
	if (mInstance == NULL)
	{
		mInstance = new ProfileManager(defaultProfile);
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

ProfileManager::ProfileManager(QString defaultProfile)
{
	QString profileUid = this->getDefaultProfileUid(defaultProfile);
	profileUid = this->getGenericSettings()->value("profile", profileUid).toString();

	mSettings.reset(new Settings());

	this->setActiveProfile(profileUid);
}

ProfileManager::~ProfileManager()
{

}

QString ProfileManager::getDefaultProfileUid(QString defaultProfile)
{
	QStringList installed = this->getInstalledProfiles();
	if (installed.contains(defaultProfile))
	{
		return defaultProfile;
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

	if (!copyRecursively(base, newPath, true))
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
