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
#include <iostream>
#include <sstream>
#include <QFile>
#include <set>
#include <QTcpSocket>

#include "cxLogger.h"
#include "cxUtilHelpers.h"

namespace cx
{

Ur5Connection::Ur5Connection()
{
}

Ur5Connection::~Ur5Connection()
{
}

void Ur5Connection::setProtocol(QString protocolName)
{

}

void Ur5Connection::setAddress(QString address, int port)
{
    info.host = address;
    info.port = port;

    setConnectionInfo(info);
}

bool Ur5Connection::isConnectedToRobot()
{
    return (mSocket && mSocket->waitForConnected());
}

bool Ur5Connection::sendMessage(QString message)
{
    message.append('\n');
    bool ok = this->sendData(message.toStdString().c_str(), message.size());
    if(!ok)
        return false;

    return mSocket->waitForBytesWritten(3000);
}

bool Ur5Connection::waitForUpdate()
{
    mSocket->waitForBytesWritten(3000);
    return mSocket->waitForReadyRead(3000);
}

void Ur5Connection::internalDataAvailable()
{
    if(!this->socketIsConnected())
        return;

    if(!isPotentialPacket(mSocket->bytesAvailable()))
    {
        char *voidData = new char[mSocket->bytesAvailable()];
        int retval = mSocket->read(voidData, mSocket->bytesAvailable());
        delete[] voidData;
        return;
    }

    mPreviousState = mCurrentState;
    updateCurrentState(mSocket->read(mSocket->bytesAvailable()));
}

void Ur5Connection::updateCurrentState(QByteArray rawData)
{
    mCurrentState = mMessageDecoder.analyzeRawPacket(rawData);
    if(mCurrentState.updated == false)
        mCurrentState = mPreviousState;

    emit(stateChanged());
}

void Ur5Connection::clearCurrentTCP()
{
    Ur5State clearState(0,0,0,0,0,0);
    sendMessage(mMessageEncoder.set_tcp(clearState));
    waitForUpdate();
}

bool Ur5Connection::isPotentialPacket(qint64 bytes)
{
    return (bytes == 1460 || bytes == 560 || bytes == 812 || bytes == 1624 || bytes ==1254);
}

Ur5State Ur5Connection::getCurrentState()
{
    return (this->mCurrentState);
}

Ur5State Ur5Connection::getTargetState()
{
    return (this->mCurrentState);
}

Ur5State Ur5Connection::getPreviousState()
{
    return (this->mCurrentState);
}

} // cx
