/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxNdiCapiDevice.h"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <QDateTime>
#include <QDir>
#include <QTextStream>

#include "ndicapi.h"
#include "cxNdiTransformUtils.h"

namespace cx
{

namespace
{
const int kFirstInitTimeout_ms = 5000; ///< The first INIT on a network device can take several seconds
const int kCommandTimeout_ms = 1000;
const int kToolDefinitionRetries = 3;
}

NdiCapiDevice::NdiCapiDevice() :
	mDevice(nullptr),
	mNetwork(false),
	mTracking(false)
{
}

NdiCapiDevice::~NdiCapiDevice()
{
	this->close();
}

bool NdiCapiDevice::open(const NdiConnectionSettings& settings)
{
	this->close();
	if (!settings.communicationLogFile.isEmpty())
	{
		mLogFile.setFileName(settings.communicationLogFile);
		mLogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
	}

	QString hostname = settings.hostname.trimmed();
	bool serial = hostname.isEmpty() || hostname.startsWith("/dev/") || hostname.startsWith("COM", Qt::CaseInsensitive);
	bool success = false;
	if (!serial)
	{
		success = this->openNetwork(hostname, settings.networkPort);
	}
	else
	{
		QString device = hostname.isEmpty() ? this->findSerialDevice() : hostname;
		success = !device.isEmpty() && this->openSerial(device, settings.baudRate);
	}

	if (success && this->command("VER:4"))
	{
		mDescription = mLastReply.section('\n', 0, 0).trimmed();
	}
	if (!success)
	{
		this->close();
	}
	return success;
}

QString NdiCapiDevice::findSerialDevice()
{
	QString retval;
	QStringList candidates = getSerialPortCandidates();
	for (const QString& candidate : candidates)
	{
		if (retval.isEmpty() && ndiSerialProbe(candidate.toLocal8Bit().constData(), false) == NDI_OKAY)
		{
			retval = candidate;
		}
	}
	if (retval.isEmpty())
	{
		mLastError = QString("No NDI device found on the serial ports (tried %1)").arg(candidates.join(", "));
	}
	this->log("Serial port probe: " + (retval.isEmpty() ? mLastError : retval));
	return retval;
}

QStringList NdiCapiDevice::getSerialPortCandidates()
{
	QStringList retval;
#if defined(__linux__)
	QDir dev("/dev");
	QStringList usbPorts = dev.entryList(QStringList() << "ttyUSB*" << "ttyACM*", QDir::System, QDir::Name);
	for (const QString& port : usbPorts)
	{
		retval << "/dev/" + port;
	}
#endif
	for (int i = 0; i < 20; ++i)
	{
		const char* name = ndiSerialDeviceName(i);
		if (name && !retval.contains(QString(name)))
		{
			retval << QString(name);
		}
	}
	return retval;
}

bool NdiCapiDevice::openSerial(QString device, int baudRate)
{
	int probeResult = ndiSerialProbe(device.toLocal8Bit().constData(), false);
	if (probeResult != NDI_OKAY)
	{
		mLastError = QString("No NDI device on %1: %2").arg(device).arg(ndiErrorString(probeResult));
		return false;
	}
	mDevice = ndiOpenSerial(device.toLocal8Bit().constData());
	if (!mDevice)
	{
		mLastError = QString("Could not open serial port %1").arg(device);
		return false;
	}
	mNetwork = false;
	this->log("Opened serial port " + device);
	return this->setBaudRate(baudRate);
}

bool NdiCapiDevice::setBaudRate(int baudRate)
{
	// Try the requested rate first, then fall back to slower ones (as PLUS does)
	std::vector<int> rates = {baudRate, 115200, 57600, 38400, 19200, 9600};
	bool success = false;
	for (int rate : rates)
	{
		int code = baudRateToNdiCode(rate);
		if (!success && code >= 0)
		{
			ndiCommand(mDevice, "COMM:%X%03d%d", code, NDI_8N1, NDI_NOHANDSHAKE);
			int error = ndiGetError(mDevice);
			success = (error == NDI_OKAY);
			this->log(QString("COMM %1 baud: %2").arg(rate).arg(ndiErrorString(error)));
			if (!success && error != NDI_COMM_FAIL)
			{
				mLastError = QString("Failed to set serial communication parameters: %1").arg(ndiErrorString(error));
				break;
			}
		}
	}
	return success;
}

int NdiCapiDevice::baudRateToNdiCode(int baudRate)
{
	int retval = -1;
	switch (baudRate)
	{
	case 9600: retval = NDI_9600; break;
	case 14400: retval = NDI_14400; break;
	case 19200: retval = NDI_19200; break;
	case 38400: retval = NDI_38400; break;
	case 57600: retval = NDI_57600; break;
	case 115200: retval = NDI_115200; break;
	case 230400: retval = NDI_230400; break;
	case 921600: retval = NDI_921600; break;
	case 1228739: retval = NDI_1228739; break;
	default: break;
	}
	return retval;
}

bool NdiCapiDevice::openNetwork(QString hostname, int port)
{
	mDevice = ndiOpenNetwork(hostname.toLocal8Bit().constData(), port);
	if (!mDevice)
	{
		mLastError = QString("Could not connect to %1:%2").arg(hostname).arg(port);
		return false;
	}
	mNetwork = true;
	this->log(QString("Connected to %1:%2").arg(hostname).arg(port));
	ndiTimeoutSocket(mDevice, kFirstInitTimeout_ms);
	bool success = this->command("INIT:");
	ndiTimeoutSocket(mDevice, kCommandTimeout_ms);
	return success;
}

void NdiCapiDevice::close()
{
	if (mDevice)
	{
		if (mTracking)
		{
			this->stopTracking();
		}
		if (this->isSerial())
		{
			this->command("COMM:00000"); // back to 9600 baud, so the next connection finds the device
			ndiCloseSerial(mDevice);
		}
		else
		{
			ndiCloseNetwork(mDevice);
		}
		mDevice = nullptr;
		this->log("Closed connection");
	}
	mLogFile.close();
}

bool NdiCapiDevice::isOpen() const
{
	return mDevice != nullptr;
}

bool NdiCapiDevice::isSerial() const
{
	return mDevice && !mNetwork;
}

QString NdiCapiDevice::getLastError() const
{
	return mLastError;
}

QString NdiCapiDevice::getDescription() const
{
	return mDescription;
}

bool NdiCapiDevice::command(const char* format, ...)
{
	bool success = false;
	if (mDevice)
	{
		va_list args;
		va_start(args, format);
		char commandText[2048];
		vsnprintf(commandText, sizeof(commandText), format, args);
		va_end(args);

		const char* reply = ndiCommand(mDevice, "%s", commandText);
		int error = ndiGetError(mDevice);
		success = (error == NDI_OKAY);
		if (!success)
		{
			mLastError = QString("%1 failed: %2").arg(commandText).arg(ndiErrorString(error));
		}
		bool binaryReply = QString(commandText).startsWith("BX");
		mLastReply = (binaryReply || !reply) ? QString() : QString(reply);
		this->log(QString("%1 -> %2").arg(commandText).arg(binaryReply || !reply ? QString(ndiErrorString(error)) : mLastReply.trimmed()));
	}
	else
	{
		mLastError = "Not connected to an NDI device";
	}
	return success;
}

void NdiCapiDevice::log(const QString& text)
{
	if (mLogFile.isOpen())
	{
		QTextStream stream(&mLogFile);
		stream << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << " " << text << "\n";
	}
}

std::vector<int> NdiCapiDevice::searchPortHandles(NdiPortHandleSearch search)
{
	std::vector<int> retval;
	if (this->command("PHSR:%02X", static_cast<int>(search)))
	{
		int count = ndiGetPHSRNumberOfHandles(mDevice);
		for (int i = 0; i < count; ++i)
		{
			retval.push_back(ndiGetPHSRHandle(mDevice, i));
		}
	}
	return retval;
}

bool NdiCapiDevice::freePortHandle(int portHandle)
{
	return this->command("PHF:%02X", portHandle);
}

int NdiCapiDevice::requestWirelessPortHandle()
{
	int retval = -1;
	if (this->command("PHRQ:*********1****"))
	{
		retval = ndiGetPHRQHandle(mDevice);
	}
	return retval;
}

bool NdiCapiDevice::loadToolDefinition(int portHandle, QString sromFile)
{
	QByteArray filename = sromFile.toLocal8Bit();
	bool success = false;
	for (int attempt = 0; attempt < kToolDefinitionRetries && !success; ++attempt)
	{
		int error = ndiPVWRFromFile(mDevice, portHandle, filename.data());
		success = (error == NDI_OKAY);
		this->log(QString("PVWR %1 from %2 (attempt %3): %4").arg(portHandle, 2, 16, QChar('0')).arg(sromFile).arg(attempt + 1).arg(ndiErrorString(error)));
		if (!success)
		{
			mLastError = QString("Failed to load %1: %2").arg(sromFile).arg(ndiErrorString(error));
		}
	}
	return success;
}

bool NdiCapiDevice::initializePortHandle(int portHandle)
{
	return this->command("PINIT:%02X", portHandle);
}

bool NdiCapiDevice::enablePortHandle(int portHandle, bool staticTool)
{
	return this->command("PENA:%02X%c", portHandle, staticTool ? 'S' : 'D');
}

NdiPortLocation NdiCapiDevice::getPortLocation(int portHandle)
{
	NdiPortLocation retval;
	if (this->command("PHINF:%02X%04X", portHandle, NDI_BASIC | NDI_PORT_LOCATION))
	{
		char location[14];
		if (ndiGetPHINFPortLocation(mDevice, location) == NDI_OKAY)
		{
			retval = parsePortLocation(location);
		}
	}
	return retval;
}

NdiPortLocation NdiCapiDevice::parsePortLocation(const char location[14])
{
	// 8 chars device number, 1 char system (0 Polaris/1 Aurora), 1 char wired(0)/wireless(1),
	// 2 chars 1-based port number, 2 chars hexadecimal channel number.
	NdiPortLocation retval;
	QString text = QString::fromLatin1(location, 14);
	bool portOk = false;
	bool channelOk = false;
	int port = text.mid(10, 2).toInt(&portOk, 10);
	int channel = text.mid(12, 2).toInt(&channelOk, 16);
	retval.valid = portOk && channelOk;
	retval.wireless = (text.at(9) == '1');
	retval.port = portOk ? port - 1 : -1;
	retval.channel = channelOk ? channel : 0;
	return retval;
}

bool NdiCapiDevice::startTracking()
{
	mTracking = this->command("TSTART:");
	return mTracking;
}

bool NdiCapiDevice::stopTracking()
{
	mTracking = false;
	return this->command("TSTOP:");
}

bool NdiCapiDevice::readFrame(const std::vector<int>& portHandles, NdiFrame& frame)
{
	bool success = this->command("BX:%04X", NDI_XFORMS_AND_STATUS);
	if (success)
	{
		frame.frameNumber = 0;
		frame.samples.clear();
		for (int portHandle : portHandles)
		{
			float values[8];
			NdiToolSample sample;
			sample.portHandle = portHandle;
			sample.visible = (ndiGetBXTransform(mDevice, portHandle, values) == NDI_OKAY);
			if (sample.visible)
			{
				sample.transform = ndiQuaternionToTransform3D(values[0], values[1], values[2], values[3],
															  values[4], values[5], values[6]);
				sample.errorRMS_mm = values[7];
			}
			frame.frameNumber = std::max(frame.frameNumber, ndiGetBXFrame(mDevice, portHandle));
			frame.samples.push_back(sample);
		}
	}
	return success;
}

} // namespace cx
