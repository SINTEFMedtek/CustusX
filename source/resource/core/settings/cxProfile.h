/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	~Profile();
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

	QString getPatientTemplatePath();
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
	static ProfileManager* getInstance(QString defaultProfile = QString("Laboratory")); ///< returns the only instance of this class
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
	ProfileManager(QString defaultProfile);
	~ProfileManager();
	static ProfileManager* mInstance; ///< The only instance of this class that can exist.
	ProfilePtr mActive;
	QSettingsPtr getGenericSettings();

	SettingsPtr mSettings; ///< used by Profile, changes content for each profile change

	QString getDefaultProfileUid(QString defaultProfile);
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
