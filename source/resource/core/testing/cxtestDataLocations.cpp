/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxDataLocations.h"

namespace cx
{

TEST_CASE("Website url can be set", "[unit][datalocations]")
{
    CHECK(DataLocations::getWebsiteURL().isEmpty());
    cx::DataLocations::setWebsiteURL("custusx.pages.sintef.no/CustusX/");
    CHECK(!DataLocations::getWebsiteURL().isEmpty());
}

TEST_CASE("User documentation url is set", "[unit][datalocations]")
{
    CHECK(!DataLocations::getWebsiteUserDocumentationURL().isEmpty());
}

TEST_CASE("Get user home path", "[unit][datalocations]")
{
	CHECK(!DataLocations::getUserHomePath().isEmpty());
}

TEST_CASE("Family root path is under the user's home folder", "[unit][datalocations]")
{
	QString familyRoot = DataLocations::getFamilyRootPath();
	CHECK(!familyRoot.isEmpty());
	CHECK(familyRoot.startsWith(DataLocations::getUserHomePath()));
}

TEST_CASE("Persistent writable path is nested under the family root", "[unit][datalocations]")
{
	QString familyRoot = DataLocations::getFamilyRootPath();
	QString persistentPath = DataLocations::getPersistentWritablePath();
	CHECK(persistentPath.startsWith(familyRoot + "/"));
}

TEST_CASE("Shared family data is not nested under the per-app settings path", "[unit][datalocations]")
{
	// Regression guard: models/virtualEnvironments must be shared at the family
	// level (~/<family>/models, ~/<family>/virtualEnvironments), not nested
	// under the per-app settings path, so sibling apps (e.g. Fraxinus and
	// FraxinusExcelsior) can reuse the same downloaded models/venvs without
	// duplicating multi-GB downloads. Conversely, the per-app settings path
	// itself must stay per-app (not equal to the family root, and not shared
	// between siblings) so they can run concurrently -- two apps racing to
	// read/write the same settings.ini caused exactly that bug before this was
	// split into separate family vs. per-app roots.
	QString familyRoot = DataLocations::getFamilyRootPath();
	QString persistentPath = DataLocations::getPersistentWritablePath();
	QString modelsPath = DataLocations::getModelsPath();
	QString venvPath = DataLocations::getVirtualEnvironmentsPath();

	CHECK(modelsPath.startsWith(familyRoot + "/"));
	CHECK(venvPath.startsWith(familyRoot + "/"));
	CHECK(!modelsPath.startsWith(persistentPath + "/"));
	CHECK(!venvPath.startsWith(persistentPath + "/"));
}

TEST_CASE("Filter scripts path is not tied to a specific profile", "[unit][datalocations]")
{
	QString path = DataLocations::getFilterScriptsPath();
	CHECK(path.endsWith("filter_scripts/"));
	CHECK(!path.contains("Laboratory"));
}

} // namespace cx

