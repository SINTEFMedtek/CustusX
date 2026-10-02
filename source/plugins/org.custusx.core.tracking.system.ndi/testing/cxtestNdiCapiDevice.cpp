/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxNdiCapiDevice.h"
#include "cxNdiTool.h"
#include "cxToolUsingNDI.h"

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

} // namespace cxtest
