#include "catch.hpp"

#include "cxReporter.h"
#include "cxtestTestToolMesh.h"

#include <QDir>
#include <QStringList>
#include "cxToolManagerUsingIGSTK.h"
#include "cxDataLocations.h"
#include "cxEnumConverter.h"
#include "cxDefinitions.h"
#include "cxTrackerConfiguration.h"

namespace
{

QFileInfoList getDirs(QString path)
{
//	std::cout << "getDirs path: " << path << std::endl;
	QDir dir(path);
	dir.setFilter(QDir::AllDirs|QDir::NoDotAndDotDot);
	QFileInfoList retval = dir.entryInfoList();
	return retval;
}

QStringList getXmlFiles(QString path, bool includeSubDirs = false)
{
//	std::cout << "getXmlFiles path: " << path << std::endl;
	QStringList retval;
	QDir dir(path);
	dir.setFilter(QDir::Files);
	dir.setNameFilters(QStringList("*.xml"));

	foreach(QFileInfo file, dir.entryInfoList())
		retval << file.absoluteFilePath();

	if (includeSubDirs)
		foreach(QFileInfo directory, getDirs(path))
			retval << getXmlFiles(directory.absoluteFilePath(), includeSubDirs);

	return retval;
}

QStringList getAllToolFiles()
{
	bool includeSubDirs = true;
	QString toolFilePath = cx::DataLocations::getRootConfigPath() + "/tool/Tools/";
	return getXmlFiles(toolFilePath, includeSubDirs);
}

} //namespace

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
	foreach(QString filename, getAllToolFiles())
	{
		QString toolTrackingSystem = config->getToolTrackingSystem(filename);

		INFO("Filename: " + filename.toStdString());
		INFO("Tracking system: " + toolTrackingSystem.toStdString());
		REQUIRE(trackingSystems.contains(toolTrackingSystem, Qt::CaseInsensitive));
	}
}

TEST_CASE("Tool configuration files link to existing files", "[unit][tool][xml]")
{
	cx::TrackingServicePtr trackingService = cx::ToolManagerUsingIGSTK::create();
	cx::TrackerConfigurationPtr config = trackingService->getConfiguration();
	QString configFilePath = cx::DataLocations::getRootConfigPath() + "/tool/";

	foreach(QFileInfo dir, getDirs(configFilePath))
	{
		foreach(QString filename, getXmlFiles(dir.absoluteFilePath()))
		{
//			std::cout << "Tool config: " << filename << std::endl;
			cx::TrackerConfiguration::Configuration data = config->getConfiguration(filename);
			QStringList selectedTools = data.mTools;
			foreach(QString toolFileName, selectedTools)
			{
//				std::cout << "Tool: " << toolFileName.toStdString() << std::endl;
				QFileInfo file(toolFileName);
				INFO("File: " + toolFileName.toStdString() + " don't exist");
				REQUIRE(file.exists());
			}
//			std::cout << std::endl;
		}
	}
}

TEST_CASE("Tool xml files got existing image files", "[unit][tool][xml]")
{
	cx::TrackingServicePtr trackingService = cx::ToolManagerUsingIGSTK::create();
	cx::TrackerConfigurationPtr config = trackingService->getConfiguration();

	foreach(QString filename, getAllToolFiles())
	{
		QString imageFile = config->getToolPictureFilename(filename);
		{
//			INFO("Tool: " + filename + " got no image");
//			CHECK(!imageFile.isEmpty());
		}
		if(!imageFile.isEmpty())
		{
			QFileInfo file(imageFile);
			INFO("File: " + imageFile.toStdString() + " don't exist");
			REQUIRE(file.exists());
		}
	}
}

} //namespace cxtest
