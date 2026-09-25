/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxNdiToolSetup.h"

#include <set>
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

void matchWiredTools(NdiDevice& device, const std::vector<NdiToolConfig>& tools, NdiToolSetupResult& result)
{
	std::set<QString> assigned;
	for (const std::pair<const int, QString>& entry : result.portHandleToUid)
	{
		assigned.insert(entry.second);
	}

	for (int portHandle : device.searchPortHandles(NdiPortHandleSearch::Enabled))
	{
		if (result.portHandleToUid.count(portHandle))
		{
			continue;
		}
		NdiPortLocation location = device.getPortLocation(portHandle);
		QString uid;
		for (const NdiToolConfig& tool : tools)
		{
			if (uid.isEmpty() && !assigned.count(tool.uid) && matchesWiredTool(tool, location))
			{
				uid = tool.uid;
			}
		}
		if (uid.isEmpty())
		{
			result.messages << QString("Port handle %1 (port %2, channel %3) has no configured tool.")
							   .arg(handleText(portHandle)).arg(location.port).arg(location.channel);
		}
		else
		{
			result.portHandleToUid[portHandle] = uid;
			assigned.insert(uid);
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
