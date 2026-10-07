/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QFile>
#include <QTemporaryDir>
#include "cxDataLocations.h"

#include "cxNdiCapiDevice.h"
#include "ndicapi.h"
#include "cxNdiTool.h"
#include "cxToolUsingNDI.h"
#include "cxTrackingSystemNDIService.h"

namespace cxtest
{

TEST_CASE("NdiCapiDevice: port location of a wired Aurora sensor", "[unit][org.custusx.core.tracking.system.ndi]")
{
	// 8 chars device number, Aurora (1), wired (0), port 03, channel 01
	cx::NdiPortLocation location = cx::NdiCapiDevice::parsePortLocation("12345678100301");
	CHECK(location.valid);
	CHECK_FALSE(location.wireless);
	CHECK(location.port == 2); // 0-based, as in the tool files
	CHECK(location.channel == 1);
}

TEST_CASE("NdiCapiDevice: port location of a wireless Polaris tool", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiPortLocation location = cx::NdiCapiDevice::parsePortLocation("00000000010100");
	CHECK(location.valid);
	CHECK(location.wireless);
	CHECK(location.channel == 0);
}

TEST_CASE("NdiCapiDevice: invalid port location", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiPortLocation location = cx::NdiCapiDevice::parsePortLocation("xxxxxxxxxxxxxx");
	CHECK_FALSE(location.valid);
}

TEST_CASE("NdiCapiDevice: baud rates", "[unit][org.custusx.core.tracking.system.ndi]")
{
	CHECK(cx::NdiCapiDevice::baudRateToNdiCode(9600) == 0);
	CHECK(cx::NdiCapiDevice::baudRateToNdiCode(115200) == 5);
	CHECK(cx::NdiCapiDevice::baudRateToNdiCode(921600) == 6);
	CHECK(cx::NdiCapiDevice::baudRateToNdiCode(230400) == 10);
	CHECK(cx::NdiCapiDevice::baudRateToNdiCode(12345) == -1);
}

TEST_CASE("NdiCapiDevice: illuminator rates", "[unit][org.custusx.core.tracking.system.ndi]")
{
	CHECK(cx::NdiCapiDevice::illuminatorRateToNdiCode(20) == 0);
	CHECK(cx::NdiCapiDevice::illuminatorRateToNdiCode(30) == 1);
	CHECK(cx::NdiCapiDevice::illuminatorRateToNdiCode(60) == 2);
	CHECK(cx::NdiCapiDevice::illuminatorRateToNdiCode(40) == -1);
}

TEST_CASE("NdiCapiDevice: baud rates to try, fastest first", "[unit][org.custusx.core.tracking.system.ndi]")
{
	CHECK(cx::NdiCapiDevice::getBaudRatesToTry(921600) == std::vector<int>({921600, 115200, 57600, 38400, 19200, 9600}));
	CHECK(cx::NdiCapiDevice::getBaudRatesToTry(115200) == std::vector<int>({115200, 57600, 38400, 19200, 9600}));
	CHECK(cx::NdiCapiDevice::getBaudRatesToTry(9600) == std::vector<int>({9600}));
	CHECK(cx::NdiCapiDevice::getBaudRatesToTry(12345) == std::vector<int>({115200, 57600, 38400, 19200, 9600}));
}

TEST_CASE("NdiCapiDevice: timeouts and I/O errors mean the device is lost", "[unit][org.custusx.core.tracking.system.ndi]")
{
	CHECK(cx::NdiCapiDevice::isLostDeviceError(NDI_TIMEOUT));
	CHECK(cx::NdiCapiDevice::isLostDeviceError(NDI_WRITE_ERROR));
	CHECK(cx::NdiCapiDevice::isLostDeviceError(NDI_READ_ERROR));
	CHECK_FALSE(cx::NdiCapiDevice::isLostDeviceError(NDI_OKAY));
	CHECK_FALSE(cx::NdiCapiDevice::isLostDeviceError(NDI_COMM_FAIL));
}

TEST_CASE("NdiCapiDevice: a slower baud rate is tried when this computer or the device can't use the rate", "[unit][org.custusx.core.tracking.system.ndi]")
{
	CHECK(cx::NdiCapiDevice::isSlowerBaudRateWorthTrying(NDI_COMM_FAIL));
	CHECK(cx::NdiCapiDevice::isSlowerBaudRateWorthTrying(NDI_BAD_COMM));
	CHECK_FALSE(cx::NdiCapiDevice::isSlowerBaudRateWorthTrying(NDI_TIMEOUT));
}

TEST_CASE("NdiCapiDevice: opening a device that isn't there fails cleanly", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiCapiDevice device;
	cx::NdiConnectionSettings settings;
	settings.hostname = "/dev/this_ndi_device_does_not_exist";

	CHECK_FALSE(device.open(settings));
	CHECK_FALSE(device.isOpen());
	CHECK_FALSE(device.getLastError().isEmpty());
	CHECK_FALSE(device.startTracking());
	std::vector<int> handles = device.searchPortHandles(cx::NdiPortHandleSearch::All);
	CHECK(handles.empty());
}

TEST_CASE("NdiCapiDevice: a network device that refuses the connection fails cleanly", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiCapiDevice device;
	cx::NdiConnectionSettings settings;
	settings.hostname = "127.0.0.1";
	settings.networkPort = 1; // nothing listens here

	CHECK_FALSE(device.open(settings));
	CHECK_FALSE(device.isOpen());
}

TEST_CASE("NdiCapiDevice: count the used markers in the BX marker information", "[unit][org.custusx.core.tracking.system.ndi]")
{
	char none[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	CHECK(cx::NdiCapiDevice::countUsedMarkers(none) == 0);

	char fourUsed[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0x33, 0x33};
	CHECK(cx::NdiCapiDevice::countUsedMarkers(fourUsed) == 4);

	char threeUsedOneMissing[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0x03, 0x33};
	CHECK(cx::NdiCapiDevice::countUsedMarkers(threeUsedOneMissing) == 3);

	char exceeded[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0x12, 0x33};
	CHECK(cx::NdiCapiDevice::countUsedMarkers(exceeded) == 2);

	char allTwenty[10] = {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33};
	CHECK(cx::NdiCapiDevice::countUsedMarkers(allTwenty) == 20);
}

TEST_CASE("ToolUsingNDI: forwards the position warning", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::ToolFileParser::ToolInternalStructurePtr structure(new cx::ToolFileParser::ToolInternalStructure());
	structure->mUid = "pointer";
	structure->mIsPointer = true;
	cx::NdiToolPtr ndiTool(new cx::NdiTool(structure));
	cx::ToolUsingNDI tool(ndiTool);
	int signalCount = 0;
	QObject::connect(&tool, &cx::Tool::toolPositionWarning, [&signalCount](QString) { signalCount++; });

	ndiTool->setPositionWarning("out of the measurement volume");
	CHECK(tool.getPositionWarning() == "out of the measurement volume");

	ndiTool->setPositionWarning("");
	CHECK(tool.getPositionWarning().isEmpty());
	CHECK(signalCount == 2);
}

TEST_CASE("ToolUsingNDI: the calibration is applied to the positions", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::ToolFileParser::ToolInternalStructurePtr structure(new cx::ToolFileParser::ToolInternalStructure());
	structure->mUid = "pointer";
	structure->mIsPointer = true;
	structure->mCalibration = cx::createTransformTranslate(cx::Vector3D(0, 0, 100));
	cx::NdiToolPtr ndiTool(new cx::NdiTool(structure));
	cx::ToolUsingNDI tool(ndiTool);
	cx::Transform3D prMs = cx::createTransformTranslate(cx::Vector3D(10, 20, 30)) * cx::createTransformRotateX(M_PI / 2);

	ndiTool->updateTransform(prMs, 1000.0, true);

	CHECK(tool.isCalibrated());
	CHECK(cx::similar(tool.get_prMt(), prMs * structure->mCalibration));
	CHECK(cx::similar(tool.get_prMt().coord(cx::Vector3D(0, 0, 0)), cx::Vector3D(10, -80, 30)));
	CHECK(tool.getTimestamp() == Approx(1000.0));
}

TEST_CASE("ToolUsingNDI: tracking rate from the position history", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::TimedTransformMap history;
	CHECK(cx::ToolUsingNDI::calculateTrackingRate(history) == 0);

	history[1000] = cx::Transform3D::Identity();
	CHECK(cx::ToolUsingNDI::calculateTrackingRate(history) == 0);

	// 40 Hz (every 25 ms), fewer positions than the 10 used for the estimate
	for (int i = 1; i < 5; ++i)
	{
		history[1000 + 25 * i] = cx::Transform3D::Identity();
	}
	CHECK(cx::ToolUsingNDI::calculateTrackingRate(history) == 40);

	// more than 10 positions: only the last 10 are used (here at 20 Hz)
	for (int i = 0; i < 10; ++i)
	{
		history[2000 + 50 * i] = cx::Transform3D::Identity();
	}
	CHECK(cx::ToolUsingNDI::calculateTrackingRate(history) == 20);
}

TEST_CASE("TrackingSystemNDIService: handles ndi configurations, and igstk ones unless the IGSTK plugin is built", "[unit][org.custusx.core.tracking.system.ndi]")
{
	CHECK(cx::TrackingSystemNDIService::handlesImplementation("ndi", false));
	CHECK(cx::TrackingSystemNDIService::handlesImplementation("ndi", true));
	CHECK(cx::TrackingSystemNDIService::handlesImplementation("igstk", false));
	CHECK_FALSE(cx::TrackingSystemNDIService::handlesImplementation("igstk", true));
	CHECK_FALSE(cx::TrackingSystemNDIService::handlesImplementation("openigtlink", false));
}

namespace
{

/** An NDI Aurora tool configuration in folder, with the tools on the given ports (1-4). */
QString writeAuroraConfiguration(QString folder, QString name, QStringList ports)
{
	QString root = "Shared/";
	QStringList toolFiles = {"Reference_AURORA_port1/Reference_AURORA_port1.xml", "Pointer_AURORA_port2/Pointer_AURORA_port2.xml",
							 "Sensor_AURORA_port3/Sensor_AURORA_port3.xml", "Sensor_AURORA_port4/Sensor_AURORA_port4.xml"};
	QString tools;
	for (const QString& port : ports)
	{
		tools += QString("<toolfile reference=\"%1\">%2</toolfile>").arg(port == "1" ? "yes" : "no").arg(root + toolFiles[port.toInt() - 1]);
	}
	QString filename = folder + "/" + name;
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	file.write(QString("<?xml version=\"1.0\"?><configuration clinical_app=\"Laboratory\"><trackingsystemimplementation type=\"ndi\">"
					   "<tracker type=\"aurora\">%1</tracker></trackingsystemimplementation></configuration>").arg(tools).toUtf8());
	return filename;
}

/** Configuring finishes when the tracker thread's signal is processed. */
bool waitUntilConfigured(cx::TrackingSystemNDIService& service)
{
	QElapsedTimer timer;
	timer.start();
	while (service.getState() < cx::Tool::tsCONFIGURED && timer.elapsed() < 5000)
	{
		QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
	}
	return service.getState() >= cx::Tool::tsCONFIGURED;
}

} // namespace

TEST_CASE("TrackingSystemNDIService: a new tool configuration replaces the configured one", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::DataLocations::setTestMode();
	QTemporaryDir folder;
	QString fourTools = writeAuroraConfiguration(folder.path(), "four.xml", {"1", "2", "3", "4"});
	QString twoTools = writeAuroraConfiguration(folder.path(), "two.xml", {"1", "2"});

	cx::TrackingSystemNDIService service;
	service.setConfigurationFile(fourTools);
	service.setState(cx::Tool::tsCONFIGURED);
	REQUIRE(waitUntilConfigured(service));
	size_t configuredTools = service.getTools().size();

	service.setConfigurationFile(twoTools);
	REQUIRE(waitUntilConfigured(service));
	CHECK(service.getTools().size() == configuredTools - 2);
}

} // namespace cxtest
