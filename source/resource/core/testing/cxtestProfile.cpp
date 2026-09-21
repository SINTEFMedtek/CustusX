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

} // namespace cxtest
