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

TEST_CASE("Tool configurations: Polaris NDI tools are valid", "[unit][tool]")
{
	checkToolConfiguration("LabDefault.xml", cx::tsPOLARIS, 2);
}

TEST_CASE("Tool configurations: Aurora tools on ports 1-4 are valid", "[unit][tool]")
{
	checkToolConfiguration("LabAuroraTools.xml", cx::tsAURORA, 4);
}

TEST_CASE("Tool configurations: a missing tool file is skipped when saving a configuration", "[unit][tool]")
{
	cx::DataLocations::setTestMode();
	QString validTool = cx::DataLocations::getRootConfigPath() + "/tool/Tools/Shared/NDI-Reference-frame_small_POLARIS/NDI_ref_small.xml";
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
	CHECK(content.contains("NDI_ref_small.xml"));
	CHECK_FALSE(content.contains("removed.xml"));
}

TEST_CASE("Tool configurations: a missing tool file gives an empty, invalid tool", "[unit][tool]")
{
	cx::TrackerConfigurationImpl config;
	QString missing = "/nonexistent/Removed_POLARIS/removed.xml";
	CHECK(config.getTool(missing).mTrackingSystemName.isEmpty());
	CHECK_FALSE(config.verifyTool(missing));
}

TEST_CASE("Tool configurations: the OpenIGTLink ids in a configuration are kept when it is saved", "[unit][tool]")
{
	// The NDI reference tool file has no OpenIGTLink id: it is only in the configuration
	cx::DataLocations::setTestMode();
	QString toolFolder = cx::DataLocations::getRootConfigPath() + "/tool/Tools/Shared/NDI-Reference-frame_small_POLARIS";
	QTemporaryDir folder;
	cx::ConfigurationFileParser::ToolStructure reference;
	reference.mAbsoluteToolFilePath = toolFolder + "/NDI_ref_small.xml";
	reference.mReference = true;
	reference.mOpenIGTLinkTransformId = "MyRef";
	cx::ConfigurationFileParser::Configuration original;
	original.mFileName = folder.path() + "/original.xml";
	original.mClinical_app = "Laboratory";
	original.mTrackingSystemImplementation = "ndi";
	original.mApplyRefToTools = false;
	original.mTrackersAndTools[cx::tsPOLARIS] = {reference};
	cx::ConfigurationFileParser::saveConfiguration(original);

	cx::TrackerConfigurationImpl trackerConfiguration;
	cx::TrackerConfiguration::Configuration config = trackerConfiguration.getConfiguration(original.mFileName);
	config.mUid = folder.path() + "/saved.xml";
	config.mTools = QStringList() << toolFolder + "/../NDI-Reference-frame_small_POLARIS/NDI_ref_small.xml"; // another spelling of the same file
	config.mReferenceTool = config.mTools[0];
	trackerConfiguration.saveConfiguration(config);

	std::vector<cx::ConfigurationFileParser::ToolStructure> saved = cx::ConfigurationFileParser(config.mUid).getToolListWithMetaInformation();
	REQUIRE(saved.size() == 1);
	CHECK(saved[0].mOpenIGTLinkTransformId == "MyRef");
}

TEST_CASE("Tool configurations: two missing tool files keep their own OpenIGTLink ids when saved", "[unit][tool]")
{
	QTemporaryDir folder;
	cx::TrackerConfiguration::Configuration config;
	config.mUid = folder.path() + "/saved.xml";
	config.mClinicalApplication = "Laboratory";
	config.mTrackingSystemImplementation = "ndi";
	config.mTrackingSystemName = "polaris";
	config.mApplyRefToTools = false;
	config.mTools << folder.path() + "/A_POLARIS/a.xml" << folder.path() + "/B_POLARIS/b.xml";
	cx::ConfigurationFileParser::ToolStructure a;
	a.mAbsoluteToolFilePath = config.mTools[0];
	a.mOpenIGTLinkTransformId = "a";
	cx::ConfigurationFileParser::ToolStructure b;
	b.mAbsoluteToolFilePath = config.mTools[1];
	b.mOpenIGTLinkTransformId = "b";
	config.mToolList = {a, b};

	std::vector<cx::ConfigurationFileParser::ToolStructure> structures = cx::TrackerConfigurationImpl::createToolStructures(config);
	REQUIRE(structures.size() == 2);
	CHECK(structures[0].mOpenIGTLinkTransformId == "a");
	CHECK(structures[1].mOpenIGTLinkTransformId == "b");
}

} // namespace cxtest
