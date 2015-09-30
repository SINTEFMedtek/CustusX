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

#include "cxNetworkConnection.h"

#include "cxSender.h"
#include "cxTime.h"
#include <QThread>

#include "cxPlusProtocol.h"
#include "cxCustusProtocol.h"
#include "cxRASProtocol.h"
#include "igtl_header.h"
#include "cxIGTLinkConversionImage.h"
#include "cxIGTLinkConversionPolyData.h"
#include "cxXmlOptionItem.h"
#include "cxProfile.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "vtkImageData.h"

namespace cx
{

NetworkConnection::NetworkConnection(QString uid, QObject *parent) :
    SocketConnection(parent),
    mHeader(igtl::MessageHeader::New()),
	mHeaderReceived(false),
	mUid(uid)
{
    qRegisterMetaType<Transform3D>("Transform3D");
    qRegisterMetaType<ImagePtr>("ImagePtr");
	qRegisterMetaType<ImagePtr>("MeshPtr");
	qRegisterMetaType<ProbeDefinitionPtr>("ProbeDefinitionPtr");


	ConnectionInfo info = this->getConnectionInfo();

    info.protocol = this->initDialect(ProtocolPtr(new CustusProtocol()))->getName();
    this->initDialect(ProtocolPtr(new PlusProtocol()));
    this->initDialect(ProtocolPtr(new Protocol()));
    this->initDialect(ProtocolPtr(new RASProtocol()));

	SocketConnection::setConnectionInfo(info);
	this->setDialect(info.protocol);
}

NetworkConnection::~NetworkConnection()
{
}

ProtocolPtr NetworkConnection::initDialect(ProtocolPtr value)
{
	mAvailableDialects[value->getName()] = value;
	return value;
}

void NetworkConnection::setConnectionInfo(ConnectionInfo info)
{
	SocketConnection::setConnectionInfo(info);
	this->setDialect(info.protocol);
}

void NetworkConnection::invoke(boost::function<void()> func)
{
	QMetaObject::invokeMethod(this,
							  "onInvoke",
							  Qt::QueuedConnection,
							  Q_ARG(boost::function<void()>, func));
}

void NetworkConnection::onInvoke(boost::function<void()> func)
{
	func();
}

QStringList NetworkConnection::getAvailableDialects() const
{
    QStringList retval;
    DialectMap::const_iterator it = mAvailableDialects.begin();
    for( ; it!=mAvailableDialects.end() ; ++it)
    {
        retval << it->first;
    }
    return retval;
}

void NetworkConnection::setDialect(QString dialectname)
{
    QMutexLocker locker(&mMutex);
    if(mDialect && (dialectname == mDialect->getName()))
        return;

    ProtocolPtr dialect = mAvailableDialects[dialectname];
    if(!dialect)
    {
        CX_LOG_ERROR() << "\"" << dialectname << "\" is an unknown opentigtlink dialect.";
        return;
    }

    if(mDialect)
    {
        disconnect(mDialect.get(), &Protocol::image, this, &NetworkConnection::image);
        disconnect(mDialect.get(), &Protocol::mesh, this, &NetworkConnection::mesh);
        disconnect(mDialect.get(), &Protocol::transform, this, &NetworkConnection::transform);
        disconnect(mDialect.get(), &Protocol::calibration, this, &NetworkConnection::calibration);
        disconnect(mDialect.get(), &Protocol::probedefinition, this, &NetworkConnection::probedefinition);
        disconnect(mDialect.get(), &Protocol::usstatusmessage, this, &NetworkConnection::usstatusmessage);
        disconnect(mDialect.get(), &Protocol::igtlimage, this, &NetworkConnection::igtlimage);
    }

    mDialect = dialect;
    connect(dialect.get(), &Protocol::image, this, &NetworkConnection::image);
    connect(dialect.get(), &Protocol::mesh, this, &NetworkConnection::mesh);
    connect(dialect.get(), &Protocol::transform, this, &NetworkConnection::transform);
    connect(dialect.get(), &Protocol::calibration, this, &NetworkConnection::calibration);
    connect(dialect.get(), &Protocol::probedefinition, this, &NetworkConnection::probedefinition);
    connect(dialect.get(), &Protocol::usstatusmessage, this, &NetworkConnection::usstatusmessage);
    connect(dialect.get(), &Protocol::igtlimage, this, &NetworkConnection::igtlimage);

	CX_LOG_CHANNEL_SUCCESS(CX_OPENIGTLINK_CHANNEL_NAME) << "IGTL Dialect set to " << dialectname;

}

namespace
{
void write_send_info(igtl::ImageMessage::Pointer msg)
{
	int kb = msg->GetPackSize()/1024;
	CX_LOG_CHANNEL_DEBUG("igtl_test") << "Writing image to socket: " << msg->GetDeviceName()
									  << ", " << kb << " kByte";
}
}

void NetworkConnection::streamImage(ImagePtr image)
{
    QMutexLocker locker(&mMutex);

    IGTLinkConversionImage imageConverter;
    igtl::ImageMessage::Pointer msg = imageConverter.encode(image, mDialect->coordinateSystem());
    msg->Pack();

    int sendSize = msg->GetPackSize()/1024;
    int waitingSize = mSocket->bytesToWrite()/1024;
    double waitToSendRatio = double(waitingSize)/double(sendSize);
    double waitToSendRatioThreshold = 0.5;

    static int sentCounter =0;
    static int dropCounter =0;
//    CX_LOG_CHANNEL_DEBUG("igtl_test") << "ratio: " << waitToSendRatio << " ratio";
    CX_LOG_CHANNEL_DEBUG("igtl_test") << "GetNumComponents: " << msg->GetNumComponents() << " ";


    if (waitToSendRatio > waitToSendRatioThreshold)
    {
        dropCounter++;
        CX_LOG_CHANNEL_DEBUG("igtl_test") << QString("dropped stream image: wanted to send %1kB but %2kB already waiting, sent=%3,drop=%4")
                                             .arg(sendSize)
                                             .arg(waitingSize)
                                             .arg(sentCounter)
                                             .arg(dropCounter);

        return;
    }
    sentCounter++;

    CX_LOG_CHANNEL_DEBUG("igtl_test") << "bytes to write pre write: " << waitingSize << " kbyte";
    image->getBaseVtkImageData()->Print(std::cout);

    write_send_info(msg);
    mSocket->write(reinterpret_cast<char*>(msg->GetPackPointer()), msg->GetPackSize());
//    CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << "Sent image: " << image->getName();
}

void NetworkConnection::sendImage(ImagePtr image)
{
	QMutexLocker locker(&mMutex);

//	image.reset(new Image("tmp", image->getGrayScaleVtkImageData()));

	IGTLinkConversionImage imageConverter;
	igtl::ImageMessage::Pointer msg = imageConverter.encode(image, mDialect->coordinateSystem());
	msg->Pack();

    int kb = mSocket->bytesToWrite()/1024;
    CX_LOG_CHANNEL_DEBUG("igtl_test") << "bytes to write pre write: " << kb << " kbyte";

	QImage qimg((uchar*)image->getBaseVtkImageData()->GetScalarPointer(), 640, 1136, QImage::Format_RGB888);
	CX_LOG_CHANNEL_DEBUG("CA") << "start saves";
	CX_LOG_CHANNEL_DEBUG("CA") << "save png: " << qimg.save("/Users/christiana/dev/temp.png");
	CX_LOG_CHANNEL_DEBUG("CA") << "saved png";
	for (int i=0; i<=100; i+=10)
		CX_LOG_CHANNEL_DEBUG("CA") << "save jpg: " << i << " success:" << qimg.save(QString("/Users/christiana/dev/temp%1.jpg").arg(i),"jpg", i);
	CX_LOG_CHANNEL_DEBUG("CA") << "save bmp: " << qimg.save("/Users/christiana/dev/temp.bmp");

	write_send_info(msg);
	CX_LOG_CHANNEL_DEBUG("CA") << QString("start compress s=%2 (%3)").arg(msg->GetPackSize()/1024).arg(msg->GetPackSize());
//	CX_LOG_CHANNEL_DEBUG("CA") << "start compress s=" << msg->GetPackSize()/1024;
	QByteArray compressed = qCompress(reinterpret_cast<const uchar*>(msg->GetPackPointer()), msg->GetPackSize(), -1);
	CX_LOG_CHANNEL_DEBUG("CA") << QString("  done compress %1: s=%2 (%3)").arg(-1).arg(compressed.size()/1024).arg(compressed.size());
//	CX_LOG_CHANNEL_DEBUG("CA") << "done compress -1: s=" << compressed.size();
	for (int i=0; i<10; ++i)
	{
		compressed = qCompress(reinterpret_cast<const uchar*>(msg->GetPackPointer()), msg->GetPackSize(), i);
		CX_LOG_CHANNEL_DEBUG("CA") << QString("  done compress %1: s=%2 (%3)").arg(i).arg(compressed.size()/1024).arg(compressed.size());
	}
//	compressed = qCompress(reinterpret_cast<const uchar*>(msg->GetPackPointer()), msg->GetPackSize(), 9);
//	CX_LOG_CHANNEL_DEBUG("CA") << "done compress 9: s=" << compressed.size()/1024;
	CX_LOG_CHANNEL_DEBUG("CA") << "stop compress";

	mSocket->write(reinterpret_cast<char*>(msg->GetPackPointer()), msg->GetPackSize());
//    CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << "Sent image: " << image->getName();
}

void NetworkConnection::sendMesh(MeshPtr data)
{
	QMutexLocker locker(&mMutex);

	IGTLinkConversionPolyData polyConverter;
	igtl::PolyDataMessage::Pointer msg = polyConverter.encode(data, mDialect->coordinateSystem());
	CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << "Sending mesh: " << data->getName();
	msg->Pack();

	mSocket->write(reinterpret_cast<char*>(msg->GetPackPointer()), msg->GetPackSize());
}

/*
void NetworkConnection::sendStringMessage(QString command)
{
    QMutexLocker locker(&mMutex);

    OpenIGTLinkConversion converter;
    igtl::StringMessage::Pointer stringMsg = converter.encode(command);
    CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << "Sending string: " << command;
    stringMsg->Pack();

    mSocket->write(reinterpret_cast<char*>(stringMsg->GetPackPointer()), stringMsg->GetPackSize());
}
*/

void NetworkConnection::internalDataAvailable()
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
void NetworkConnection::queryServer()
{
    QString command = "<Command Name=\"RequestChannelIds\" />";
    this->sendStringMessage(command);

    command = "<Command Name=\"RequestDeviceIds\" />";
    this->sendStringMessage(command);

    command = "<Command Name=\"SaveConfig\" />";
    this->sendStringMessage(command);
}
*/

bool NetworkConnection::receiveHeader(const igtl::MessageHeader::Pointer header) const
{
    header->InitPack();

    if(!this->socketReceive(header->GetPackPointer(), header->GetPackSize()))
        return false;

    int c = header->Unpack(1);
    if (c & igtl::MessageHeader::UNPACK_HEADER)
    {
        std::string deviceType = std::string(header->GetDeviceType());
        return true;
    }
    else
        return false;
}

bool NetworkConnection::receiveBody(const igtl::MessageBase::Pointer header)
{
	QString type = QString(header->GetDeviceType()).toUpper();
	if (type=="TRANSFORM")
    {
        if(!this->receive<igtl::TransformMessage>(header))
            return false;
    }
	else if (type=="POLYDATA")
	{
		if(!this->receive<igtl::PolyDataMessage>(header))
			return false;
	}
	else if (type=="IMAGE")
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
	else if (type=="STATUS")
    {
        if(!this->receive<igtl::StatusMessage>(header))
            return false;
    }
	else if (type=="STRING")
    {
        if(!this->receive<igtl::StringMessage>(header))
            return false;
    }
	else if (type=="CX_US_ST")
    {
        if(!this->receive<IGTLinkUSStatusMessage>(header))
            return false;
    }
    else
    {
        CX_LOG_CHANNEL_WARNING(CX_OPENIGTLINK_CHANNEL_NAME) << "Skipping message of type " << type;
        igtl::MessageBase::Pointer body = igtl::MessageBase::New();
        body->SetMessageHeader(header);
		this->skip(body->GetBodySizeToRead());
    }
    return true;
}

qint64 NetworkConnection::skip(qint64 maxSizeBytes) const
{
	char *voidData = new char[maxSizeBytes];
	int retval = mSocket->read(voidData, maxSizeBytes);
	delete[] voidData;
	return retval;
}


template <typename T>
bool NetworkConnection::receive(const igtl::MessageBase::Pointer header)
{
    QMutexLocker locker(&mMutex);

    typename T::Pointer body = T::New();
    body->SetMessageHeader(header);
    body->AllocatePack();

    if(!this->socketReceive(body->GetPackBodyPointer(), body->GetPackBodySize()))
        return false;

    int c = body->Unpack(mDialect->doCRC());
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


}//namespace cx
