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

#include "cxIGTLinkConversion.h"

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
    //check affinity on socket!!!
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
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "trying to connect to " << mIp << ":" << mPort;
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

    CX_LOG_CHANNEL_DEBUG("janne beate ") << "---------------------------- internalDataAvailable START";
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
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "---------------------------- internalDataAvailable STOP";
}

bool OpenIGTLinkClient::socketIsConnected()
{
    return mSocket->isConnected();
}

bool OpenIGTLinkClient::enoughBytesAvailableOnSocket(int bytes) const
{
    bool retval = mSocket->minBytesAvailable(bytes);
    //if(!retval) //this is ok
    //    CX_LOG_CHANNEL_DEBUG("janne beate ") << "not enought bytes available";
    return retval;
}

bool OpenIGTLinkClient::receiveHeader(const igtl::MessageHeader::Pointer header) const
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "*** HEADER";

    header->InitPack();

    if(!this->socketReceive(header->GetPackPointer(), header->GetPackSize()))
        return false;

    int c = header->Unpack(1);
    this->checkCRC(c);
    if (c & igtl::MessageHeader::UNPACK_HEADER)
    {
        std::string deviceType = std::string(header->GetDeviceType());
        CX_LOG_CHANNEL_DEBUG("janne beate ") << "Received header of type: " << deviceType;

        return true;
    }
    else
        return false;
}

bool OpenIGTLinkClient::receiveBody(const igtl::MessageBase::Pointer header)
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "*** BODY";
    if(strcmp(header->GetDeviceType(), "TRANSFORM") == 0)
    {
        CX_LOG_CHANNEL_DEBUG("janne beate ") << "T";
        if(!this->receiveTransform(header))
            return false;
    }
    else if(strcmp(header->GetDeviceType(), "IMAGE") == 0)
    {
        CX_LOG_CHANNEL_DEBUG("janne beate ") << "I";
        if(!this->receiveImage(header))
            return false;
    }
    else
    {
        CX_LOG_CHANNEL_DEBUG("janne beate ") << "U";
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

    CX_LOG_CHANNEL_DEBUG("janne beate ") << "Going to read TRANSFORM body";
    if(!this->socketReceive(body->GetPackBodyPointer(), body->GetPackBodySize()))
        return false;

    int c = body->Unpack(1);
    this->checkCRC(c);
    if (c & igtl::MessageHeader::UNPACK_BODY)
    {
        // if CRC check is OK. Read transform data.
        igtl::Matrix4x4 matrix;
        body->GetMatrix(matrix);

        QString deviceName = body->GetDeviceName();
        Transform3D transform3D = Transform3D::fromFloatArray(matrix);
        igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();
        body->GetTimeStamp(ts);
        igtlUint64 timestamp = ts->GetTimeStampInNanoseconds(); //since epoc

        emit transform(deviceName, transform3D, timestamp);
    }
    else
    {
        CX_LOG_ERROR() << "Could  not unpack the body.";
        return false;
    }
    return true;
}

bool OpenIGTLinkClient::receiveImage(const igtl::MessageBase::Pointer header)
{
    igtl::ImageMessage::Pointer body = igtl::ImageMessage::New();
    body->SetMessageHeader(header);
    body->AllocatePack();
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "GetBodySizeToRead " << body->GetBodySizeToRead();
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "GetPackSize " << body->GetPackSize();
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "GetPackBodySize " << body->GetPackBodySize();

    CX_LOG_CHANNEL_DEBUG("janne beate ") << "Going to read IMAGE body";
    if(!this->socketReceive(body->GetPackBodyPointer(), body->GetPackBodySize()))
        return false;

    int c = body->Unpack(1);
    this->checkCRC(c);
    if (c & (igtl::MessageHeader::UNPACK_HEADER | igtl::MessageHeader::UNPACK_BODY))
    {
        CX_LOG_CHANNEL_DEBUG("janne beate ") << "Converting and emitting image.";
        IGTLinkConversion converter;
        ImagePtr theImage = converter.decode(body);
        emit image(theImage);
    }
    return true;
}

bool OpenIGTLinkClient::socketReceive(void *packPointer, int packSize) const
{
    if(!this->enoughBytesAvailableOnSocket(packSize))
        return false;

    CX_LOG_CHANNEL_DEBUG("janne beate ") << "socketReceive " << packSize;
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
            CX_LOG_CHANNEL_DEBUG("janne beate ") << "UNPACK_UNDEF";
            break;
        case igtl::MessageHeader::UNPACK_HEADER:
            CX_LOG_CHANNEL_DEBUG("janne beate ") << "UNPACK_HEADER";
            break;
        case igtl::MessageHeader::UNPACK_BODY:
            CX_LOG_CHANNEL_DEBUG("janne beate ") << "UNPACK_BODY";
            break;
        case igtl::MessageHeader::UNPACK_HEADER|igtl::MessageHeader::UNPACK_BODY:
            CX_LOG_CHANNEL_DEBUG("janne beate ") << "UNPACK_HEADER|UNPACK_BODY";
            break;
        default:
            CX_LOG_CHANNEL_DEBUG("janne beate ") << "default: " << c;
            break;
    }
}


}//namespace cx
