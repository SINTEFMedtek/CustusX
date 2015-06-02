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
#ifndef CXPROFILE_H
#define CXPROFILE_H

#include "cxResourceExport.h"

#include <QString>
#include <QObject>
#include "boost/shared_ptr.hpp"

class QSettings;

namespace cx
{
typedef boost::shared_ptr<class QSettings> QSettingsPtr;
typedef boost::shared_ptr<class Settings> SettingsPtr;
typedef boost::shared_ptr<class StringProperty> StringPropertyPtr;

class XmlOptionFile;
class Settings;

typedef boost::shared_ptr<class Profile> ProfilePtr;

/** Access to profile-specific data.
 *
 *  A profile is a collection of configuration data that
 *  is selectable by the user. Several profiles can exist,
 *  but only one can be active at a time.
 *
 * \ingroup cx_resource_core_settings
 */
class cxResource_EXPORT Profile : public QObject
{
public:
	Profile(QString path, SettingsPtr settings);
	void activate(); /// internal use

	/**
	 * Return settings xml file.
	 */
	XmlOptionFile getXmlSettings();

	/**
	 * Return settings ini file.
	 */
	Settings* getSettings() const;

	/**
	 * Return the user-friendly name of profile.
	 */
	QString getName() const;

	/**
	 * Return the unique uid of profile.
	 */
	QString getUid() const;

	/**
	 * Return location of profile-specific data. Writable.
	 */
	QString getPath() const;

	/**
	 * Return all paths where tool config files can be found.
	 * TODO: must know where to save files, one loc.
	 */
	QStringList getApplicationToolConfigPaths();

	/**
	 * Return the full path to the current tool config file.
	 */
	QString getToolConfigFilePath();
	void setToolConfigFilePath(QString path);

	/**
	 * Return location of profile-specific settings.
	 */
	QString getSettingsPath();

	/**
	 * Return root path of all locations where readable config can be found,
	 * including general (profile-unspecific) locations.
	 * This includes the value of getPath(), which also is writable.
	 */
	QStringList getAllRootConfigPaths();

	/**
	 * Return root folder where sessions are to be stored
	 */
	QString getSessionRootFolder() const;
	void setSessionRootFolder(QString path);

private:
	QString mPath;
	SettingsPtr mSettings;
	QString getSettingsFile();
	QString getDefaultSessionRootFolder() const;
};

/** Manager for the collection of profiles.
 *
 * \ingroup cx_resource_core_settings
 */
class cxResource_EXPORT ProfileManager : public QObject
{
	Q_OBJECT
public:
	static ProfileManager* getInstance(); ///< returns the only instance of this class
	static void initialize();
	static void shutdown();

	QStringList getProfiles();

	ProfilePtr activeProfile();
	void setActiveProfile(QString uid);

	/**
	 * Path to writable settings folder common to all profiles.
	 */
	QString getSettingsPath();

signals:
	void activeProfileChanged();

private:
	ProfileManager();
	~ProfileManager();
	static ProfileManager* mInstance; ///< The only instance of this class that can exist.
	ProfilePtr mActive;
	QSettingsPtr getGenericSettings();

	SettingsPtr mSettings; ///< used by Profile, changes content for each profile change

	QString getDefaultProfileUid();
	void profilesChanged();
	QStringList getInstalledProfiles();
	QStringList getCustomProfiles();
	QStringList getProfilesInFolder(QString folder);

	QString getCustomPath();
	void newProfile(QString uid);
	void copyProfile(QString base, QString uid);
	QString getPathForInstalled(QString uid);
	QString getPathForCustom(QString uid);
	void createCustomProfile(QString uid);
};

cxResource_EXPORT ProfilePtr profile();



} // namespace cx


#endif // CXPROFILE_H
