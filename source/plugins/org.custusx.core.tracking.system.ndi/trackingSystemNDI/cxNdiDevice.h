/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXNDIDEVICE_H_
#define CXNDIDEVICE_H_

#include "org_custusx_core_tracking_system_ndi_Export.h"

#include <vector>
#include <memory>
#include <QString>
#include "cxTransform3D.h"

namespace cx
{

/** How to connect to an NDI device. */
struct org_custusx_core_tracking_system_ndi_EXPORT NdiConnectionSettings
{
	QString hostname; ///< Serial device (e.g. /dev/ttyUSB0, COM3) or network host/IP. Empty: probe the serial ports.
	int networkPort = 8765; ///< Port for network connections (NDI default)
	bool takeOverDevice = false; ///< Network: also connect to a device that is already tracking, stopping the other application's session
	int baudRate = 921600; ///< Serial baud rate. Falls back to slower rates if the device or the serial adapter can't use it.
	QString communicationLogFile; ///< If not empty, log the commands and replies to this file.
	bool requestMarkerInfo = false; ///< Request tool and marker information in BX (Polaris only). Set by NdiTrackerManager.
	bool debugMarkerCount = false; ///< Also request the 3D marker positions in BX, and log the marker count of each tool
	double maxErrorRMS_mm = 0.0; ///< Warn about the position above this RMS error. 0: default for the device type.
	int illuminatorRate_Hz = 60; ///< Polaris only: 20, 30 or 60 Hz. 0: the device's default (20 Hz).
};

/** Which port handles to list, see the NDI PHSR command. */
enum class NdiPortHandleSearch
{
	All = 0x00,
	ToBeFreed = 0x01,
	NotInitialized = 0x02,
	NotEnabled = 0x03,
	Enabled = 0x04
};

/** Physical location of a port handle, see the NDI PHINF command (port location). */
struct org_custusx_core_tracking_system_ndi_EXPORT NdiPortLocation
{
	bool valid = false;
	bool wireless = false;
	int port = -1;   ///< 0-based physical port number (wired tools), matches the port number in the tool file
	int channel = 0; ///< Channel on the port, non-zero only for the second sensor on a splitter (5DOF)
};

/** Tracking data for one port handle in one frame. */
/** BX port status bits used by the plugin */
namespace NdiPortStatus
{
const int OutOfVolume = 0x40;
const int PartiallyInVolume = 0x80;
}

/** BX tool information bits (Polaris) */
namespace NdiToolInfo
{
const int BadTransformFit = 0x01;
const int NotEnoughMarkers = 0x02;
}

struct org_custusx_core_tracking_system_ndi_EXPORT NdiToolSample
{
	int portHandle = -1;
	bool visible = false;          ///< false if the transform is missing (out of view) or the port is disabled
	Transform3D transform = Transform3D::Identity(); ///< camera/field generator to tool sensor, mm
	double errorRMS_mm = 0.0;
	int portStatus = 0;  ///< BX port status bits, see NdiPortStatus
	int toolInfo = 0;    ///< BX tool information bits, see NdiToolInfo. Polaris only.
	int markersUsed = -1; ///< Markers used to compute the transform, -1 if unknown. Polaris only.
	unsigned long frameNumber = 0; ///< The frame of this port handle, which may be older than the frame
	double received_ms = 0.0; ///< When the frame was read, before it was timestamped from the frame number
};

/** Tracking data for all port handles in one frame. */
struct org_custusx_core_tracking_system_ndi_EXPORT NdiFrame
{
	unsigned long frameNumber = 0;
	int systemStatus = 0; ///< BX system status bits, see NdiTrackerManager::describeSystemStatus()
	std::vector<NdiToolSample> samples;
};

/** \brief Interface to an NDI Aurora/Polaris device.
 *
 * The small set of NDI Combined API commands the tracking plugin needs. The real
 * implementation (NdiCapiDevice) uses the ndicapi library; tests use a fake device.
 * All calls are made from the tracker thread.
 *
 * \ingroup org_custusx_core_tracking_system_ndi
 */
class org_custusx_core_tracking_system_ndi_EXPORT NdiDevice
{
public:
	virtual ~NdiDevice() {}

	virtual bool open(const NdiConnectionSettings& settings) = 0; ///< Connect and initialize (INIT)
	virtual void close() = 0;
	virtual bool isOpen() const = 0;
	virtual QString getLastError() const = 0;
	virtual QString getDescription() const = 0; ///< Device and firmware, for the log
	virtual bool isInUseByAnotherApplication() const { return false; } ///< The last open() failed because another application uses the device

	virtual std::vector<int> searchPortHandles(NdiPortHandleSearch search) = 0; ///< PHSR
	virtual bool freePortHandle(int portHandle) = 0; ///< PHF
	virtual int requestWirelessPortHandle() = 0; ///< PHRQ, returns -1 on failure
	virtual bool loadToolDefinition(int portHandle, QString sromFile) = 0; ///< PVWR from a .rom file
	virtual bool initializePortHandle(int portHandle) = 0; ///< PINIT
	virtual bool enablePortHandle(int portHandle, bool staticTool) = 0; ///< PENA, static for reference tools
	virtual NdiPortLocation getPortLocation(int portHandle) = 0; ///< PHINF port location

	virtual bool setIlluminatorRate(int rate_Hz) = 0; ///< IRATE, Polaris: 20, 30 or 60 Hz
	virtual bool startTracking() = 0; ///< TSTART
	virtual bool stopTracking() = 0;  ///< TSTOP
	virtual bool readFrame(const std::vector<int>& portHandles, NdiFrame& frame) = 0; ///< BX
};
typedef std::shared_ptr<NdiDevice> NdiDevicePtr;

} // namespace cx

#endif /* CXNDIDEVICE_H_ */
