#include "catch.hpp"

#include "cxReporter.h"
#include "cxtestTestToolMesh.h"

#include <QStringList>
#include "cxToolManagerUsingIGSTK.h"
#include "cxEnumConverter.h"
#include "cxDefinitions.h"
#include "cxTrackerConfiguration.h"
#include "cxFileHelpers.h"
#include "cxDataLocations.h"

namespace cxtest
{
TEST_CASE("Sonowand tools' STL files are readable", "[unit][tool]")
{
	cx::Reporter::initialize();

	TestToolMesh *meshTester = new TestToolMesh();
	meshTester->setToolPath("Neurology/SW-Invite/SW-Intraoperative-Navigator-07-081-0223_POLARIS/");
	CHECK(meshTester->canLoadMesh("SW-Intraop-Navigator.stl"));

	meshTester->setToolPath("Neurology/SW-Invite/SW-Planning-Navigator_01-117-0329_POLARIS/");
	REQUIRE(meshTester->canLoadMesh("01-117-0329_Planning-Navigator.stl"));
	delete meshTester;

	cx::Reporter::shutdown();
}

TEST_CASE("Tool xml files use tracking systems supported by ToolManagerUsingIGSTK", "[unit][tool][xml]")
{
	cx::TrackingServicePtr trackingService = cx::ToolManagerUsingIGSTK::create();
	cx::TrackerConfigurationPtr config = trackingService->getConfiguration();
	QStringList trackingSystems = trackingService->getSupportedTrackingSystems();

	//Verify tool uses supported tracking system
	foreach(QString filename, config->getAbsoluteFilePathToAllTools())
	{
		QString toolTrackingSystem = config->getToolTrackingSystem(filename);

		INFO("Filename: " + filename.toStdString());
		INFO("Tracking system: " + toolTrackingSystem.toStdString());
		REQUIRE(trackingSystems.contains(toolTrackingSystem, Qt::CaseInsensitive));
	}
}

TEST_CASE("Tool configuration files", "[unit][tool][xml]")
{
	cx::TrackingServicePtr trackingService = cx::ToolManagerUsingIGSTK::create();
	cx::TrackerConfigurationPtr config = trackingService->getConfiguration();
	QString configFilePath = cx::DataLocations::getRootConfigPath() + "/tool/";

	foreach(QFileInfo dir, cx::getDirs(configFilePath))
	{
		foreach(QString filename, cx::getAbsolutePathToXmlFiles(dir.absoluteFilePath()))
		{
			INFO("Tool config file: " + filename.toStdString());
			cx::TrackerConfiguration::Configuration configData = config->getConfiguration(filename);
			QStringList selectedTools = configData.mTools;
			foreach(QString toolFileName, selectedTools)
			{
				QFileInfo file(toolFileName);
				INFO("Tool file: " + toolFileName.toStdString());
				REQUIRE(file.exists());
				REQUIRE(configData.mTrackingSystem == config->getToolTrackingSystem(toolFileName));
			}
		}
	}
}

TEST_CASE("Tool xml files got existing image files", "[unit][tool][xml]")
{
	cx::TrackingServicePtr trackingService = cx::ToolManagerUsingIGSTK::create();
	cx::TrackerConfigurationPtr config = trackingService->getConfiguration();

	foreach(QString filename, config->getAbsoluteFilePathToAllTools())
	{
		QString imageFileName = config->getToolPictureFilename(filename);
		INFO("Tool file: " + filename);
//		CHECK(!imageFileName.isEmpty());
		if(!imageFileName.isEmpty())
		{
			QFileInfo imageFile(imageFileName);
			INFO("Image file: " + imageFileName);
			REQUIRE(imageFile.exists());
		}
	}
}

TEST_CASE("Verify tool xml files", "[unit][tool][xml]")
{
	cx::TrackingServicePtr trackingService = cx::ToolManagerUsingIGSTK::create();
	cx::TrackerConfigurationPtr config = trackingService->getConfiguration();

	foreach(QString filename, config->getAbsoluteFilePathToAllTools())
	{
		INFO("Tool file is faulty: " + filename.toStdString());
		REQUIRE(config->verifyTool(filename));
	}
}

} //namespace cxtest
