/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTNDIFAKEDEVICE_H_
#define CXTESTNDIFAKEDEVICE_H_

#include <map>
#include <set>
#include <QThread>
#include "cxNdiDevice.h"

namespace cxtest
{

/** A port handle on the fake device. */
struct FakePortHandle
{
	bool wireless = false;
	int port = -1;
	int channel = 0;
	bool present = true;       ///< false: appears after appearsAfterEnabling is enabled (splitter)
	int appearsAfterEnabling = -1;
	bool initialized = false;
	bool enabled = false;
	bool enabledAsStatic = false;
	bool toolDefinitionLoaded = false;
};

/** \brief NdiDevice without hardware, for testing the tool setup and tracker manager.
 *
 * Behaves like an NDI device in the ways the plugin relies on: port handles
 * with physical port/channel, initialize and enable, handles to be freed, and
 * frames set by the test.
 */
class FakeNdiDevice : public cx::NdiDevice
{
public:
	bool open(const cx::NdiConnectionSettings& settings) override
	{
		lastConnection = settings;
		opened = openSucceeds;
		return opened;
	}
	void close() override { opened = false; closeCount++; }
	bool isOpen() const override { return opened; }
	QString getLastError() const override { return "fake error"; }
	QString getDescription() const override { return "fake NDI device"; }
	bool isInUseByAnotherApplication() const override { return inUse; }
	bool isStopRequested() const override { return stopRequested; }

	std::vector<int> searchPortHandles(cx::NdiPortHandleSearch search) override
	{
		std::vector<int> retval;
		if (search == cx::NdiPortHandleSearch::ToBeFreed)
		{
			retval.assign(toBeFreed.begin(), toBeFreed.end());
		}
		for (const std::pair<const int, FakePortHandle>& entry : handles)
		{
			const FakePortHandle& h = entry.second;
			bool include = false;
			switch (search)
			{
			case cx::NdiPortHandleSearch::All: include = h.present; break;
			case cx::NdiPortHandleSearch::ToBeFreed: include = false; break;
			case cx::NdiPortHandleSearch::NotInitialized: include = h.present && !h.initialized; break;
			case cx::NdiPortHandleSearch::NotEnabled: include = h.present && h.initialized && !h.enabled; break;
			case cx::NdiPortHandleSearch::Enabled: include = h.present && h.enabled; break;
			}
			if (include)
			{
				retval.push_back(entry.first);
			}
		}
		return retval;
	}
	bool freePortHandle(int portHandle) override
	{
		freed.insert(portHandle);
		toBeFreed.erase(portHandle);
		handles.erase(portHandle);
		return true;
	}
	int requestWirelessPortHandle() override
	{
		int retval = -1;
		if (!failWirelessRequest)
		{
			retval = nextWirelessHandle++;
			FakePortHandle h;
			h.wireless = true;
			handles[retval] = h;
		}
		return retval;
	}
	bool loadToolDefinition(int portHandle, QString sromFile) override
	{
		loadedToolDefinitions[portHandle] = sromFile;
		bool success = !failToolDefinition.count(sromFile);
		handles[portHandle].toolDefinitionLoaded = success;
		return success;
	}
	bool initializePortHandle(int portHandle) override
	{
		bool success = !failInitialize.count(portHandle);
		handles[portHandle].initialized = success;
		return success;
	}
	bool enablePortHandle(int portHandle, bool staticTool) override
	{
		handles[portHandle].enabled = true;
		handles[portHandle].enabledAsStatic = staticTool;
		for (std::pair<const int, FakePortHandle>& entry : handles)
		{
			if (entry.second.appearsAfterEnabling == portHandle)
			{
				entry.second.present = true;
			}
		}
		return true;
	}
	cx::NdiPortLocation getPortLocation(int portHandle) override
	{
		cx::NdiPortLocation retval;
		const FakePortHandle& h = handles[portHandle];
		retval.valid = true;
		retval.wireless = h.wireless;
		retval.port = h.port;
		retval.channel = h.channel;
		return retval;
	}
	bool setIlluminatorRate(int rate_Hz) override { illuminatorRate_Hz = rate_Hz; return illuminatorRateSucceeds; }
	bool startTracking() override { tracking = startTrackingSucceeds; return tracking; }
	bool stopTracking() override { tracking = false; stopCount++; return true; }
	bool readFrame(const std::vector<int>& portHandles, cx::NdiFrame& frame) override
	{
		requestedHandles = portHandles;
		if (readDelay_ms > 0)
		{
			QThread::msleep(readDelay_ms); // a device whose reply waits for its next frame
		}
		bool success = (failReads <= 0);
		if (success)
		{
			frame = nextFrame;
		}
		else
		{
			failReads--;
		}
		return success;
	}

	/** Add a wired sensor, e.g. an Aurora tool on port (0-based) and channel. */
	void addWiredHandle(int portHandle, int port, int channel = 0, int appearsAfterEnabling = -1)
	{
		FakePortHandle h;
		h.port = port;
		h.channel = channel;
		h.appearsAfterEnabling = appearsAfterEnabling;
		h.present = (appearsAfterEnabling < 0);
		handles[portHandle] = h;
	}

	/** Set the next frame: frame number and one sample per (handle, visible, transform). */
	void setNextFrame(unsigned long frameNumber, std::vector<cx::NdiToolSample> samples)
	{
		nextFrame.frameNumber = frameNumber;
		nextFrame.samples = samples;
	}

	std::map<int, FakePortHandle> handles;
	std::set<int> toBeFreed;
	std::set<int> freed;
	std::map<int, QString> loadedToolDefinitions;
	std::set<QString> failToolDefinition;
	std::set<int> failInitialize;
	bool failWirelessRequest = false;
	int nextWirelessHandle = 1;
	bool openSucceeds = true;
	bool inUse = false; ///< open() fails because another application uses the device
	bool stopRequested = false;
	bool startTrackingSucceeds = true;
	bool opened = false;
	bool tracking = false;
	int closeCount = 0;
	int stopCount = 0;
	int failReads = 0;
	cx::NdiFrame nextFrame;
	int readDelay_ms = 0;
	int illuminatorRate_Hz = 0;
	bool illuminatorRateSucceeds = true;
	std::vector<int> requestedHandles;
	cx::NdiConnectionSettings lastConnection;
};

inline cx::NdiToolSample createSample(int portHandle, bool visible, cx::Transform3D transform = cx::Transform3D::Identity())
{
	cx::NdiToolSample retval;
	retval.portHandle = portHandle;
	retval.visible = visible;
	retval.transform = transform;
	return retval;
}

} // namespace cxtest

#endif /* CXTESTNDIFAKEDEVICE_H_ */
