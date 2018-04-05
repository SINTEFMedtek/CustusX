/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxIGTLinkClientStreamer.h"

#include <QTcpSocket>
#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlImageMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"

#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxIGTLinkConversion.h"
#include "cxIGTLinkConversionImage.h"
#include "cxIGTLinkConversionSonixCXLegacy.h"
#include "cxCyclicActionLogger.h"
#include "cxUtilHelpers.h"
#include "cxTime.h"
#include "cxSender.h"
#include "vtkImageData.h"

namespace cx
{

IGTLinkClientStreamer::IGTLinkClientStreamer() :
	mHeadingReceived(false),
	mAddress(""),
	mPort(0)
{
}

IGTLinkClientStreamer::~IGTLinkClientStreamer()
{

}

void IGTLinkClientStreamer::setAddress(QString address, int port)
{
	mAddress = address;
	mPort = port;
}


void IGTLinkClientStreamer::startStreaming(SenderPtr sender)
{
	mSender = sender;
//	this->createSendTimer();
//	mTestTimer = new QTimer(this);
//	connect(mTestTimer, SIGNAL(timeout()), this, SLOT(myStreamSlot()));
//	std::cout << "IGTLinkClientStreamer::startStreaming " << std::endl;

	// Establish Connection
	mSocket.reset(new QTcpSocket());
	connect(mSocket.get(), SIGNAL(readyRead()), this, SLOT(readyReadSlot()), Qt::DirectConnection);
	connect(mSocket.get(), SIGNAL(hostFound()), this, SLOT(hostFoundSlot()), Qt::DirectConnection);
	connect(mSocket.get(), SIGNAL(connected()), this, SLOT(connectedSlot()), Qt::DirectConnection);
	connect(mSocket.get(), SIGNAL(disconnected()), this, SLOT(disconnectedSlot()), Qt::DirectConnection);
	connect(mSocket.get(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorSlot(QAbstractSocket::SocketError)),
					Qt::DirectConnection);

	if (!this->multipleTryConnectToHost())
	{
		reportError("IGTLinkClientStreamer: Failed to start streaming");
		mSocket.reset();
		return;
	}

	// Create a message buffer to receive header
	mHeaderMsg = igtl::MessageHeader::New();
}

bool IGTLinkClientStreamer::multipleTryConnectToHost()
{
	// hold here until all attempts are finished
	int numberOfConnectionAttempts = 5;
	int baseSleep = 300;
	for (int i=0; i<numberOfConnectionAttempts; ++i)
	{
		if (i>0)
			report(QString("[%2] Attempt %1 to connect to host").arg(i+1).arg(this->hostDescription()));
		if (this->tryConnectToHost())
			return true;
		sleep_ms(baseSleep*(i+1));
	}
	reportError(QString("[%1] Timeout connecting to host").arg(this->hostDescription()));
	return false;
}

bool IGTLinkClientStreamer::tryConnectToHost()
{
	mSocket->connectToHost(mAddress, mPort);

	int timeout = 5000;
	if (!mSocket->waitForConnected(timeout))
	{
		mSocket->disconnectFromHost();
		return false;
	}
	return true;
}

void IGTLinkClientStreamer::stopStreaming()
{
	if (mSocket)
	{
		mSocket->disconnectFromHost();
		mSocket.reset();
	}
	mSender.reset();
}

bool IGTLinkClientStreamer::isStreaming()
{
	return (mSocket && mSocket->isValid());
}

QString IGTLinkClientStreamer::hostDescription() const
{
	return mAddress + ":" + qstring_cast(mPort);
}

void IGTLinkClientStreamer::hostFoundSlot()
{
	report(QString("[%1] Found host").arg(this->hostDescription()));
//	report("Host found: " + this->hostDescription());
}
void IGTLinkClientStreamer::connectedSlot()
{
	reportSuccess(QString("[%1] Connected to host").arg(this->hostDescription()));
}
void IGTLinkClientStreamer::disconnectedSlot()
{
	report(QString("[%1] Disconnected from host").arg(this->hostDescription()));
//	report("Disconnected from host " + this->hostDescription());
}
void IGTLinkClientStreamer::errorSlot(QAbstractSocket::SocketError socketError)
{
	report(QString("[%1] Socket error [code=%2]: %3")
		   .arg(this->hostDescription())
		   .arg(QString::number(socketError))
		   .arg(mSocket->errorString()));
}

void IGTLinkClientStreamer::readyReadSlot()
{
	// read messages until one fails
	while (this->readOneMessage());
}

/**Read one IGTLink message from the socket.
 * Return false if there was not enough data to
 * read the entire message.
 */
bool IGTLinkClientStreamer::readOneMessage()
{

//  std::cout << "tick " << std::endl;

	if (!mHeadingReceived)
	{
//    std::cout << "client::tick: received: " << mSocket->bytesAvailable() << ", head needed: " << mHeaderMsg->GetPackSize() << std::endl;
		// Initialize receive buffer
		mHeaderMsg->InitPack();

		// ignore if not enough data (yet)
		if (mSocket->bytesAvailable() < mHeaderMsg->GetPackSize())
		{
			//std::cout << "Incomplete heading received, ignoring. " << std::endl;
			//std::cout << "available: " << mSocket->bytesAvailable() << ", needed " << mHeaderMsg->GetPackSize() << std::endl;
			return false;
		}

		// after peek: read to increase pos
		mSocket->read(reinterpret_cast<char*>(mHeaderMsg->GetPackPointer()), mHeaderMsg->GetPackSize());
		mHeadingReceived = true;

		// Deserialize the header
		mHeaderMsg->Unpack();
	}

	if (mHeadingReceived)
	{
//    std::cout << "client::tick: received: " << mSocket->bytesAvailable() << ", body needed: " << mHeaderMsg->GetBodySizeToRead() << std::endl;
		bool success = false;
		// Check data type and receive data body
//    if (QString(mHeaderMsg->GetDeviceType()) == "TRANSFORM")
//    {
//      ReceiveTransform(mSocket, mHeaderMsg);
//    }
//    else if (QString(mHeaderMsg->GetDeviceType() == "POSITION")
//    {
//      ReceivePosition(mSocket, mHeaderMsg);
//    }
		if (QString(mHeaderMsg->GetDeviceType()) == "IMAGE")
		{
			success = this->ReceiveImage(mSocket.get(), mHeaderMsg);
		}
		else if (QString(mHeaderMsg->GetDeviceType()) == "CX_US_ST")
		{
			success = this->ReceiveSonixStatus(mSocket.get(), mHeaderMsg);
		}
//    else if (QString(mHeaderMsg->GetDeviceType() == "STATUS")
//    {
//      ReceiveStatus(mSocket, mHeaderMsg);
//    }
		else
		{
			std::cerr << "Receiving : " << mHeaderMsg->GetDeviceType() << std::endl;
			mSocket->read(mHeaderMsg->GetBodySizeToRead());
		}

		if (success)
			mHeadingReceived = false; // restart
		else
			return false;
	}
//  std::cout << "  tock " << std::endl;
	return true;
}

bool IGTLinkClientStreamer::ReceiveSonixStatus(QTcpSocket* socket, igtl::MessageHeader::Pointer& header)
{
	IGTLinkUSStatusMessage::Pointer msg;
	msg = IGTLinkUSStatusMessage::New();
	msg->SetMessageHeader(header);
	msg->AllocatePack();

	if (socket->bytesAvailable() < msg->GetPackBodySize())
	{
		//std::cout << "Incomplete body received, ignoring. " << std::endl;
		return false;
	}
	socket->read(reinterpret_cast<char*>(msg->GetPackBodyPointer()), msg->GetPackBodySize());
	// Deserialize the transform data
	// If you want to do a CRC check, call Unpack(1).
	// If you want to skip CRC check, call Unpack() without argument.
	int c = msg->Unpack();
	if (c & (igtl::MessageHeader::UNPACK_BODY | igtl::MessageHeader::UNPACK_UNDEF)) // if CRC check is OK or skipped
	{
		this->addToQueue(msg);

		return true;
	}

	std::cout << "body crc failed!" << std::endl;
	return true;
}

namespace
{
QDateTime my_decode_timestamp(igtl::MessageBase* msg)
{
	// get timestamp from igtl second-format:
	igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
	msg->GetTimeStamp(timestamp);
	double timestampMS = timestamp->GetTimeStamp() * 1000;
	return QDateTime::fromMSecsSinceEpoch(timestampMS);
}

void write_time_info(igtl::ImageMessage::Pointer imgMsg)
{
	int kb = imgMsg->GetPackSize()/1024;
//	CX_LOG_CHANNEL_DEBUG("igtl_rec_test") << "unpacked: , " << kb << " kByte, name=" << imgMsg->GetDeviceName();
	QDateTime org_ts = my_decode_timestamp(imgMsg.GetPointer());
	QDateTime now_ts = QDateTime::currentDateTime();
	QString format = timestampMilliSecondsFormatNice();
	CX_LOG_CHANNEL_DEBUG("igtl_rec_test") << "received " << kb << "kByte"
										  << ", time=(" << org_ts.toString(format) << "->" << now_ts.toString(format) << ")"
										  << ", lag=" << org_ts.msecsTo(now_ts) << "ms";
}
}

bool IGTLinkClientStreamer::ReceiveImage(QTcpSocket* socket, igtl::MessageHeader::Pointer& header)
{
	// Create a message buffer to receive transform data
	igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
	imgMsg->SetMessageHeader(header);
	imgMsg->AllocatePack();

	// Receive transform data from the socket
	// ignore if not enough data (yet)
	if (socket->bytesAvailable() < imgMsg->GetPackBodySize())
	{
		//std::cout << "Incomplete body received, ignoring. " << std::endl;
		return false;
	}

	socket->read(reinterpret_cast<char*>(imgMsg->GetPackBodyPointer()), imgMsg->GetPackBodySize());
	// Deserialize the transform data
	// If you want to do a CRC check, call Unpack(1).
	// If you want to skip CRC check, call Unpack() without argument.
	int c = imgMsg->Unpack();

//	write_time_info(imgMsg);

	if (c & (igtl::MessageHeader::UNPACK_BODY | igtl::MessageHeader::UNPACK_UNDEF)) // if CRC check is OK or skipped
	{
		this->addToQueue(imgMsg);
		return true;
	}

	std::cout << "body crc failed!" << std::endl;
	return true;
}

void IGTLinkClientStreamer::addToQueue(IGTLinkUSStatusMessage::Pointer msg)
{
	// set temporary, then assume the image adder will pass this message on.
	mUnsentUSStatusMessage = msg;
}

void IGTLinkClientStreamer::addToQueue(igtl::ImageMessage::Pointer msg)
{
	IGTLinkConversion converter;
	IGTLinkConversionImage imageconverter;
    IGTLinkConversionSonixCXLegacy cxconverter;

    PackagePtr package(new Package());

    if (cxconverter.guessIsSonixLegacyFormat(msg->GetDeviceName()))
    {
        package->mImage = cxconverter.decode(msg);
    }
    else
    {
        package->mImage = imageconverter.decode(msg);
    }

	// if us status not sent, do it here
	if (mUnsentUSStatusMessage)
	{
        package->mProbe = converter.decode(mUnsentUSStatusMessage, msg, ProbeDefinitionPtr());

        if (cxconverter.guessIsSonixLegacyFormat(mUnsentUSStatusMessage->GetDeviceName()))
        {
            package->mProbe = cxconverter.decode(package->mProbe);
        }

				//Commenting out this line means that the last US sector information is always sent together with the image
//				mUnsentUSStatusMessage = IGTLinkUSStatusMessage::Pointer();
	}

    //Should only be needed if time stamp is set on another computer that is
    //not synched with the one running this code: e.g. The Ultrasonix scanner
    mStreamSynchronizer.syncToCurrentTime(package->mImage);

	mSender->send(package);
}


} // namespace cx


