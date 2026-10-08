/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"

#include "cxProfile.h"

#include <QString>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>

namespace
{
void writeFile(QString path, QString content)
{
	QFile file(path);
	file.open(QIODevice::WriteOnly);
	QTextStream(&file) << content;
}

QString readFile(QString path)
{
	QFile file(path);
	file.open(QIODevice::ReadOnly);
	return QTextStream(&file).readAll();
}
} // namespace

namespace cxtest
{

TEST_CASE("Profile::migrateStaleSessionRootFolder migrates the exact pre-family-folder default", "[unit]")
{
	QString preFamilyDefault = "/home/user/Patients/MyProfile";
	QString familyDefault = "/home/user/CustusX/Patients/MyProfile";

	CHECK(cx::Profile::migrateStaleSessionRootFolder(preFamilyDefault, preFamilyDefault, familyDefault) == familyDefault);
}

TEST_CASE("Profile::migrateStaleSessionRootFolder leaves a deliberately chosen folder untouched", "[unit]")
{
	QString preFamilyDefault = "/home/user/Patients/MyProfile";
	QString familyDefault = "/home/user/CustusX/Patients/MyProfile";
	QString userChosenFolder = "/mnt/external/MyPatients";

	CHECK(cx::Profile::migrateStaleSessionRootFolder(userChosenFolder, preFamilyDefault, familyDefault) == userChosenFolder);
}

TEST_CASE("Profile::migrateStaleSessionRootFolder is a no-op on an already-migrated folder", "[unit]")
{
	QString preFamilyDefault = "/home/user/Patients/MyProfile";
	QString familyDefault = "/home/user/CustusX/Patients/MyProfile";

	CHECK(cx::Profile::migrateStaleSessionRootFolder(familyDefault, preFamilyDefault, familyDefault) == familyDefault);
}

TEST_CASE("ProfileManager::copyMissingToolConfigs adds missing configs and keeps existing ones", "[unit]")
{
	QTemporaryDir root;
	REQUIRE(root.isValid());
	QString installed = root.path() + "/installed";
	QString custom = root.path() + "/custom";
	QDir().mkpath(installed + "/tool");
	QDir().mkpath(custom + "/tool");
	writeFile(installed + "/tool/New.xml", "new");
	writeFile(installed + "/tool/Existing.xml", "installed");
	writeFile(installed + "/tool/NotAConfig.txt", "ignored");
	writeFile(custom + "/tool/Existing.xml", "edited by user");

	QStringList copied = cx::ProfileManager::copyMissingToolConfigs(installed, custom);

	CHECK(copied == QStringList("New.xml"));
	CHECK(readFile(custom + "/tool/New.xml") == "new");
	CHECK(readFile(custom + "/tool/Existing.xml") == "edited by user");
	CHECK_FALSE(QFile::exists(custom + "/tool/NotAConfig.txt"));
	CHECK(cx::ProfileManager::copyMissingToolConfigs(installed, custom).isEmpty());
}

TEST_CASE("ProfileManager::copyMissingToolConfigs doesn't copy back a config the user deleted", "[unit]")
{
	QTemporaryDir root;
	REQUIRE(root.isValid());
	QString installed = root.path() + "/installed";
	QString custom = root.path() + "/custom";
	QDir().mkpath(installed + "/tool");
	writeFile(installed + "/tool/Shipped.xml", "shipped");

	CHECK(cx::ProfileManager::copyMissingToolConfigs(installed, custom) == QStringList("Shipped.xml"));
	QFile::remove(custom + "/tool/Shipped.xml");
	CHECK(cx::ProfileManager::copyMissingToolConfigs(installed, custom).isEmpty());
	CHECK_FALSE(QFile::exists(custom + "/tool/Shipped.xml"));

	// A newly shipped config is still added
	writeFile(installed + "/tool/NewlyShipped.xml", "new");
	CHECK(cx::ProfileManager::copyMissingToolConfigs(installed, custom) == QStringList("NewlyShipped.xml"));
}

TEST_CASE("ProfileManager::copyMissingToolConfigs does nothing without an installed profile", "[unit]")
{
	QTemporaryDir root;
	REQUIRE(root.isValid());
	QString custom = root.path() + "/custom";

	CHECK(cx::ProfileManager::copyMissingToolConfigs(root.path() + "/missing", custom).isEmpty());
	CHECK_FALSE(QDir(custom).exists());
}

} // namespace cxtest
