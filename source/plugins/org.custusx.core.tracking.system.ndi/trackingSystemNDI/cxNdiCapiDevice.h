/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXNDICAPIDEVICE_H_
#define CXNDICAPIDEVICE_H_

#include "org_custusx_core_tracking_system_ndi_Export.h"

#include <QFile>
#include <QStringList>
#include <map>
#include "cxNdiDevice.h"

struct ndicapi;

namespace cx
{

/** \brief NdiDevice implemented with the ndicapi library (serial or network).
 *
 * Follows the connection sequence used by PLUS (probe, COMM to set the baud rate,
 * or INIT with a long timeout on network), which is well tested on Aurora and Polaris.
 *
 * \ingroup org_custusx_core_tracking_system_ndi
 */
class org_custusx_core_tracking_system_ndi_EXPORT NdiCapiDevice : public NdiDevice
{
public:
	NdiCapiDevice();
	virtual ~NdiCapiDevice();

	bool open(const NdiConnectionSettings& settings) override;
	void close() override;
	bool isOpen() const override;
	QString getLastError() const override;
	bool isInUseByAnotherApplication() const override;
	QString getDescription() const override;

	std::vector<int> searchPortHandles(NdiPortHandleSearch search) override;
	bool freePortHandle(int portHandle) override;
	int requestWirelessPortHandle() override;
	bool loadToolDefinition(int portHandle, QString sromFile) override;
	bool initializePortHandle(int portHandle) override;
	bool enablePortHandle(int portHandle, bool staticTool) override;
	NdiPortLocation getPortLocation(int portHandle) override;

	bool setIlluminatorRate(int rate_Hz) override;
	bool startTracking() override;
	bool stopTracking() override;
	bool readFrame(const std::vector<int>& portHandles, NdiFrame& frame) override;

	static QStringList getSerialPortCandidates();
	static bool isSerialDevice(QString hostname); ///< Empty (probe), /dev/..., COMn or \\.\COMn. Otherwise a network host
	static QString toSerialDeviceName(QString device); ///< Windows: COM10 and up must be opened as \\.\COM10
	static bool isSerialPortInUse(QString device); ///< The port exists, but another application has it open (Windows, Linux) or locked (Linux, macOS)
	static bool isOpenByAnotherProcess(QString path); ///< Linux: another process has the file open. Always false elsewhere.
	static NdiPortLocation parsePortLocation(const char location[14]);
	static int baudRateToNdiCode(int baudRate);
	static int illuminatorRateToNdiCode(int rate_Hz); ///< -1 if not supported
	static std::vector<int> getBaudRatesToTry(int requested); ///< The requested rate, then slower standard rates
	static bool isSlowerBaudRateWorthTrying(int ndiError); ///< After a failed COMM
	static bool isLostDeviceError(int ndiError); ///< The device didn't answer, or the connection is gone
	static int countUsedMarkers(const char markerInfo[10]); ///< From the 10 marker bytes in a BX reply, 4 bits per marker

private:
	bool openSerial(QString device, int baudRate);
	bool openNetwork(QString hostname, int port, bool takeOverDevice);
	bool isTrackingForAnotherClient(); ///< BX only succeeds in tracking mode: another client is using the device
	bool setBaudRate(int baudRate);
	bool resetToDefaultBaudRate();
	QString findSerialDevice();
	bool command(const char* format, ...);
	void log(const QString& text);
	bool isSerial() const;
	void setSerialTimeout(int timeout_ms);
	bool sendBX();
	NdiToolSample getBXSample(int portHandle);
	void log3DMarkerCount(int portHandle, const NdiToolSample& sample);
	const char* findBXToolMarkerInformation(int portHandle) const; ///< Tool info byte + 10 marker bytes, or null

	ndicapi* mDevice;
	bool mNetwork;
	bool mTracking;
	bool mRequestMarkerInfo;
	bool mMarkerInfoConfirmed; ///< BX with marker information has worked at least once
	bool mLastBXHasMarkerInfo;
	int mMarkerInfoFailures;
	bool mRequest3DMarkers; ///< Debug: request the 3D marker positions to log the marker count of each tool
	bool mLastBXHas3DMarkers;
	std::map<int, QString> mLast3DMarkerLog;
	bool mResponding; ///< False after the device stopped answering: don't wait for it when closing
	bool mInUse; ///< The last open() failed because another application uses the device
	QString mLastError;
	QString mLastReply; ///< Reply to the last text (non-binary) command
	QString mDescription;
	QFile mLogFile;
};

} // namespace cx

#endif /* CXNDICAPIDEVICE_H_ */
