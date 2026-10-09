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
#include <QFileInfo>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QTextStream>

#include "ndicapi.h"
#include "ndicapi_serial.h"
#include "cxNdiTransformUtils.h"
#include "cxLogger.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace cx
{

namespace
{
const int kFirstInitTimeout_ms = 5000; ///< The first INIT on a network device can take several seconds
const int kCommandTimeout_ms = 1000;
const int kSerialTimeout_ms = 5000; ///< ndicapi's default. Commands like INIT, PINIT and TSTART take up to a few seconds
const int kTrackingSerialTimeout_ms = 500; ///< While tracking a frame comes every 15-50 ms: a lost device is found quickly
const int kToolDefinitionRetries = 3;
const int kMarkerInfoAttempts = 3; ///< BX with marker information fails this many times while BX without works: the device doesn't support it
}

NdiCapiDevice::NdiCapiDevice() :
	mDevice(nullptr),
	mNetwork(false),
	mTracking(false),
	mRequestMarkerInfo(false),
	mMarkerInfoConfirmed(false),
	mLastBXHasMarkerInfo(false),
	mMarkerInfoFailures(0),
	mRequest3DMarkers(false),
	mLastBXHas3DMarkers(false),
	mResponding(true),
	mInUse(false)
{
}

NdiCapiDevice::~NdiCapiDevice()
{
	this->close();
}

bool NdiCapiDevice::open(const NdiConnectionSettings& settings)
{
	this->close();
	mResponding = true;
	mInUse = false;
	mRequestMarkerInfo = settings.requestMarkerInfo;
	mRequest3DMarkers = settings.requestMarkerInfo && settings.debugMarkerCount;
	mLast3DMarkerLog.clear();
	mMarkerInfoConfirmed = false;
	mMarkerInfoFailures = 0;
	if (!settings.communicationLogFile.isEmpty())
	{
		mLogFile.setFileName(settings.communicationLogFile);
		mLogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
	}

	QString hostname = settings.hostname.trimmed();
	bool serial = isSerialDevice(hostname);
	bool success = false;
	if (!serial)
	{
		success = this->openNetwork(hostname, settings.networkPort, settings.takeOverDevice);
	}
	else
	{
		QString device = hostname.isEmpty() ? this->findSerialDevice() : toSerialDeviceName(hostname);
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

bool NdiCapiDevice::isSerialDevice(QString hostname)
{
	return hostname.isEmpty() || hostname.startsWith("/dev/") || hostname.startsWith("COM", Qt::CaseInsensitive)
			|| hostname.startsWith("\\\\.\\");
}

QString NdiCapiDevice::toSerialDeviceName(QString device)
{
	QString retval = device;
	QRegularExpressionMatch match = QRegularExpression("^COM(\\d+)$", QRegularExpression::CaseInsensitiveOption).match(device);
	if (match.hasMatch() && match.captured(1).toInt() >= 10)
	{
		retval = "\\\\.\\COM" + match.captured(1);
	}
	return retval;
}

bool NdiCapiDevice::isOpenByAnotherProcess(QString path)
{
	bool retval = false;
#ifdef __linux__
	QString target = QFileInfo(path).canonicalFilePath();
	QString self = QString::number(QCoreApplication::applicationPid());
	QStringList processes = QDir("/proc").entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (const QString& process : processes)
	{
		bool isProcess = false;
		process.toLongLong(&isProcess);
		if (!retval && isProcess && process != self && !target.isEmpty())
		{
			QDir fds("/proc/" + process + "/fd");
			for (const QFileInfo& fd : fds.entryInfoList(QDir::AllEntries | QDir::System | QDir::NoDotAndDotDot))
			{
				retval = retval || (fd.symLinkTarget() == target);
			}
		}
	}
#else
	Q_UNUSED(path);
#endif
	return retval;
}

bool NdiCapiDevice::isSerialPortInUse(QString device)
{
	QByteArray name = toSerialDeviceName(device).toLocal8Bit();
	bool retval = isOpenByAnotherProcess(QString::fromLocal8Bit(name)); // e.g. a server that neither locks nor opens it exclusively
#ifdef _WIN32
	HANDLE handle = CreateFileA(name.constData(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		retval = (GetLastError() == ERROR_ACCESS_DENIED); // opened by another application
	}
	else
	{
		CloseHandle(handle);
	}
#else
	int port = retval ? -1 : ::open(name.constData(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (!retval && port < 0)
	{
		retval = (errno == EBUSY); // opened exclusively by another application
	}
	else if (!retval)
	{
		struct flock lock = {};
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		retval = (fcntl(port, F_GETLK, &lock) == 0) && (lock.l_type != F_UNLCK); // locked, as ndicapi does
		::close(port);
	}
#endif
	return retval;
}

bool NdiCapiDevice::isTrackingForAnotherClient()
{
	bool responding = mResponding; // a device that doesn't answer is found by INIT
	ndiTimeoutSocket(mDevice, kCommandTimeout_ms);
	bool retval = this->command("BX:%04X", NDI_XFORMS_AND_STATUS);
	mResponding = responding;
	return retval;
}

QString NdiCapiDevice::findSerialDevice()
{
	QString retval;
	QStringList candidates = getSerialPortCandidates();
	QStringList inUse;
	for (const QString& candidate : candidates)
	{
		if (!retval.isEmpty())
		{
			continue;
		}
		if (isSerialPortInUse(candidate)) // not probed: the probe sends INIT, which would disturb the other application
		{
			inUse << candidate;
		}
		else if (ndiSerialProbe(candidate.toLocal8Bit().constData(), false) == NDI_OKAY)
		{
			retval = candidate;
		}
	}
	if (retval.isEmpty())
	{
		mInUse = !inUse.isEmpty();
		mLastError = QString("No NDI device found on the serial ports (tried %1)").arg(candidates.join(", "));
		if (mInUse)
		{
			mLastError += QString(", used by another application: %1").arg(inUse.join(", "));
		}
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
	if (isSerialPortInUse(device)) // checked first: probing sends INIT, which would disturb the other application
	{
		mInUse = true;
		mLastError = QString("The serial port %1 is used by another application").arg(device);
		return false;
	}
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
	bool success = false;
	bool tryNext = true;
	for (int rate : getBaudRatesToTry(baudRate))
	{
		if (tryNext)
		{
			ndiCommand(mDevice, "COMM:%X%03d%d", baudRateToNdiCode(rate), NDI_8N1, NDI_NOHANDSHAKE);
			int error = ndiGetError(mDevice);
			if (error == NDI_OKAY && !this->command("VER:4"))
			{
				error = NDI_BAD_COMM; // e.g. a serial adapter that can't use the rate
			}
			success = (error == NDI_OKAY);
			this->log(QString("COMM %1 baud: %2").arg(rate).arg(ndiErrorString(error)));
			tryNext = !success && isSlowerBaudRateWorthTrying(error);
			if (tryNext && error == NDI_BAD_COMM)
			{
				tryNext = this->resetToDefaultBaudRate(); // the device switched, but this computer couldn't
			}
			if (!success && !tryNext)
			{
				mLastError = QString("Failed to set serial communication parameters: %1").arg(ndiErrorString(error));
			}
		}
	}
	return success;
}

bool NdiCapiDevice::resetToDefaultBaudRate()
{
	ndiCommand(mDevice, nullptr); // serial break: the device resets to 9600 baud
	int error = ndiGetError(mDevice);
	this->log(QString("Reset with serial break: %1").arg(ndiErrorString(error)));
	return (error == NDI_OKAY) && this->command("INIT:");
}

std::vector<int> NdiCapiDevice::getBaudRatesToTry(int requested)
{
	std::vector<int> retval;
	bool supported = (baudRateToNdiCode(requested) >= 0);
	if (supported)
	{
		retval.push_back(requested);
	}
	for (int rate : {115200, 57600, 38400, 19200, 9600})
	{
		if (rate < requested || !supported)
		{
			retval.push_back(rate);
		}
	}
	return retval;
}

bool NdiCapiDevice::isSlowerBaudRateWorthTrying(int ndiError)
{
	return (ndiError == NDI_COMM_FAIL) || (ndiError == NDI_BAD_COMM);
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

bool NdiCapiDevice::openNetwork(QString hostname, int port, bool takeOverDevice)
{
	mDevice = ndiOpenNetwork(hostname.toLocal8Bit().constData(), port);
	if (!mDevice)
	{
		mLastError = QString("Could not connect to %1:%2").arg(hostname).arg(port);
		return false;
	}
	mNetwork = true;
	this->log(QString("Connected to %1:%2").arg(hostname).arg(port));
	// INIT would stop the tracking of another application using the device
	if (!takeOverDevice && this->isTrackingForAnotherClient())
	{
		mInUse = true;
		mLastError = QString("The NDI device at %1 is already tracking, probably for another application "
							 "(e.g. an OpenIGTLink server). Not connecting, so its tracking continues. If no other "
							 "application uses it, restart the device or set ndiTrackingTakeOverDevice=true.").arg(hostname);
		return false;
	}
	ndiTimeoutSocket(mDevice, kFirstInitTimeout_ms);
	bool success = this->command("INIT:");
	ndiTimeoutSocket(mDevice, kCommandTimeout_ms);
	return success;
}

void NdiCapiDevice::close()
{
	if (mDevice)
	{
		if (mTracking && mResponding)
		{
			this->stopTracking();
		}
		if (this->isSerial())
		{
			if (mResponding)
			{
				this->command("COMM:00000"); // back to 9600 baud, so the next connection finds the device
			}
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

bool NdiCapiDevice::isLostDeviceError(int ndiError)
{
	return (ndiError == NDI_TIMEOUT) || (ndiError == NDI_WRITE_ERROR) || (ndiError == NDI_READ_ERROR);
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

bool NdiCapiDevice::isInUseByAnotherApplication() const
{
	return mInUse;
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
		mResponding = mResponding && !isLostDeviceError(error);
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

bool NdiCapiDevice::setIlluminatorRate(int rate_Hz)
{
	int code = illuminatorRateToNdiCode(rate_Hz);
	bool success = (code >= 0) && this->command("IRATE:%d", code);
	if (code < 0)
	{
		mLastError = QString("%1 Hz is not an illuminator rate (20, 30 or 60 Hz)").arg(rate_Hz);
	}
	return success;
}

int NdiCapiDevice::illuminatorRateToNdiCode(int rate_Hz)
{
	int retval = -1;
	switch (rate_Hz)
	{
	case 20: retval = 0; break;
	case 30: retval = 1; break;
	case 60: retval = 2; break;
	default: break;
	}
	return retval;
}

bool NdiCapiDevice::startTracking()
{
	mTracking = this->command("TSTART:");
	if (mTracking)
	{
		this->setSerialTimeout(kTrackingSerialTimeout_ms);
	}
	return mTracking;
}

bool NdiCapiDevice::stopTracking()
{
	mTracking = false;
	this->setSerialTimeout(kSerialTimeout_ms);
	return this->command("TSTOP:");
}

void NdiCapiDevice::setSerialTimeout(int timeout_ms)
{
	if (this->isSerial())
	{
		ndiSerialTimeout(mDevice->SerialDevice, timeout_ms);
	}
}

bool NdiCapiDevice::readFrame(const std::vector<int>& portHandles, NdiFrame& frame)
{
	bool success = this->sendBX();
	if (success)
	{
		frame.frameNumber = 0;
		frame.systemStatus = ndiGetBXSystemStatus(mDevice);
		frame.samples.clear();
		for (int portHandle : portHandles)
		{
			frame.frameNumber = std::max(frame.frameNumber, ndiGetBXFrame(mDevice, portHandle));
			frame.samples.push_back(this->getBXSample(portHandle));
		}
	}
	return success;
}

bool NdiCapiDevice::sendBX()
{
	if (mRequest3DMarkers)
	{
		mLastBXHasMarkerInfo = true;
		mLastBXHas3DMarkers = true;
		if (this->command("BX:%04X", NDI_XFORMS_AND_STATUS | NDI_ADDITIONAL_INFO | NDI_3D_MARKER_POSITIONS))
		{
			return true;
		}
		CX_LOG_INFO() << "NDI Tracker debug: BX with 3D marker positions failed, no longer requested: " << mLastError;
		mRequest3DMarkers = false;
	}
	mLastBXHas3DMarkers = false;
	mLastBXHasMarkerInfo = mRequestMarkerInfo;
	bool success = this->command("BX:%04X", NDI_XFORMS_AND_STATUS | (mRequestMarkerInfo ? NDI_ADDITIONAL_INFO : 0));
	if (!success && mRequestMarkerInfo && !mMarkerInfoConfirmed)
	{
		QString markerInfoError = mLastError;
		mLastBXHasMarkerInfo = false;
		success = this->command("BX:%04X", NDI_XFORMS_AND_STATUS);
		mMarkerInfoFailures = success ? mMarkerInfoFailures + 1 : 0;
		if (mMarkerInfoFailures >= kMarkerInfoAttempts)
		{
			reportWarning("NDI Tracker: The device doesn't report marker information, tracking without it: " + markerInfoError);
			mRequestMarkerInfo = false;
		}
	}
	mMarkerInfoConfirmed = mMarkerInfoConfirmed || (success && mLastBXHasMarkerInfo);
	return success;
}

NdiToolSample NdiCapiDevice::getBXSample(int portHandle)
{
	NdiToolSample sample;
	sample.portHandle = portHandle;
	float values[8];
	sample.visible = (ndiGetBXTransform(mDevice, portHandle, values) == NDI_OKAY);
	if (sample.visible)
	{
		sample.transform = ndiQuaternionToTransform3D(values[0], values[1], values[2], values[3],
													  values[4], values[5], values[6]);
		sample.errorRMS_mm = values[7];
	}
	sample.portStatus = ndiGetBXPortStatus(mDevice, portHandle);
	const char* info = mLastBXHasMarkerInfo ? this->findBXToolMarkerInformation(portHandle) : nullptr;
	if (info)
	{
		sample.toolInfo = info[0] & 0xFF;
		sample.markersUsed = countUsedMarkers(info + 1);
	}
	if (mLastBXHas3DMarkers)
	{
		this->log3DMarkerCount(portHandle, sample);
	}
	return sample;
}

void NdiCapiDevice::log3DMarkerCount(int portHandle, const NdiToolSample& sample)
{
	for (int i = 0; i < mDevice->BxHandleCount; ++i)
	{
		if (static_cast<unsigned char>(mDevice->BxHandles[i]) == portHandle && mDevice->BxHandlesStatus[i] != NDI_HANDLE_DISABLED)
		{
			QString text = QString("3D marker count %1, markers used %2, visible %3, out of volume bits %4")
					.arg(static_cast<int>(mDevice->Bx3DMarkerCount[i]))
					.arg(sample.markersUsed)
					.arg(sample.visible ? "yes" : "no")
					.arg(static_cast<unsigned char>(mDevice->Bx3DMarkerOutOfVolume[i][0]), 8, 2, QChar('0'));
			if (mLast3DMarkerLog[portHandle] != text)
			{
				mLast3DMarkerLog[portHandle] = text;
				CX_LOG_INFO() << QString("NDI Tracker debug: port %1: %2").arg(portHandle, 2, 16, QChar('0')).arg(text);
			}
		}
	}
}

const char* NdiCapiDevice::findBXToolMarkerInformation(int portHandle) const
{
	const char* retval = nullptr;
	for (int i = 0; i < mDevice->BxHandleCount; ++i)
	{
		if (static_cast<unsigned char>(mDevice->BxHandles[i]) == portHandle && mDevice->BxHandlesStatus[i] != NDI_HANDLE_DISABLED)
		{
			retval = mDevice->BxToolMarkerInformation[i];
		}
	}
	return retval;
}

int NdiCapiDevice::countUsedMarkers(const char markerInfo[10])
{
	const int kMarkerUsed = 3;
	int retval = 0;
	for (int i = 0; i < 10; ++i)
	{
		int lowMarker = markerInfo[i] & 0x0F;
		int highMarker = (markerInfo[i] >> 4) & 0x0F;
		retval += (lowMarker == kMarkerUsed ? 1 : 0) + (highMarker == kMarkerUsed ? 1 : 0);
	}
	return retval;
}

} // namespace cx
