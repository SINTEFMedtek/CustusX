/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxNdiToolSetup.h"

#include <algorithm>
#include <set>
#include <tuple>
#include <QFileInfo>

namespace cx
{

namespace
{
const int kMaxInitializeRounds = 5;

QString handleText(int portHandle)
{
	return QString("%1").arg(portHandle, 2, 16, QChar('0')).toUpper();
}

bool isReferenceHandle(int portHandle, const std::map<int, QString>& handleToUid, const std::vector<NdiToolConfig>& tools)
{
	bool retval = false;
	std::map<int, QString>::const_iterator it = handleToUid.find(portHandle);
	if (it != handleToUid.end())
	{
		for (const NdiToolConfig& tool : tools)
		{
			retval = retval || (tool.uid == it->second && tool.reference);
		}
	}
	return retval;
}

void setupWirelessTools(NdiDevice& device, const std::vector<NdiToolConfig>& tools, NdiToolSetupResult& result)
{
	for (const NdiToolConfig& tool : tools)
	{
		if (!tool.wireless)
		{
			continue;
		}
		if (tool.sromFile.isEmpty() || !QFileInfo(tool.sromFile).isFile())
		{
			result.warnings << QString("Tool %1: tool definition (.rom) file not found: '%2'").arg(tool.uid).arg(tool.sromFile);
			continue;
		}
		int portHandle = device.requestWirelessPortHandle();
		if (portHandle < 0)
		{
			result.warnings << QString("Tool %1: could not get a port handle: %2").arg(tool.uid).arg(device.getLastError());
		}
		else if (!device.loadToolDefinition(portHandle, tool.sromFile))
		{
			result.warnings << QString("Tool %1: could not load %2: %3").arg(tool.uid).arg(tool.sromFile).arg(device.getLastError());
			device.freePortHandle(portHandle);
		}
		else
		{
			result.portHandleToUid[portHandle] = tool.uid;
		}
	}
}

/** Initialize and enable until no new port handles appear (splitters, see setupNdiTools). */
void initializeAndEnablePortHandles(NdiDevice& device, const std::vector<NdiToolConfig>& tools, NdiToolSetupResult& result)
{
	std::set<int> failed;
	bool newHandles = true;
	for (int round = 0; round < kMaxInitializeRounds && newHandles; ++round)
	{
		newHandles = false;
		for (int portHandle : device.searchPortHandles(NdiPortHandleSearch::NotInitialized))
		{
			if (failed.count(portHandle))
			{
				continue;
			}
			newHandles = true;
			if (!device.initializePortHandle(portHandle))
			{
				failed.insert(portHandle);
				result.warnings << QString("Port handle %1: initialization failed: %2").arg(handleText(portHandle)).arg(device.getLastError());
			}
		}
		for (int portHandle : device.searchPortHandles(NdiPortHandleSearch::NotEnabled))
		{
			if (failed.count(portHandle))
			{
				continue;
			}
			newHandles = true;
			bool staticTool = isReferenceHandle(portHandle, result.portHandleToUid, tools);
			if (!device.enablePortHandle(portHandle, staticTool))
			{
				failed.insert(portHandle);
				result.warnings << QString("Port handle %1: enabling failed: %2").arg(handleText(portHandle)).arg(device.getLastError());
			}
		}
	}
}

bool matchesWiredTool(const NdiToolConfig& tool, const NdiPortLocation& location)
{
	bool channelMatches = tool.fiveDOF ? (tool.channel == location.channel) : (location.channel == 0);
	return !tool.wireless && location.valid && !location.wireless && tool.port == location.port && channelMatches;
}

/** A 6DOF tool can't be the second sensor on a splitter. */
bool canUseSensor(const NdiToolConfig& tool, const NdiPortLocation& location)
{
	return !tool.wireless && location.valid && !location.wireless && (tool.fiveDOF || location.channel == 0);
}

struct WiredSensor
{
	int portHandle;
	NdiPortLocation location;
};

bool byPortAndChannel(const WiredSensor& a, const WiredSensor& b)
{
	return std::make_tuple(a.location.port, a.location.channel, a.portHandle) < std::make_tuple(b.location.port, b.location.channel, b.portHandle);
}

/** Fallback for tools whose configured port has no sensor: give them the leftover
 *  sensors in port order. This is how the previous NDI plugin matched all wired
 *  tools (by sorted order), so tool files with port numbers that don't match the
 *  physical ports keep working.
 */
void assignLeftoverSensors(std::vector<WiredSensor>& sensors, const std::vector<NdiToolConfig>& tools,
						   std::set<QString>& assigned, NdiToolSetupResult& result)
{
	std::vector<NdiToolConfig> leftoverTools;
	for (const NdiToolConfig& tool : tools)
	{
		if (!tool.wireless && !assigned.count(tool.uid))
		{
			leftoverTools.push_back(tool);
		}
	}
	std::sort(leftoverTools.begin(), leftoverTools.end(), [](const NdiToolConfig& a, const NdiToolConfig& b)
	{
		return std::make_tuple(a.port, a.channel) < std::make_tuple(b.port, b.channel);
	});
	std::sort(sensors.begin(), sensors.end(), byPortAndChannel);

	for (const NdiToolConfig& tool : leftoverTools)
	{
		std::vector<WiredSensor>::iterator sensor = std::find_if(sensors.begin(), sensors.end(),
			[&tool, &result](const WiredSensor& s) { return !result.portHandleToUid.count(s.portHandle) && canUseSensor(tool, s.location); });
		if (sensor != sensors.end())
		{
			result.portHandleToUid[sensor->portHandle] = tool.uid;
			assigned.insert(tool.uid);
			result.warnings << QString("Tool %1: no sensor on its configured port %2, using the sensor on port %3%4 instead. "
									   "Update the port number in the tool file if this is not the right tool.")
							   .arg(tool.uid).arg(tool.port).arg(sensor->location.port)
							   .arg(sensor->location.channel ? QString(", channel %1").arg(sensor->location.channel) : QString());
		}
	}
}

void matchWiredTools(NdiDevice& device, const std::vector<NdiToolConfig>& tools, NdiToolSetupResult& result)
{
	std::set<QString> assigned;
	for (const std::pair<const int, QString>& entry : result.portHandleToUid)
	{
		assigned.insert(entry.second);
	}

	// Sensors on the port (and channel) given in the tool file
	std::vector<WiredSensor> sensors;
	for (int portHandle : device.searchPortHandles(NdiPortHandleSearch::Enabled))
	{
		if (result.portHandleToUid.count(portHandle))
		{
			continue;
		}
		WiredSensor sensor = {portHandle, device.getPortLocation(portHandle)};
		sensors.push_back(sensor);
		for (const NdiToolConfig& tool : tools)
		{
			if (!result.portHandleToUid.count(portHandle) && !assigned.count(tool.uid) && matchesWiredTool(tool, sensor.location))
			{
				result.portHandleToUid[portHandle] = tool.uid;
				assigned.insert(tool.uid);
			}
		}
	}

	assignLeftoverSensors(sensors, tools, assigned, result);

	for (const WiredSensor& sensor : sensors)
	{
		if (!result.portHandleToUid.count(sensor.portHandle))
		{
			result.messages << QString("Port handle %1 (port %2, channel %3) has no configured tool.")
							   .arg(handleText(sensor.portHandle)).arg(sensor.location.port).arg(sensor.location.channel);
		}
	}
	for (const NdiToolConfig& tool : tools)
	{
		if (!tool.wireless && !assigned.count(tool.uid))
		{
			result.warnings << QString("Tool %1: no sensor found on port %2%3. Is the tool plugged in?")
							   .arg(tool.uid).arg(tool.port).arg(tool.fiveDOF ? QString(", channel %1").arg(tool.channel) : QString());
		}
	}
}

} // namespace

NdiToolSetupResult setupNdiTools(NdiDevice& device, const std::vector<NdiToolConfig>& tools)
{
	NdiToolSetupResult result;
	for (int portHandle : device.searchPortHandles(NdiPortHandleSearch::ToBeFreed))
	{
		device.freePortHandle(portHandle);
	}
	setupWirelessTools(device, tools, result);
	initializeAndEnablePortHandles(device, tools, result);
	matchWiredTools(device, tools, result);
	return result;
}

} // namespace cx
