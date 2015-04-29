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
#include "igtlTransformMessage.h"
#include "igtlImageMessage.h"
#include "cxLogger.h"
#include "cxTime.h"

#include "cxIGTLinkConversion.h"

#define CX_OPENIGTLINK_CHANNEL_NAME "OpenIGTLink"

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
}

void OpenIGTLinkClient::setIpAndPort(QString ip, int port)
{
    mIp = ip;
    mPort = port;
}

void OpenIGTLinkClient::requestConnect()
{
    CX_LOG_INFO() << "Trying to connect to " << mIp << ":" << mPort;
    mSocket->requestConnectToHost(mIp, mPort);
}

void OpenIGTLinkClient::requestDisconnect()
{
    mSocket->requestCloseConnection();
}

void OpenIGTLinkClient::internalConnected()
{
    emit connected();
}

void OpenIGTLinkClient::internalDisconnected()
{
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
        if(!this->receiveTransform(header))
            return false;
    }
    else if(strcmp(header->GetDeviceType(), "IMAGE") == 0)
    {
        if(!this->receiveImage(header))
            return false;
    }
    else if(strcmp(header->GetDeviceType(), "STATUS") == 0)
    {
        if(!this->receiveStatus(header))
            return false;
    }
    else if(strcmp(header->GetDeviceType(), "STRING") == 0)
    {
        if(!this->receiveString(header))
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

bool OpenIGTLinkClient::receiveTransform(const igtl::MessageBase::Pointer header)
{
    igtl::TransformMessage::Pointer body = igtl::TransformMessage::New();
    body->SetMessageHeader(header);
    body->AllocatePack();

    if(!this->socketReceive(body->GetPackBodyPointer(), body->GetPackBodySize()))
        return false;

    int c = body->Unpack(1);
    this->checkCRC(c);
    if (c & igtl::MessageHeader::UNPACK_BODY)
    {
        // if CRC check is OK. Read transform data.
        QString deviceName = body->GetDeviceName();

        IGTLinkConversion converter;
        Transform3D transform3D = converter.decode(body);

        igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();
        body->GetTimeStamp(ts);
        double timestamp_ms = ts->GetTimeStamp()*1000; //since epoch

        emit transform(deviceName, transform3D, timestamp_ms);
    }
    else
    {
        CX_LOG_CHANNEL_ERROR(CX_OPENIGTLINK_CHANNEL_NAME) << "Could not unpack the body (Transform).";
        return false;
    }
    return true;
}

bool OpenIGTLinkClient::receiveImage(const igtl::MessageBase::Pointer header)
{
    igtl::ImageMessage::Pointer body = igtl::ImageMessage::New();
    body->SetMessageHeader(header);
    body->AllocatePack();

    if(!this->socketReceive(body->GetPackBodyPointer(), body->GetPackBodySize()))
        return false;

    int c = body->Unpack(1);
    this->checkCRC(c);
    if (c & (igtl::MessageHeader::UNPACK_HEADER | igtl::MessageHeader::UNPACK_BODY))
    {
        IGTLinkConversion converter;
        ImagePtr theImage = converter.decode(body);
        emit image(theImage);
    }
    return true;
}

bool OpenIGTLinkClient::receiveStatus(const igtl::MessageBase::Pointer header)
{
    igtl::StatusMessage::Pointer body = igtl::StatusMessage::New();
    body->SetMessageHeader(header);
    body->AllocatePack();

    if(!this->socketReceive(body->GetPackBodyPointer(), body->GetPackBodySize()))
        return false;

    int c = body->Unpack(1);
    this->checkCRC(c);
    if (c & igtl::MessageHeader::UNPACK_BODY)
    {
        IGTLinkConversion converter;
        QString status = converter.decode(body);
        CX_LOG_CHANNEL_INFO(CX_OPENIGTLINK_CHANNEL_NAME) << status;
    }
    else
    {
        CX_LOG_CHANNEL_ERROR(CX_OPENIGTLINK_CHANNEL_NAME) << "Could not unpack the body (Status).";
        return false;
    }
    return true;
}

bool OpenIGTLinkClient::receiveString(const igtl::MessageBase::Pointer header)
{
    igtl::StringMessage::Pointer body = igtl::StringMessage::New();
    body->SetMessageHeader(header);
    body->AllocatePack();

    if(!this->socketReceive(body->GetPackBodyPointer(), body->GetPackBodySize()))
        return false;

    int c = body->Unpack(1);
    this->checkCRC(c);
    if (c & igtl::MessageHeader::UNPACK_BODY)
    {
        IGTLinkConversion converter;
        QString string = converter.decode(body);
        CX_LOG_CHANNEL_INFO(CX_OPENIGTLINK_CHANNEL_NAME) << string;
    }
    else
    {
        CX_LOG_CHANNEL_ERROR(CX_OPENIGTLINK_CHANNEL_NAME) << "Could not unpack the body (String).";
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
        CX_LOG_ERROR() << "Error when receiving data from socket.";
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
