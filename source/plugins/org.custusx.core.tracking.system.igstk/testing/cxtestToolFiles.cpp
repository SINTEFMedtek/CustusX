/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <QStringList>
#include "cxEnumConverter.h"
#include "cxDefinitions.h"
#include "cxTrackerConfiguration.h"
#include "cxTrackingSystemIGSTKService.h"
#include "cxFileHelpers.h"
#include "cxDataLocations.h"
#include "cxConfig.h"
#include "cxTrackerConfigurationImpl.h"

namespace cxtest
{

TEST_CASE("Tool xml files use tracking systems supported by ToolManagerUsingIGSTK", "[unit][tool][xml][org.custus.core.tracking.system.igstk]")
{
	cx::TrackingSystemServicePtr system(new cx::TrackingSystemIGSTKService());
//	cx::TrackingServiceOldPtr trackingService = cx::ToolManagerUsingIGSTK::create();
	cx::TrackerConfigurationPtr config = system->getConfiguration();
	QStringList trackingSystems = config->getSupportedTrackingSystems();

	//Verify tool uses supported tracking system
	foreach(QString filename, config->getAllTools())
	{
		QString toolTrackingSystemName = config->getTool(filename).mTrackingSystemName;

		INFO("Filename: " + filename.toStdString());
		INFO("Tracking system: " + toolTrackingSystemName.toStdString());
		REQUIRE(trackingSystems.contains(toolTrackingSystemName, Qt::CaseInsensitive));
	}
}

TEST_CASE("Tool configuration files", "[unit][tool][xml][org.custus.core.tracking.system.igstk]")
{
	cx::TrackingSystemServicePtr system(new cx::TrackingSystemIGSTKService());
	cx::TrackerConfigurationPtr config = system->getConfiguration();

	QStringList configurations = config->getAllConfigurations();

	foreach(QString filename, configurations)
	{
		INFO("Tool config file: " + filename.toStdString());
		cx::TrackerConfiguration::Configuration configData = config->getConfiguration(filename);
		QStringList selectedTools = configData.mTools;
		foreach(QString toolFileName, selectedTools)
		{
			QFileInfo file(toolFileName);
			INFO("Tool file: " + toolFileName.toStdString());
			CHECK(file.exists());
			if(file.exists())
				REQUIRE(configData.mTrackingSystemName == config->getTool(toolFileName).mTrackingSystemName);
		}
	}
}

TEST_CASE("Verify that saveConfiguration do not loose information", "[unit][tool][xml]")
{
	cx::TrackingSystemServicePtr system(new cx::TrackingSystemIGSTKService());
	cx::TrackerConfigurationPtr config = system->getConfiguration();
	QStringList configurations = config->getAllConfigurations();

	cx::TrackerConfigurationPtr trackerConfig = cx::TrackerConfigurationPtr(new cx::TrackerConfigurationImpl());

	foreach(QString filename, configurations)
	{
		cx::TrackerConfiguration::Configuration configData = config->getConfiguration(filename);

		trackerConfig->saveConfiguration(configData);

		// Check that we don't lose info during save
		cx::TrackerConfiguration::Configuration configData2 = config->getConfiguration(filename);
		CHECK(configData.mUid == configData2.mUid);
		CHECK(configData.mName == configData2.mName);
		CHECK(configData.mClinicalApplication == configData2.mClinicalApplication);
		CHECK(configData.mTrackingSystemName == configData2.mTrackingSystemName);
		CHECK(configData.mReferenceTool == configData2.mReferenceTool);
		CHECK(configData.mTrackingSystemImplementation == configData2.mTrackingSystemImplementation);

		REQUIRE(configData.mToolList.size() == configData2.mToolList.size());
		for(unsigned i = 0; i < configData.mToolList.size(); ++i)
		{
			CHECK(configData.mToolList[i].mOpenIGTLinkImageId == configData2.mToolList[i].mOpenIGTLinkImageId);
			CHECK(configData.mToolList[i].mOpenIGTLinkTransformId == configData2.mToolList[i].mOpenIGTLinkTransformId);
		}
	}
}

TEST_CASE("Tool xml files got existing image files", "[unit][tool][xml]")
{
	cx::TrackingSystemServicePtr system(new cx::TrackingSystemIGSTKService());
//	cx::TrackingServiceOldPtr trackingService = cx::ToolManagerUsingIGSTK::create();
	cx::TrackerConfigurationPtr config = system->getConfiguration();

	foreach(QString filename, config->getAllTools())
	{
		QString imageFileName = config->getTool(filename).mPictureFilename;
		INFO("Tool file: " + filename);
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
	cx::TrackingSystemServicePtr system(new cx::TrackingSystemIGSTKService());
//	cx::TrackingServiceOldPtr trackingService = cx::ToolManagerUsingIGSTK::create();
	cx::TrackerConfigurationPtr config = system->getConfiguration();

	foreach(QString filename, config->getAllTools())
	{
		INFO("Tool file is faulty: " + filename.toStdString());
		REQUIRE(config->verifyTool(filename));
	}
}

} //namespace cxtest
