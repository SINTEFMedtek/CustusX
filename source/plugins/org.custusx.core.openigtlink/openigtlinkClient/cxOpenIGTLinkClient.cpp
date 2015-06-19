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
#include "igtl_header.h"

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
    qRegisterMetaType<ProbeDefinitionPtr>("ProbeDefinitionPtr");

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
    if(mDialect && (dialectname == mDialect->getName()))
        return;

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
        disconnect(mDialect.get(), &Dialect::probedefinition, this, &OpenIGTLinkClient::probedefinition);
        disconnect(mDialect.get(), &Dialect::usstatusmessage, this, &OpenIGTLinkClient::usstatusmessage);
        disconnect(mDialect.get(), &Dialect::igtlimage, this, &OpenIGTLinkClient::igtlimage);
    }

    mDialect = dialect;
    connect(dialect.get(), &Dialect::image, this, &OpenIGTLinkClient::image);
    connect(dialect.get(), &Dialect::transform, this, &OpenIGTLinkClient::transform);
    connect(dialect.get(), &Dialect::calibration, this, &OpenIGTLinkClient::calibration);
    connect(dialect.get(), &Dialect::probedefinition, this, &OpenIGTLinkClient::probedefinition);
    connect(dialect.get(), &Dialect::usstatusmessage, this, &OpenIGTLinkClient::usstatusmessage);
    connect(dialect.get(), &Dialect::igtlimage, this, &OpenIGTLinkClient::igtlimage);

    CX_LOG_CHANNEL_SUCCESS(CX_OPENIGTLINK_CHANNEL_NAME) << "Dialect set to " << dialectname;

}

/*
void OpenIGTLinkClient::sendStringMessage(QString command)
{
    QMutexLocker locker(&mMutex);

    OpenIGTLinkConversion converter;
    igtl::StringMessage::Pointer stringMsg = converter.encode(command);
    CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << "Sending string: " << command;
    stringMsg->Pack();

    mSocket->write(reinterpret_cast<char*>(stringMsg->GetPackPointer()), stringMsg->GetPackSize());
}
*/

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

/*
void OpenIGTLinkClient::queryServer()
{
    QString command = "<Command Name=\"RequestChannelIds\" />";
    this->sendStringMessage(command);

    command = "<Command Name=\"RequestDeviceIds\" />";
    this->sendStringMessage(command);

    command = "<Command Name=\"SaveConfig\" />";
    this->sendStringMessage(command);
}
*/

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
    const char* type = header->GetDeviceType();
    if(strcmp(type, "TRANSFORM") == 0)
    {
        if(!this->receive<igtl::TransformMessage>(header))
            return false;
    }
    else if(strcmp(type, "IMAGE") == 0)
    {
        //----- CustusX openigtlink server -----
        //there is a special kind of image package coming from custusx
        //server where crc is set to 0.
        QString name(header->GetDeviceName());
        if(name.contains("Sonix", Qt::CaseInsensitive))
        {
            if(!this->receive<IGTLinkImageMessage>(header))
                return false;
        }
        //----------
        else
        {
            if(!this->receive<igtl::ImageMessage>(header))
                return false;
        }
    }
    else if(strcmp(type, "STATUS") == 0)
    {
        if(!this->receive<igtl::StatusMessage>(header))
            return false;
    }
    else if(strcmp(type, "STRING") == 0)
    {
        if(!this->receive<igtl::StringMessage>(header))
            return false;
    }
    else if(strcmp(type, "CX_US_ST") == 0)
    {
        if(!this->receive<IGTLinkUSStatusMessage>(header))
            return false;
    }
    else
    {
        CX_LOG_CHANNEL_WARNING(CX_OPENIGTLINK_CHANNEL_NAME) << "Skipping message of type " << type;
        igtl::MessageBase::Pointer body = igtl::MessageBase::New();
        body->SetMessageHeader(header);
        mSocket->skip(body->GetBodySizeToRead());
    }
    return true;
}

template <typename T>
bool OpenIGTLinkClient::receive(const igtl::MessageBase::Pointer header)
{
    QMutexLocker locker(&mMutex);

    typename T::Pointer body = T::New();
    body->SetMessageHeader(header);
    body->AllocatePack();

    if(!this->socketReceive(body->GetPackBodyPointer(), body->GetPackBodySize()))
        return false;

    int c = body->Unpack(mDialect->doCRC());
    this->checkCRC(c);
    if (c & igtl::MessageHeader::UNPACK_BODY)
    {
        mDialect->translate(body);
    }
    else
    {
        CX_LOG_CHANNEL_ERROR(CX_OPENIGTLINK_CHANNEL_NAME) << "Could not unpack the body of type: " << body->GetDeviceType();
        return false;
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
