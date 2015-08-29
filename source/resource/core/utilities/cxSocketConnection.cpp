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

namespace cx {

SocketConnection::SocketConnection(QObject *parent) :
    QObject(parent),
    mIp("localhost"),
    mPort(0)
{

    mSocket = SocketPtr(new Socket(this));
    //todo: check affinity on socket!!!
    connect(mSocket.get(), &Socket::connected, this, &SocketConnection::internalConnected);
    connect(mSocket.get(), &Socket::disconnected, this, &SocketConnection::internalDisconnected);
    connect(mSocket.get(), &Socket::readyRead, this, &SocketConnection::internalDataAvailable);
    connect(mSocket.get(), &Socket::error, this, &SocketConnection::error);
}


void SocketConnection::setIpAndPort(QString ip, int port)
{
    mIp = ip;
    mPort = port;
}

void SocketConnection::requestConnect()
{
    CX_LOG_INFO() << "Trying to connect to " << mIp << ":" << mPort;
    mSocket->requestConnectToHost(mIp, mPort);
}

void SocketConnection::tryConnectAndWait()
{
    CX_LOG_INFO() << "Trying to connect to " << mIp << ":" << mPort;
    mSocket->tryConnectToHostAndWait(mIp, mPort);
}

void SocketConnection::requestDisconnect()
{
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
    CX_LOG_SUCCESS() << "Connected to "  << mIp << ":" << mPort;
    emit connected();
}

void SocketConnection::internalDisconnected()
{
    CX_LOG_SUCCESS() << "Disconnected";
    emit disconnected();
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


} //namespace cx
