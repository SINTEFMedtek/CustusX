/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <QTemporaryDir>
#include <QFile>
#include "cxNdiToolSetup.h"
#include "cxtestNdiFakeDevice.h"

namespace cxtest
{

namespace
{

cx::NdiToolConfig wirelessTool(QString uid, QString sromFile, bool reference = false)
{
	cx::NdiToolConfig retval;
	retval.uid = uid;
	retval.wireless = true;
	retval.sromFile = sromFile;
	retval.reference = reference;
	return retval;
}

cx::NdiToolConfig wiredTool(QString uid, int port, int channel = 0, bool fiveDOF = false)
{
	cx::NdiToolConfig retval;
	retval.uid = uid;
	retval.port = port;
	retval.channel = channel;
	retval.fiveDOF = fiveDOF;
	return retval;
}

QString createRomFile(const QTemporaryDir& dir, QString name)
{
	QString filename = dir.path() + "/" + name;
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	file.write("fake rom");
	return filename;
}

} // namespace

TEST_CASE("NdiToolSetup: Polaris wireless tools get a port handle each and their .rom file loaded", "[unit][org.custusx.core.tracking.system.ndi]")
{
	QTemporaryDir dir;
	FakeNdiDevice device;
	std::vector<cx::NdiToolConfig> tools = {
		wirelessTool("pointer", createRomFile(dir, "pointer.rom")),
		wirelessTool("reference", createRomFile(dir, "reference.rom"), true)};

	cx::NdiToolSetupResult result = cx::setupNdiTools(device, tools);

	CHECK(result.success());
	CHECK(result.warnings.empty());
	REQUIRE(result.portHandleToUid.size() == 2);
	for (const std::pair<const int, QString>& entry : result.portHandleToUid)
	{
		const FakePortHandle& handle = device.handles[entry.first];
		CHECK(handle.toolDefinitionLoaded);
		CHECK(handle.initialized);
		CHECK(handle.enabled);
		CHECK(handle.enabledAsStatic == (entry.second == "reference"));
	}
}

TEST_CASE("NdiToolSetup: port handles waiting to be freed are freed first", "[unit][org.custusx.core.tracking.system.ndi]")
{
	QTemporaryDir dir;
	FakeNdiDevice device;
	device.toBeFreed = {0x05, 0x06};

	cx::setupNdiTools(device, {wirelessTool("pointer", createRomFile(dir, "pointer.rom"))});

	CHECK(device.freed.count(0x05));
	CHECK(device.freed.count(0x06));
}

TEST_CASE("NdiToolSetup: missing or failing .rom files give a warning, other tools are still set up", "[unit][org.custusx.core.tracking.system.ndi]")
{
	QTemporaryDir dir;
	FakeNdiDevice device;
	QString badRom = createRomFile(dir, "bad.rom");
	device.failToolDefinition.insert(badRom);
	std::vector<cx::NdiToolConfig> tools = {
		wirelessTool("missing", dir.path() + "/does_not_exist.rom"),
		wirelessTool("bad", badRom),
		wirelessTool("good", createRomFile(dir, "good.rom"))};

	cx::NdiToolSetupResult result = cx::setupNdiTools(device, tools);

	CHECK(result.success());
	CHECK(result.warnings.size() == 2);
	REQUIRE(result.portHandleToUid.size() == 1);
	CHECK(result.portHandleToUid.begin()->second == "good");
	CHECK(device.handles.size() == 1); // the handle for the failing .rom file is freed again
}

TEST_CASE("NdiToolSetup: no port handle for a wireless tool gives a warning", "[unit][org.custusx.core.tracking.system.ndi]")
{
	QTemporaryDir dir;
	FakeNdiDevice device;
	device.failWirelessRequest = true;

	cx::NdiToolSetupResult result = cx::setupNdiTools(device, {wirelessTool("pointer", createRomFile(dir, "pointer.rom"))});

	CHECK_FALSE(result.success());
	CHECK(result.warnings.size() == 1);
}

TEST_CASE("NdiToolSetup: Aurora wired tools are matched by physical port, not by port handle order", "[unit][org.custusx.core.tracking.system.ndi]")
{
	FakeNdiDevice device;
	device.addWiredHandle(0x0A, 2); // handle numbers deliberately in a different order than the ports
	device.addWiredHandle(0x0B, 1);
	device.addWiredHandle(0x0C, 0);
	std::vector<cx::NdiToolConfig> tools = {wiredTool("port0", 0), wiredTool("port2", 2)};

	cx::NdiToolSetupResult result = cx::setupNdiTools(device, tools);

	CHECK(result.success());
	CHECK(result.warnings.empty());
	REQUIRE(result.portHandleToUid.size() == 2);
	CHECK(result.portHandleToUid[0x0C] == "port0");
	CHECK(result.portHandleToUid[0x0A] == "port2");
	CHECK(result.messages.size() == 1); // port 1 has a sensor, but no configured tool
}

TEST_CASE("NdiToolSetup: an Aurora wired reference is enabled as a static tool", "[unit][org.custusx.core.tracking.system.ndi]")
{
	FakeNdiDevice device;
	device.addWiredHandle(0x0A, 0);
	device.addWiredHandle(0x0B, 1);
	cx::NdiToolConfig reference = wiredTool("reference", 0);
	reference.reference = true;
	std::vector<cx::NdiToolConfig> tools = {reference, wiredTool("pointer", 1)};

	cx::NdiToolSetupResult result = cx::setupNdiTools(device, tools);

	CHECK(result.success());
	CHECK(device.handles[0x0A].enabledAsStatic);
	CHECK_FALSE(device.handles[0x0B].enabledAsStatic);
}

TEST_CASE("NdiToolSetup: Aurora 5DOF sensors on a splitter are matched by port and channel", "[unit][org.custusx.core.tracking.system.ndi]")
{
	FakeNdiDevice device;
	device.addWiredHandle(0x0B, 1, 0);
	device.addWiredHandle(0x0D, 1, 1, 0x0B); // the second sensor only appears after the first is enabled
	std::vector<cx::NdiToolConfig> tools = {wiredTool("channel1", 1, 1, true), wiredTool("channel0", 1, 0, true)};

	cx::NdiToolSetupResult result = cx::setupNdiTools(device, tools);

	CHECK(result.warnings.empty());
	REQUIRE(result.portHandleToUid.size() == 2);
	CHECK(result.portHandleToUid[0x0B] == "channel0");
	CHECK(result.portHandleToUid[0x0D] == "channel1");
	CHECK(device.handles[0x0D].initialized);
	CHECK(device.handles[0x0D].enabled);
}

TEST_CASE("NdiToolSetup: a 6DOF tool is not matched to the second channel of a splitter", "[unit][org.custusx.core.tracking.system.ndi]")
{
	FakeNdiDevice device;
	device.addWiredHandle(0x0D, 1, 1);

	cx::NdiToolSetupResult result = cx::setupNdiTools(device, {wiredTool("sixDOF", 1)});

	CHECK_FALSE(result.success());
	CHECK(result.warnings.size() == 1);
}

TEST_CASE("NdiToolSetup: a wired tool whose port is empty uses a leftover sensor, as the previous NDI plugin did", "[unit][org.custusx.core.tracking.system.ndi]")
{
	// The previous NDI plugin matched wired tools by sorted order, so a tool file with
	// port 0 also worked with the tool plugged into another port. Keep that working.
	FakeNdiDevice device;
	device.addWiredHandle(0x0B, 1);

	cx::NdiToolSetupResult result = cx::setupNdiTools(device, {wiredTool("pointer", 0)});

	CHECK(result.success());
	REQUIRE(result.portHandleToUid.size() == 1);
	CHECK(result.portHandleToUid[0x0B] == "pointer");
	REQUIRE(result.warnings.size() == 1);
	CHECK(result.warnings[0].contains("port 1"));
}

TEST_CASE("NdiToolSetup: tools with the wrong ports get the leftover sensors in port order", "[unit][org.custusx.core.tracking.system.ndi]")
{
	FakeNdiDevice device;
	device.addWiredHandle(0x0C, 3);
	device.addWiredHandle(0x0D, 2);
	std::vector<cx::NdiToolConfig> tools = {wiredTool("second", 1), wiredTool("first", 0)};

	cx::NdiToolSetupResult result = cx::setupNdiTools(device, tools);

	REQUIRE(result.portHandleToUid.size() == 2);
	CHECK(result.portHandleToUid[0x0D] == "first");  // port 0 -> lowest free port (2)
	CHECK(result.portHandleToUid[0x0C] == "second"); // port 1 -> next free port (3)
	CHECK(result.warnings.size() == 2);
}

TEST_CASE("NdiToolSetup: a sensor on the configured port is preferred over the fallback", "[unit][org.custusx.core.tracking.system.ndi]")
{
	FakeNdiDevice device;
	device.addWiredHandle(0x0A, 0);
	device.addWiredHandle(0x0C, 3);
	std::vector<cx::NdiToolConfig> tools = {wiredTool("onPort0", 0), wiredTool("onPort1", 1)};

	cx::NdiToolSetupResult result = cx::setupNdiTools(device, tools);

	REQUIRE(result.portHandleToUid.size() == 2);
	CHECK(result.portHandleToUid[0x0A] == "onPort0");
	CHECK(result.portHandleToUid[0x0C] == "onPort1");
	CHECK(result.warnings.size() == 1);
}

TEST_CASE("NdiToolSetup: a wired tool that is not plugged in gives a warning", "[unit][org.custusx.core.tracking.system.ndi]")
{
	FakeNdiDevice device;
	device.addWiredHandle(0x0A, 0);
	std::vector<cx::NdiToolConfig> tools = {wiredTool("present", 0), wiredTool("unplugged", 3)};

	cx::NdiToolSetupResult result = cx::setupNdiTools(device, tools);

	CHECK(result.success());
	REQUIRE(result.warnings.size() == 1);
	CHECK(result.warnings[0].contains("unplugged"));
}

TEST_CASE("NdiToolSetup: a port handle that fails to initialize is not enabled or used", "[unit][org.custusx.core.tracking.system.ndi]")
{
	FakeNdiDevice device;
	device.addWiredHandle(0x0A, 0);
	device.addWiredHandle(0x0B, 1);
	device.failInitialize.insert(0x0B);
	std::vector<cx::NdiToolConfig> tools = {wiredTool("good", 0), wiredTool("bad", 1)};

	cx::NdiToolSetupResult result = cx::setupNdiTools(device, tools);

	REQUIRE(result.portHandleToUid.size() == 1);
	CHECK(result.portHandleToUid[0x0A] == "good");
	CHECK_FALSE(device.handles[0x0B].enabled);
	CHECK(result.warnings.size() == 2); // initialization failed, and tool "bad" not found
}

TEST_CASE("NdiToolSetup: no configured tools found gives no success", "[unit][org.custusx.core.tracking.system.ndi]")
{
	FakeNdiDevice device;

	cx::NdiToolSetupResult result = cx::setupNdiTools(device, {wiredTool("pointer", 0)});

	CHECK_FALSE(result.success());
}

} // namespace cxtest
