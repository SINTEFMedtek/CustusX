/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
#include "cxDataLocations.h"
#include "cxToolConfigurationParser.h"
#include "cxToolFileParser.h"
#include "cxTrackerConfigurationImpl.h"

namespace cxtest
{

namespace
{

void checkToolConfiguration(QString configFile, cx::TRACKING_SYSTEM expectedTracker, int expectedTools)
{
	cx::DataLocations::setTestMode();
	QString path = cx::DataLocations::getRootConfigPath() + "/profiles/Laboratory/tool/" + configFile;
	INFO(path.toStdString());
	REQUIRE(QFileInfo::exists(path));

	cx::ConfigurationFileParser parser(path);
	std::vector<cx::ToolFileParser::TrackerInternalStructure> trackers = parser.getTrackers();
	REQUIRE(trackers.size() == 1);
	CHECK(trackers[0].mType == expectedTracker);

	std::vector<QString> toolFiles = parser.getAbsoluteToolFilePaths();
	CHECK(toolFiles.size() == expectedTools);
	CHECK_FALSE(parser.getAbsoluteReferenceFilePath().isEmpty());
	for (const QString& toolFile : toolFiles)
	{
		INFO(toolFile.toStdString());
		cx::ToolFileParser toolParser(toolFile);
		CHECK(toolParser.getTool()->verify());
	}
}

} // namespace

TEST_CASE("Tool configurations: Polaris SW tools are valid", "[unit][tool]")
{
	checkToolConfiguration("LabPolarisSWTools.xml", cx::tsPOLARIS, 4);
}

TEST_CASE("Tool configurations: Aurora tools on ports 1-4 are valid", "[unit][tool]")
{
	checkToolConfiguration("LabAuroraTools.xml", cx::tsAURORA, 4);
}

TEST_CASE("Tool configurations: a missing tool file is skipped when saving a configuration", "[unit][tool]")
{
	cx::DataLocations::setTestMode();
	QString validTool = cx::DataLocations::getRootConfigPath() + "/tool/Tools/Shared/SW-RefFrame-02-077-0191_POLARIS/SW_ref.xml";
	REQUIRE(QFileInfo::exists(validTool));
	QTemporaryDir folder;
	cx::ConfigurationFileParser::ToolStructure valid;
	valid.mAbsoluteToolFilePath = validTool;
	valid.mReference = true;
	cx::ConfigurationFileParser::ToolStructure missing;
	missing.mAbsoluteToolFilePath = folder.path() + "/Removed_POLARIS/removed.xml";

	cx::ConfigurationFileParser::Configuration config;
	config.mFileName = folder.path() + "/config.xml";
	config.mClinical_app = "Laboratory";
	config.mTrackingSystemImplementation = "ndi";
	config.mApplyRefToTools = false;
	config.mTrackersAndTools[cx::tsPOLARIS] = {valid, missing};
	cx::ConfigurationFileParser::saveConfiguration(config);

	QFile file(config.mFileName);
	REQUIRE(file.open(QIODevice::ReadOnly));
	QString content = QString::fromUtf8(file.readAll());
	CHECK(content.contains("SW_ref.xml"));
	CHECK_FALSE(content.contains("removed.xml"));
}

TEST_CASE("Tool configurations: a missing tool file gives an empty, invalid tool", "[unit][tool]")
{
	cx::TrackerConfigurationImpl config;
	QString missing = "/nonexistent/Removed_POLARIS/removed.xml";
	CHECK(config.getTool(missing).mTrackingSystemName.isEmpty());
	CHECK_FALSE(config.verifyTool(missing));
}

} // namespace cxtest
