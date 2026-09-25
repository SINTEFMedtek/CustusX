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
	QString getDescription() const override;

	std::vector<int> searchPortHandles(NdiPortHandleSearch search) override;
	bool freePortHandle(int portHandle) override;
	int requestWirelessPortHandle() override;
	bool loadToolDefinition(int portHandle, QString sromFile) override;
	bool initializePortHandle(int portHandle) override;
	bool enablePortHandle(int portHandle, bool staticTool) override;
	NdiPortLocation getPortLocation(int portHandle) override;

	bool startTracking() override;
	bool stopTracking() override;
	bool readFrame(const std::vector<int>& portHandles, NdiFrame& frame) override;

	static QStringList getSerialPortCandidates();
	static NdiPortLocation parsePortLocation(const char location[14]);
	static int baudRateToNdiCode(int baudRate);

private:
	bool openSerial(QString device, int baudRate);
	bool openNetwork(QString hostname, int port);
	bool setBaudRate(int baudRate);
	QString findSerialDevice();
	bool command(const char* format, ...);
	void log(const QString& text);
	bool isSerial() const;

	ndicapi* mDevice;
	bool mNetwork;
	bool mTracking;
	QString mLastError;
	QString mLastReply; ///< Reply to the last text (non-binary) command
	QString mDescription;
	QFile mLogFile;
};

} // namespace cx

#endif /* CXNDICAPIDEVICE_H_ */
