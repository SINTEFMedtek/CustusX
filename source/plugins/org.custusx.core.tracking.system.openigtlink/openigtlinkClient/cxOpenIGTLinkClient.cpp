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

#include "cxOpenIGTLinkClient.h"

#include "cxSender.h"
#include "cxTime.h"

#include "cxPlusDialect.h"
#include "cxCustusDialect.h"

namespace cx
{

OpenIGTLinkClient::OpenIGTLinkClient(QObject *parent) :
    QObject(parent),
    mHeader(igtl::MessageHeader::New()),
    mHeaderReceived(false),
    mIp("localhost"),
    mPort(18944)
{
    qRegisterMetaType<Transform3D>("Transform3D");
    qRegisterMetaType<ImagePtr>("ImagePtr");

    mSocket = SocketPtr(new Socket(this));
    //todo: check affinity on socket!!!
    connect(mSocket.get(), &Socket::connected, this, &OpenIGTLinkClient::internalConnected);
    connect(mSocket.get(), &Socket::disconnected, this, &OpenIGTLinkClient::internalDisconnected);
    connect(mSocket.get(), &Socket::readyRead, this, &OpenIGTLinkClient::internalDataAvailable);
    connect(mSocket.get(), &Socket::error, this, &OpenIGTLinkClient::error);

    DialectPtr dialect = DialectPtr(new CustusDialect());
    mAvailableDialects[dialect->getName()] = dialect;
    this->setDialect(dialect->getName());

    dialect = DialectPtr(new PlusDialect());
    mAvailableDialects[dialect->getName()] = dialect;

    dialect = DialectPtr(new Dialect());
    mAvailableDialects[dialect->getName()] = dialect;

}

QStringList OpenIGTLinkClient::getAvailableDialects() const
{
    QStringList retval;
    DialectMap::const_iterator it = mAvailableDialects.begin();
    for( ; it!=mAvailableDialects.end() ; ++it)
    {
        retval << it->first;
    }
    return retval;
}

void OpenIGTLinkClient::setDialect(QString dialectname)
{
    QMutexLocker locker(&mMutex);

    DialectPtr dialect = mAvailableDialects[dialectname];
    if(!dialect)
    {
        CX_LOG_ERROR() << "\"" << dialectname << "\" is an unknown opentigtlink dialect.";
        return;
    }

    if(mDialect)
    {
        disconnect(mDialect.get(), &Dialect::image, this, &OpenIGTLinkClient::image);
        disconnect(mDialect.get(), &Dialect::transform, this, &OpenIGTLinkClient::transform);
        disconnect(mDialect.get(), &Dialect::calibration, this, &OpenIGTLinkClient::calibration);
    }

    mDialect = dialect;
    connect(dialect.get(), &Dialect::image, this, &OpenIGTLinkClient::image);
    connect(dialect.get(), &Dialect::transform, this, &OpenIGTLinkClient::transform);
    connect(dialect.get(), &Dialect::calibration, this, &OpenIGTLinkClient::calibration);

    CX_LOG_CHANNEL_SUCCESS(CX_OPENIGTLINK_CHANNEL_NAME) << "Dialect set to " << dialectname;

}

void OpenIGTLinkClient::sendStringMessage(QString command)
{
    QMutexLocker locker(&mMutex);

    //TODO move to the converter
    igtl::StringMessage::Pointer stringMsg;
    stringMsg = igtl::StringMessage::New();
    stringMsg->SetDeviceName("CustusXQuery");

    CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << "Sending string: " << command;
    stringMsg->SetString(command.toStdString());
    stringMsg->Pack();

    mSocket->write(reinterpret_cast<char*>(stringMsg->GetPackPointer()), stringMsg->GetPackSize());
}

void OpenIGTLinkClient::setIpAndPort(QString ip, int port)
{
    mIp = ip;
    mPort = port;
}

void OpenIGTLinkClient::requestConnect()
{
    CX_LOG_CHANNEL_INFO(CX_OPENIGTLINK_CHANNEL_NAME) << "Trying to connect to " << mIp << ":" << mPort;
    mSocket->requestConnectToHost(mIp, mPort);
}

void OpenIGTLinkClient::requestDisconnect()
{
    mSocket->requestCloseConnection();
}

void OpenIGTLinkClient::internalConnected()
{
    CX_LOG_CHANNEL_SUCCESS(CX_OPENIGTLINK_CHANNEL_NAME) << "Connected to "  << mIp << ":" << mPort;
    if(mDialect->getName() == "PlusServer")
        this->queryServer();
    emit connected();
}

void OpenIGTLinkClient::internalDisconnected()
{
    CX_LOG_CHANNEL_SUCCESS(CX_OPENIGTLINK_CHANNEL_NAME) << "Disconnected";
    emit disconnected();
}

void OpenIGTLinkClient::internalDataAvailable()
{
    if(!this->socketIsConnected())
        return;

    bool done = false;
    while(!done)
    {
        if(!mHeaderReceived)
        {
            if(!this->receiveHeader(mHeader))
                done = true;
            else
                mHeaderReceived = true;
        }

        if(mHeaderReceived)
        {
            if(!this->receiveBody(mHeader))
                done = true;
            else
                mHeaderReceived = false;
        }
    }
}

void OpenIGTLinkClient::queryServer()
{
    QString command = "<Command Name=\"RequestChannelIds\" />";
    this->sendStringMessage(command);

    command = "<Command Name=\"RequestDeviceIds\" />";
    this->sendStringMessage(command);

    command = "<Command Name=\"SaveConfig\" />";
    this->sendStringMessage(command);
}

bool OpenIGTLinkClient::socketIsConnected()
{
    return mSocket->isConnected();
}

bool OpenIGTLinkClient::enoughBytesAvailableOnSocket(int bytes) const
{
    bool retval = mSocket->minBytesAvailable(bytes);
    return retval;
}

bool OpenIGTLinkClient::receiveHeader(const igtl::MessageHeader::Pointer header) const
{
    header->InitPack();

    if(!this->socketReceive(header->GetPackPointer(), header->GetPackSize()))
        return false;

    int c = header->Unpack(1);
    this->checkCRC(c); //just for debugging
    if (c & igtl::MessageHeader::UNPACK_HEADER)
    {
        std::string deviceType = std::string(header->GetDeviceType());
        return true;
    }
    else
        return false;
}

bool OpenIGTLinkClient::receiveBody(const igtl::MessageBase::Pointer header)
{
    if(strcmp(header->GetDeviceType(), "TRANSFORM") == 0)
    {
        if(!this->receive<igtl::TransformMessage>(header))
            return false;
    }
    else if(strcmp(header->GetDeviceType(), "IMAGE") == 0)
    {
        if(!this->receive<igtl::ImageMessage>(header))
            return false;
    }
    else if(strcmp(header->GetDeviceType(), "STATUS") == 0)
    {
        if(!this->receive<igtl::StatusMessage>(header))
            return false;
    }
    else if(strcmp(header->GetDeviceType(), "STRING") == 0)
    {
        if(!this->receive<igtl::StringMessage>(header))
            return false;
    }
    else
    {
        igtl::MessageBase::Pointer body = igtl::MessageBase::New();
        body->SetMessageHeader(header);
        mSocket->skip(body->GetBodySizeToRead());
    }
    return true;
}

bool OpenIGTLinkClient::socketReceive(void *packPointer, int packSize) const
{
    if(!this->enoughBytesAvailableOnSocket(packSize))
        return false;

    int r = mSocket->read(reinterpret_cast<char*>(packPointer), packSize);
    if(r <= 0)
    {
        CX_LOG_CHANNEL_ERROR(CX_OPENIGTLINK_CHANNEL_NAME) << "Error when receiving data from socket.";
        return false;
    }
    return true;
}

void OpenIGTLinkClient::checkCRC(int c) const
{
    switch(c)
    {
        case igtl::MessageHeader::UNPACK_UNDEF:
            //CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << "UNPACK_UNDEF";
            break;
        case igtl::MessageHeader::UNPACK_HEADER:
            //CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << "UNPACK_HEADER";
            break;
        case igtl::MessageHeader::UNPACK_BODY:
            //CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << "UNPACK_BODY";
            break;
        case igtl::MessageHeader::UNPACK_HEADER|igtl::MessageHeader::UNPACK_BODY:
            //CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << "UNPACK_HEADER|UNPACK_BODY";
            break;
        default:
            //CX_LOG_DEBUG() << "default: " << c;
            break;
    }
}


}//namespace cx
