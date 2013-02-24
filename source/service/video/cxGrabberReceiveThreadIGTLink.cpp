// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxGrabberReceiveThreadIGTLink.h"

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlImageMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"

#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "cxIGTLinkConversion.h"
#include "cxRenderTimer.h"

//int ReceiveTransform(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
//{
//	std::cerr << "Receiving TRANSFORM data type." << std::endl;

//	// Create a message buffer to receive transform data
//	igtl::TransformMessage::Pointer transMsg;
//	transMsg = igtl::TransformMessage::New();
//	transMsg->SetMessageHeader(header);
//	transMsg->AllocatePack();

//	// Receive transform data from the socket
//	socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());
//	// Deserialize the transform data
//	// If you want to do a CRC check, call Unpack(1).
//	// If you want to skip CRC check, call Unpack() without argument.
//	int c = transMsg->Unpack();

//	if (c & (igtl::MessageHeader::UNPACK_BODY | igtl::MessageHeader::UNPACK_UNDEF)) // if CRC check is OK or skipped
//	{
//		// Retrive the transform data
//		igtl::Matrix4x4 matrix;
//		transMsg->GetMatrix(matrix);
//		igtl::PrintMatrix(matrix);
//		return 1;
//	}

//	return 0;
//}

//int ReceivePosition(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
//{
//	std::cerr << "Receiving POSITION data type." << std::endl;

//	// Create a message buffer to receive transform data
//	igtl::PositionMessage::Pointer positionMsg;
//	positionMsg = igtl::PositionMessage::New();
//	positionMsg->SetMessageHeader(header);
//	positionMsg->AllocatePack();

//	// Receive position position data from the socket
//	socket->Receive(positionMsg->GetPackBodyPointer(), positionMsg->GetPackBodySize());
//	// Deserialize the transform data
//	// If you want to do a CRC check, call Unpack(1).
//	// If you want to skip CRC check, call Unpack() without argument.
//	int c = positionMsg->Unpack();

//	if (c & (igtl::MessageHeader::UNPACK_BODY | igtl::MessageHeader::UNPACK_UNDEF)) // if CRC check is OK or skipped
//	{
//		// Retrive the transform data
//		float position[3];
//		float quaternion[4];

//		positionMsg->GetPosition(position);
//		positionMsg->GetQuaternion(quaternion);

//		std::cerr << "position   = (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
//		std::cerr << "quaternion = (" << quaternion[0] << ", " << quaternion[1] << ", " << quaternion[2] << ", "
//						<< quaternion[3] << ")" << std::endl << std::endl;

//		return 1;
//	}

//	return 0;
//}

//int ReceiveStatus(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
//{

//	std::cerr << "Receiving STATUS data type." << std::endl;

//	// Create a message buffer to receive transform data
//	igtl::StatusMessage::Pointer statusMsg;
//	statusMsg = igtl::StatusMessage::New();
//	statusMsg->SetMessageHeader(header);
//	statusMsg->AllocatePack();

//	// Receive transform data from the socket
//	socket->Receive(statusMsg->GetPackBodyPointer(), statusMsg->GetPackBodySize());
//	// Deserialize the transform data
//	// If you want to do a CRC check, call Unpack(1).
//	// If you want to skip CRC check, call Unpack() without argument.
//	int c = statusMsg->Unpack();

//	if (c & (igtl::MessageHeader::UNPACK_BODY | igtl::MessageHeader::UNPACK_UNDEF)) // if CRC check is OK or skipped
//	{
//		std::cerr << "========== STATUS ==========" << std::endl;
//		std::cerr << " Code      : " << statusMsg->GetCode() << std::endl;
//		std::cerr << " SubCode   : " << statusMsg->GetSubCode() << std::endl;
//		std::cerr << " Error Name: " << statusMsg->GetErrorName() << std::endl;
//		std::cerr << " Status    : " << statusMsg->GetStatusString() << std::endl;
//		std::cerr << "============================" << std::endl;
//	}

//	return 0;

//}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

namespace cx
{

GrabberReceiveThreadIGTLink::GrabberReceiveThreadIGTLink(QString address, int port, QObject* parent) :
		GrabberReceiveThread(parent), mHeadingReceived(false), mAddress(address), mPort(port)
{
//  std::cout << "client::create thread: " << QThread::currentThread() << std::endl;
}

void GrabberReceiveThreadIGTLink::run()
{
//  std::cout << "client::run thread: " << QThread::currentThread() << std::endl;
	//std::cout << "run client thread, connecting to " << mAddress << " " << mPort << std::endl;

	//------------------------------------------------------------
	// Establish Connection
	mSocket = new QTcpSocket();
	connect(mSocket, SIGNAL(readyRead()), this, SLOT(readyReadSlot()), Qt::DirectConnection);
	connect(mSocket, SIGNAL(hostFound()), this, SLOT(hostFoundSlot()), Qt::DirectConnection);
	connect(mSocket, SIGNAL(connected()), this, SLOT(connectedSlot()), Qt::DirectConnection);
	connect(mSocket, SIGNAL(disconnected()), this, SLOT(disconnectedSlot()), Qt::DirectConnection);
	connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorSlot(QAbstractSocket::SocketError)),
					Qt::DirectConnection);

	ssc::messageManager()->sendInfo("Looking for host: " + this->hostDescription());
	mSocket->connectToHost(mAddress, mPort);

	int timeout = 5000;
	if (!mSocket->waitForConnected(timeout))
	{
		ssc::messageManager()->sendWarning("Timeout looking for host " + this->hostDescription());
		mSocket->disconnectFromHost();
		return;
	}

	// Create a message buffer to receive header
	mHeaderMsg = igtl::MessageHeader::New();

	mFPSTimer->reset(2000);

	// run event loop
	this->exec();

	mSocket->disconnectFromHost();
//  std::cout << "finished openIGTLink client thread" << std::endl;
	delete mSocket;
}

QString GrabberReceiveThreadIGTLink::hostDescription() const
{
	return mAddress + ":" + qstring_cast(mPort);
}

void GrabberReceiveThreadIGTLink::hostFoundSlot()
{
	ssc::messageManager()->sendInfo("Host found: " + this->hostDescription());
}
void GrabberReceiveThreadIGTLink::connectedSlot()
{
	ssc::messageManager()->sendSuccess("Connected to host " + this->hostDescription());
	emit connected(true);
}
void GrabberReceiveThreadIGTLink::disconnectedSlot()
{
	ssc::messageManager()->sendInfo("Disconnected to host " + this->hostDescription());
	emit connected(false);
}
void GrabberReceiveThreadIGTLink::errorSlot(QAbstractSocket::SocketError socketError)
{
	ssc::messageManager()->sendError(
					"Socket error [Host=" + this->hostDescription() + ", Code=" + socketError + "]\n"
									+ mSocket->errorString());
}

void GrabberReceiveThreadIGTLink::readyReadSlot()
{
	// read messages until one fails
	while (this->readOneMessage());

//	readOneMessage();
}

/**Read one IGTLink message from the socket.
 * Return false if there was not enough data to
 * read the entire message.
 */
bool GrabberReceiveThreadIGTLink::readOneMessage()
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

bool GrabberReceiveThreadIGTLink::ReceiveSonixStatus(QTcpSocket* socket, igtl::MessageHeader::Pointer& header)
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

bool GrabberReceiveThreadIGTLink::ReceiveImage(QTcpSocket* socket, igtl::MessageHeader::Pointer& header)
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

void GrabberReceiveThreadIGTLink::addToQueue(IGTLinkUSStatusMessage::Pointer msg)
{
	// set temporary, then assume the image adder will pass this message on.
	mUnsentUSStatusMessage = msg;
}

void GrabberReceiveThreadIGTLink::addToQueue(IGTLinkImageMessage::Pointer msg)
{
	IGTLinkConversion converter;
	this->addImageToQueue(converter.decode(msg));

	// if us status not sent, do it here
	if (mUnsentUSStatusMessage)
	{
		ssc::ProbeData probeData;
		this->addSonixStatusToQueue(converter.decode(mUnsentUSStatusMessage, msg, probeData));
		mUnsentUSStatusMessage = IGTLinkUSStatusMessage::Pointer();
	}
}

} //end namespace cx
