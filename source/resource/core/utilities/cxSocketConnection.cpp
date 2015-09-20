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

#include "cxSocketConnection.h"
#include "cxLogger.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, CX_SOCKETCONNECTION_STATE, scsCOUNT)
{
	"inactive",
	"connected",
	"listening",
	"connecting"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, CX_SOCKETCONNECTION_STATE, scsCOUNT)

namespace cx {

SocketConnection::SocketConnection(QObject *parent) :
	QObject(parent)
{
	qRegisterMetaType<boost::function<void()> >("boost::function<void()>");
	qRegisterMetaType<CX_SOCKETCONNECTION_STATE>("CX_SOCKETCONNECTION_STATE");

	mConnectionInfo.host = "localhost";
	mConnectionInfo.port = 18944;

	mSocket = new Socket(this);
	connect(mSocket, &Socket::connected, this, &SocketConnection::internalConnected);
	connect(mSocket, &Socket::disconnected, this, &SocketConnection::internalDisconnected);
	connect(mSocket, &Socket::readyRead, this, &SocketConnection::internalDataAvailable);
	connect(mSocket, &Socket::error, this, &SocketConnection::error);
	connect(mSocket, &Socket::error, this, &SocketConnection::internalError);
}

void SocketConnection::setConnectionInfo(ConnectionInfo info)
{
	mConnectionInfo = info;
}

void SocketConnection::requestConnect()
{
	CX_LOG_CHANNEL_DEBUG("CA") << "RequestConnect";
	if (mConnectionInfo.isClient())
	{
		CX_LOG_INFO() << "Trying to connect to " << mConnectionInfo.getDescription();
		emit stateChanged(scsCONNECTING);
		mSocket->requestConnectToHost(mConnectionInfo.host, mConnectionInfo.port);
	}
	else
	{
		this->startListen();
	}
}

//void SocketConnection::tryConnectAndWait()
//{
//    CX_LOG_INFO() << "Trying to connect to " << mIp << ":" << mPort;
//    mSocket->tryConnectToHostAndWait(mIp, mPort);
//}

void SocketConnection::requestDisconnect()
{
	CX_LOG_INFO("CA") << "RequestDisconnect";

	this->stopListen();
	mSocket->requestCloseConnection();
}

bool SocketConnection::sendData(const char *data, qint64 maxSize)
{
    if(!this->socketIsConnected())
        return false;
    qint64 writtenBytes = mSocket->write(data, maxSize);
    if(writtenBytes != maxSize)
        return false;
    return true;
}

void SocketConnection::internalConnected()
{
	CX_LOG_SUCCESS() << "Connected to "  << mConnectionInfo.getDescription();
	emit stateChanged(scsCONNECTED);
	emit connected();
}

void SocketConnection::internalDisconnected()
{
    CX_LOG_SUCCESS() << "Disconnected";
	emit stateChanged(scsINACTIVE);
	emit disconnected();
}

void SocketConnection::internalError()
{
	CX_LOG_INFO() << "Error";
	emit stateChanged(scsINACTIVE);
}

void SocketConnection::internalDataAvailable()
{
    if(!this->socketIsConnected())
        return;

	// How many bytes?
	qint64 maxAvailableBytes = mSocket->bytesAvailable();

    char* inMessage = new char [maxAvailableBytes];
    if(!this->socketReceive(inMessage, maxAvailableBytes))
        return;

    CX_LOG_INFO() << "SocketConnection incoming message: " << QString(inMessage);

    //TODO: Do something with received message
}

bool SocketConnection::socketIsConnected()
{
    return mSocket->isConnected();
}

bool SocketConnection::enoughBytesAvailableOnSocket(int bytes) const
{
    bool retval = mSocket->minBytesAvailable(bytes);
    return retval;
}

bool SocketConnection::socketReceive(void *packPointer, int packSize) const
{
    if(!this->enoughBytesAvailableOnSocket(packSize))
        return false;

    int r = mSocket->read(reinterpret_cast<char*>(packPointer), packSize);
    if(r <= 0)
    {
        CX_LOG_ERROR() << "Error when receiving data from socket.";
        return false;
    }
    return true;
}


bool SocketConnection::startListen()
{
	if (!mServer)
		mServer = new QTcpServer(this);
	emit stateChanged(scsCONNECTING);

//	CX_LOG_CHANNEL_DEBUG("CA") << "set port " << mConnectionInfo.port;
	bool started = mServer->listen(QHostAddress::Any, mConnectionInfo.port);

	if (started)
	{
//		this->getAllServerHostnames();
		CX_LOG_INFO() << QString("Server address: %1").arg(this->getAllServerHostnames().join(", "));
		CX_LOG_INFO() << QString("Server is listening to port %1").arg(mServer->serverPort());
	}
	else
	{
		CX_LOG_INFO() << QString("Server failed to start. Error: ").arg(mServer->errorString());
	}

	emit stateChanged(scsLISTENING);
	return started;
}

void SocketConnection::stopListen()
{
	if (mServer && mServer->isListening())
	{
		mServer->close();
		if (!mSocket->isConnected())
		{
			emit stateChanged(scsINACTIVE);
		}
	}
}

void SocketConnection::incomingConnection(qintptr socketDescriptor)
{
	CX_LOG_INFO() << "Server: Incoming connection...";

	if (this->socketIsConnected())
	{
		reportError("Incoming connection request rejected: The server can only handle a single connection.");
		return;
	}

//	connect(mSocket, SIGNAL(disconnected()), this, SLOT(socketDisconnectedSlot()));
	mSocket->getSocket()->setSocketDescriptor(socketDescriptor);
	QString clientName = mSocket->getSocket()->localAddress().toString();
	report("Connected to "+clientName+". Session started.");
//	SenderPtr sender(new GrabberSenderQTcpSocket(mSocket));

//	mImageSender->startStreaming(sender);
	emit connected();
	emit stateChanged(scsCONNECTED);
}

QStringList SocketConnection::getAllServerHostnames()
{
//	CX_LOG_INFO() <<  "Server IP adresses: ";
	QStringList addresses;

	foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces())
	{
		if (interface.flags().testFlag(QNetworkInterface::IsRunning))
			foreach (QNetworkAddressEntry entry, interface.addressEntries())
			{
				if ( interface.hardwareAddress() != "00:00:00:00:00:00"
					 && entry.ip().toString() != "127.0.0.1"
					 && entry.ip().toString().contains(".") )
					addresses << QString("%1: %2").arg(interface.name()).arg(entry.ip().toString());
//					CX_LOG_INFO() << interface.name() << " " << entry.ip().toString();
			}
	}

	return addresses;
}

} //namespace cx
