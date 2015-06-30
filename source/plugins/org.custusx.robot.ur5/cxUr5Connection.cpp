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

#include "cxUr5Connection.h"
#include <QTcpSocket>
#include <iostream>
#include <sstream>

#include "cxLogger.h"
#include "cxUtilHelpers.h"


namespace cx
{

Ur5Connection::Ur5Connection(QString address, int port)
{
    mIp = address;
    mPort = port;
}

void Ur5Connection::setAddress(QString address, int port)
{
    mIp = address;
    mPort = port;
}

bool Ur5Connection::isConnectedToRobot()
{
    return (mSocket && mSocket->isConnected());
}

bool Ur5Connection::sendMessage(QString message)
{
    message.append('\n');
    bool ok = this->sendData(message.toStdString().c_str(), message.size());
    if(!ok)
        return false;

    return mSocket->waitForBytesWritten(3000);
}

bool Ur5Connection::waitForMessage()
{
    mSocket->waitForBytesWritten(3000);
    return mSocket->waitForReadyRead(5000);
}

void Ur5Connection::internalDataAvailable()
{
    if(!this->socketIsConnected())
        return;

    int headerSize = 6;

    // How many bytes?
    qint64 maxAvailableBytes = mSocket->bytesAvailable();

    CX_LOG_INFO() << "SocketConnection incoming message: " << maxAvailableBytes << " bytes";

    if(maxAvailableBytes<headerSize)
        return;

    //char* inMessage = new char [maxAvailableBytes];
    unsigned char* header = new unsigned char [headerSize];
    if(!this->socketReceive(header, headerSize))
        return;
    CX_LOG_INFO() << "SocketConnection incoming message 1 char: " << (int)(header[0]) << " " << (int)header[1] << " " << (int)header[2] << " " << (int)header[3] << " " << (int)header[4] << " " << (int)header[5];

    std::stringstream tmpStream;
    tmpStream << header;

    bool ok = true;
    int msgType;
    tmpStream >> msgType;

    if(!ok)
        CX_LOG_INFO() << "Conversion failed";
    else
        CX_LOG_INFO() << "SocketConnection incoming message: " << msgType;


    //TODO: Do something with received message
}


} // cx
