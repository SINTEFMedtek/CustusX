/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <QFileInfo>
#include "cxDataLocations.h"
#include "cxToolConfigurationParser.h"
#include "cxToolFileParser.h"

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

} // namespace cxtest
