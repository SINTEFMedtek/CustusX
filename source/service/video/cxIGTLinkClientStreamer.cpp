/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#include "cxIGTLinkClientStreamer.h"

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlImageMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"

#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxIGTLinkConversion.h"
#include "cxCyclicActionLogger.h"

namespace cx
{

IGTLinkClientStreamer::IGTLinkClientStreamer() :
	mHeadingReceived(false),
	mAddress(""),
	mPort(0),
	mSocket(NULL)
{
//	this->setSendInterval(200);


}

IGTLinkClientStreamer::~IGTLinkClientStreamer()
{

}

void IGTLinkClientStreamer::setAddress(QString address, int port)
{
	mAddress = address;
	mPort = port;
	std::cout << "IGTLinkClientStreamer::setAddress " << std::endl;
}


bool IGTLinkClientStreamer::startStreaming(SenderPtr sender)
{
	mSender = sender;
//	this->createSendTimer();
//	mTestTimer = new QTimer(this);
//	connect(mTestTimer, SIGNAL(timeout()), this, SLOT(myStreamSlot()));
//	std::cout << "IGTLinkClientStreamer::startStreaming " << std::endl;

	// Establish Connection
	mSocket = new QTcpSocket();
	connect(mSocket, SIGNAL(readyRead()), this, SLOT(readyReadSlot()), Qt::DirectConnection);
	connect(mSocket, SIGNAL(hostFound()), this, SLOT(hostFoundSlot()), Qt::DirectConnection);
	connect(mSocket, SIGNAL(connected()), this, SLOT(connectedSlot()), Qt::DirectConnection);
	connect(mSocket, SIGNAL(disconnected()), this, SLOT(disconnectedSlot()), Qt::DirectConnection);
	connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorSlot(QAbstractSocket::SocketError)),
					Qt::DirectConnection);

	report("Looking for host: " + this->hostDescription());
	mSocket->connectToHost(mAddress, mPort);

	int timeout = 5000;
	if (!mSocket->waitForConnected(timeout))
	{
		reportWarning("Timeout looking for host " + this->hostDescription());
		mSocket->disconnectFromHost();
		delete mSocket;
		mSocket = NULL;
		return false;
	}

	// Create a message buffer to receive header
	mHeaderMsg = igtl::MessageHeader::New();


	return true;
}

void IGTLinkClientStreamer::stopStreaming()
{
//	std::cout << "IGTLinkClientStreamer::stopStreaming " << std::endl;

	if (mSocket)
	{
		mSocket->disconnectFromHost();
	//  std::cout << "finished openIGTLink client thread" << std::endl;
		delete mSocket;
		mSocket = NULL;
	}
	mSender.reset();
}

QString IGTLinkClientStreamer::getType()
{
	return "IGTLinkClient";
}

//void IGTLinkClientStreamer::myStreamSlot()
//{
//	std::cout << "-- IGTLinkClientStreamer::myStreamSlot() " << std::endl;
//}

QString IGTLinkClientStreamer::hostDescription() const
{
	return mAddress + ":" + qstring_cast(mPort);
}

void IGTLinkClientStreamer::hostFoundSlot()
{
	report("Host found: " + this->hostDescription());
}
void IGTLinkClientStreamer::connectedSlot()
{
	reportSuccess("Connected to host " + this->hostDescription());
//	emit connected(true);
}
void IGTLinkClientStreamer::disconnectedSlot()
{
	report("Disconnected from host " + this->hostDescription());
//	emit connected(false);
}
void IGTLinkClientStreamer::errorSlot(QAbstractSocket::SocketError socketError)
{
	reportError(
					"Socket error [Host=" + this->hostDescription() + ", Code=" + QString::number(socketError) + "] "
									+ mSocket->errorString());
}

void IGTLinkClientStreamer::readyReadSlot()
{
	// read messages until one fails
	while (this->readOneMessage());

//	readOneMessage();
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
			success = this->ReceiveImage(mSocket, mHeaderMsg);
		}
		else if (QString(mHeaderMsg->GetDeviceType()) == "CX_US_ST")
		{
			success = this->ReceiveSonixStatus(mSocket, mHeaderMsg);
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

bool IGTLinkClientStreamer::ReceiveImage(QTcpSocket* socket, igtl::MessageHeader::Pointer& header)
{
	// Create a message buffer to receive transform data
	IGTLinkImageMessage::Pointer imgMsg;
	imgMsg = IGTLinkImageMessage::New();
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

void IGTLinkClientStreamer::addToQueue(IGTLinkImageMessage::Pointer msg)
{
	IGTLinkConversion converter;

	PackagePtr package(new Package());
	package->mIgtLinkImageMessage = msg;

	// if us status not sent, do it here
	if (mUnsentUSStatusMessage)
	{
		package->mIgtLinkUSStatusMessage = mUnsentUSStatusMessage;
		mUnsentUSStatusMessage = IGTLinkUSStatusMessage::Pointer();
	}

	mSender->send(package);
}


} // namespace cx


