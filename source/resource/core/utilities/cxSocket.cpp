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


#include "cxSocket.h"

#include <QTcpSocket>
#include "cxLogger.h"

namespace cx
{

Socket::Socket(QObject *parent) :
    QObject(parent),
    mConnected(false)
{
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");

	mSocket = new QTcpSocket(this);
	connect(mSocket, &QTcpSocket::connected, this, &Socket::receivedConnected);
	connect(mSocket, &QTcpSocket::disconnected, this, &Socket::receivedDisconnected);
    //see http://stackoverflow.com/questions/26062397/qt-connect-function-signal-disambiguation-using-lambdas
    void (QTcpSocket::* errorOverloaded)(QAbstractSocket::SocketError) = &QTcpSocket::error;
	connect(mSocket, errorOverloaded, this, &Socket::receivedError);
	connect(mSocket, &QTcpSocket::hostFound, this, &Socket::receivedHostFound);
	connect(mSocket, &QTcpSocket::stateChanged, this, &Socket::receivedStateChanged);
	connect(mSocket, &QTcpSocket::readyRead, this, &Socket::receiveReadyRead);
	connect(mSocket, &QTcpSocket::bytesWritten, this, &Socket::receiveBytesWritten);
	connect(mSocket, &QTcpSocket::aboutToClose, this, &Socket::receiveAboutToClose);
}

void Socket::requestConnectToHost(QString ip, int port) const
{
    mSocket->connectToHost(ip, port);
}

bool Socket::tryConnectToHostAndWait(QString ip, int port) const
{
	this->requestConnectToHost(ip, port);

	int timeout = 5000;
	if (!mSocket->waitForConnected(timeout))
	{
		mSocket->disconnectFromHost();
		return false;
	}
	return true;
}

bool Socket::isConnected() const
{
    return mConnected;
}

QString Socket::getLastError() const
{
    return mSocket->errorString();
}

void Socket::requestCloseConnection() const
{
    mSocket->close();
}

bool Socket::minBytesAvailable(int bytes) const
{
    return mSocket->bytesAvailable() >= bytes;
}

qint64 Socket::bytesAvailable() const
{
    return mSocket->bytesAvailable();
}

qint64 Socket::read(char *data, qint64 maxSizeBytes) const
{
    qint64 readBytes = mSocket->read(data, maxSizeBytes);
    return readBytes;
}

qint64 Socket::skip(qint64 maxSizeBytes) const
{
    char *voidData = new char[maxSizeBytes];
    int retval = mSocket->read(voidData, maxSizeBytes);
    delete[] voidData;
    return retval;
}

qint64 Socket::write(const char *data, qint64 maxSizeBytes) const
{
    qint64 writtenBytes = mSocket->write(data, maxSizeBytes);
    return writtenBytes;
}

bool Socket::waitForBytesWritten(int msecs)
{
    return mSocket->waitForBytesWritten(msecs);
}

bool Socket::waitForReadyRead(int msecs)
{
    return mSocket->waitForReadyRead(msecs);
}

void Socket::receivedConnected()
{
    mConnected = true;
    emit connected();
}

void Socket::receivedDisconnected()
{
    mConnected = false;
    emit disconnected();
}

void Socket::receivedError(QAbstractSocket::SocketError socketError)
{
    CX_LOG_ERROR() << QString("[%1] Socket error [code=%2]: %3")
                       .arg(mSocket->peerName())
                       .arg(QString::number(socketError))
                       .arg(mSocket->errorString());
    emit error();
}

void Socket::receivedHostFound()
{
    //CX_LOG_DEBUG() << "Socket found host.";
}

QString socketStateToString(QAbstractSocket::SocketState socketState)
{
    QString retval;
    switch(socketState)
    {
        case QAbstractSocket::UnconnectedState:
            retval = "UnconnectedState";
            break;
        case QAbstractSocket::HostLookupState:
            retval = "HostLookupState";
            break;
        case QAbstractSocket::ConnectingState:
            retval = "ConnectingState";
            break;
        case QAbstractSocket::ConnectedState:
            retval = "ConnectedState";
            break;
        case QAbstractSocket::BoundState:
            retval = "BoundState";
            break;
        case QAbstractSocket::ListeningState:
            retval = "ListeningState";
            break;
        case QAbstractSocket::ClosingState:
            retval = "ClosingState";
            break;
        default:
            retval = "Unknown";
    }
    return retval;
}

void Socket::receivedStateChanged(QAbstractSocket::SocketState socketState)
{
    //CX_LOG_DEBUG() <<  "Socket is now in state " << socketStateToString(socketState);
}

void Socket::receiveReadyRead()
{
    emit readyRead();
}

void Socket::receiveBytesWritten(qint64 bytes)
{
    //CX_LOG_DEBUG() << "Bytes written to socket: " << bytes;
}

void Socket::receiveAboutToClose()
{
    //CX_LOG_DEBUG() << "Socket is about to close";
}
}//namespace cx
