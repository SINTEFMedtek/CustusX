#include "cxOpenIGTLinkProtocol.h"

#include <QMutexLocker>
#include "cxLogger.h"
#include "cxIGTLinkConversionImage.h"
#include "cxIGTLinkConversionBase.h"
#include "cxIGTLinkConversion.h"
#include "cxIGTLinkConversionPolyData.h"
#include "cxStreamedTimestampSynchronizer.h"

namespace cx
{

OpenIGTLinkProtocol::OpenIGTLinkProtocol() :
    mHeader(igtl::MessageHeader::New()),
    mBody(igtl::MessageBase::New())
{
    this->getReadyToReceiveHeader();
}

bool OpenIGTLinkProtocol::doCRC() const
{
	return true;
}

bool OpenIGTLinkProtocol::readyToReceiveData()
{
//    QMutexLocker locker(&mReadyReadMutex);
//    QMutexLocker lock(&mPackMutex);
    return mReadyToReceive && mPack->isFinishedWith();
}

EncodedPackagePtr OpenIGTLinkProtocol::encode(ImagePtr image)
{
    IGTLinkConversionImage imageConverter;
    igtl::ImageMessage::Pointer msg = imageConverter.encode(image, this->coordinateSystem());
    msg->Pack();

	return igtlEncodedPackage<igtl::ImageMessage>::create(msg);
}

EncodedPackagePtr OpenIGTLinkProtocol::encode(MeshPtr data)
{
    IGTLinkConversionPolyData polyConverter;
    igtl::PolyDataMessage::Pointer msg = polyConverter.encode(data, this->coordinateSystem());
    msg->Pack();

	return igtlEncodedPackage<igtl::PolyDataMessage>::create(msg);
}

void OpenIGTLinkProtocol::translate(const igtl::MessageHeader::Pointer &header, const igtl::MessageBase::Pointer &body)
{
    //CX_LOG_DEBUG() << "Incoming message to OpenIGTLinkProtocol";
    QString type = QString(header->GetDeviceType()).toUpper();
    if (type=="TRANSFORM")
    {
        const igtl::TransformMessage::Pointer temp = dynamic_cast<igtl::TransformMessage*>(body.GetPointer());
        this->translate(temp);
    }
    else if (type=="POLYDATA")
    {
        const igtl::PolyDataMessage::Pointer temp = dynamic_cast<igtl::PolyDataMessage*>(body.GetPointer());
        this->translate(temp);
    }
    else if (type=="IMAGE")
    {
//        //----- CustusX openigtlink server -----
//        //there is a special kind of image package coming from custusx
//        //server where crc is set to 0.
//        QString name(header->GetDeviceName());
//        if(name.contains("Sonix", Qt::CaseInsensitive))
//        {
//            const IGTLinkImageMessage::Pointer temp = dynamic_cast<IGTLinkImageMessage*>(body.GetPointer());
//            this->translate(temp);
//        }
//        //----------
//        else
//        {
            const igtl::ImageMessage::Pointer temp = dynamic_cast<igtl::ImageMessage*>(body.GetPointer());
            this->translate(temp);
//        }
    }
    else if (type=="STATUS")
    {
        const igtl::StatusMessage::Pointer temp = dynamic_cast<igtl::StatusMessage*>(body.GetPointer());
        this->translate(temp);
    }
    else if (type=="STRING")
    {
        const igtl::StringMessage::Pointer temp = dynamic_cast<igtl::StringMessage*>(body.GetPointer());
        this->translate(temp);
    }
    else if (type=="CX_US_ST")
    {
        const IGTLinkUSStatusMessage::Pointer temp = dynamic_cast<IGTLinkUSStatusMessage*>(body.GetPointer());
        this->translate(temp);
    }
    else
    {
        this->writeNotSupportedMessage(body);
    }
}

void OpenIGTLinkProtocol::translate(const igtl::TransformMessage::Pointer body)
{
    //CX_LOG_DEBUG() << "Transform incoming to OpenIGTLinkProtocol";
    QString deviceName = body->GetDeviceName();

    IGTLinkConversion converter;
    Transform3D prMs = converter.decode(body);

    IGTLinkConversionBase baseConverter;
    QDateTime timestamp = baseConverter.decode_timestamp(body);

    if (mStreamSynchronizer)
    {
        mStreamSynchronizer->addTimestamp(timestamp);
        timestamp = timestamp.addMSecs(mStreamSynchronizer->getShift());
    }

    emit transform(deviceName, prMs, timestamp.toMSecsSinceEpoch());
}

void OpenIGTLinkProtocol::translate(const igtl::PolyDataMessage::Pointer body)
{
    this->writeAcceptingMessage(body);

    IGTLinkConversionPolyData polyConverter;
    MeshPtr retval = polyConverter.decode(body, this->coordinateSystem());
    emit mesh(retval);
}

void OpenIGTLinkProtocol::translate(const igtl::ImageMessage::Pointer body)
{
    this->writeAcceptingMessage(body);

    IGTLinkConversionImage imageConverter;
    ImagePtr retval = imageConverter.decode(body);

    if (mStreamSynchronizer)
        mStreamSynchronizer->syncToCurrentTime(retval);

    emit image(retval);
}

void OpenIGTLinkProtocol::translate(const igtl::StatusMessage::Pointer body)
{
    IGTLinkConversion converter;
    QString status = converter.decode(body);
    CX_LOG_CHANNEL_VOLATILE(CX_OPENIGTLINK_CHANNEL_NAME) << "IGTL status: " << status;
}

void OpenIGTLinkProtocol::translate(igtl::StringMessage::Pointer body)
{
    IGTLinkConversion converter;
    QString string = converter.decode(body);
    //This was spamming the console
    CX_LOG_CHANNEL_INFO(CX_OPENIGTLINK_CHANNEL_NAME) << "IGTL string: " << string;
}

void OpenIGTLinkProtocol::translate(const IGTLinkUSStatusMessage::Pointer body)
{
    this->writeNotSupportedMessage(body);
}

void OpenIGTLinkProtocol::writeNotSupportedMessage(igtl::MessageBase* base) const
{
    QString dtype(base->GetDeviceType());
    QString dname(base->GetDeviceName());
    CX_LOG_CHANNEL_INFO(CX_OPENIGTLINK_CHANNEL_NAME) << QString("Ignoring incoming igtlink message (%1, %2): ")
                                                        .arg(dtype)
                                                        .arg(dname);
}

void OpenIGTLinkProtocol::writeAcceptingMessage(igtl::MessageBase* body) const
{
    QString dtype(body->GetDeviceType());
    QString dname(body->GetDeviceName());
    CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << QString("Accepting incoming igtlink message (%1, %2): ")
                                                        .arg(dtype)
                                                        .arg(dname);
}

void OpenIGTLinkProtocol::getReadyToReceiveBody()
{
    //CX_LOG_DEBUG() << "getting ready to receive BODY";
    this->prepareBody(mHeader, mBody);
    //CX_LOG_DEBUG() << "mBody->pointer " << mBody->GetPackBodyPointer() << " size " << mBody->GetPackBodySize();
    this->prepareBodyPack(mBody);
    this->setReadyToReceive(true);
}

void OpenIGTLinkProtocol::getReadyToReceiveHeader()
{
    //CX_LOG_DEBUG() << "getting ready to receive HEADER";
    this->prepareHeader(mHeader);
    this->prepareHeaderPack(mHeader);
    this->setReadyToReceive(true);
}

bool OpenIGTLinkProtocol::isValid(const igtl::MessageBase::Pointer &msg) const
{
    if(this->isSupportedBodyType(msg->GetDeviceType()))
        return true;
    else
        return false;
}

bool OpenIGTLinkProtocol::isSupportedBodyType(QString type) const
{
    QStringList supportedTypes;
    supportedTypes << "TRANSFORM";
    supportedTypes << "POLYDATA";
    supportedTypes << "IMAGE";
    supportedTypes << "STATUS";
    supportedTypes << "STRING";
    supportedTypes << "CX_US_ST";

    if(supportedTypes.contains(type, Qt::CaseInsensitive))
        return true;
    else
        return false;

}

void OpenIGTLinkProtocol::processPack()
{
    //creates deadlock....
    //QMutexLocker lock(&mPackMutex);

    //CX_LOG_DEBUG() << "1 processPack ";
    this->setReadyToReceive(false);
    //CX_LOG_DEBUG() << "2 processPack ";
    if(mPack->data()->size == mHeader->GetPackSize())
    {
        this->unpackHeader(mHeader);
        CX_LOG_DEBUG() << "HEADER: " << mHeader->GetDeviceType();
        if(this->isValid(mHeader))
            this->getReadyToReceiveBody();
        else
            this->getReadyToReceiveHeader();
    }
    else
    {
        this->unpackBody(mBody);
        CX_LOG_DEBUG() << "BODY: " << mBody->GetDeviceType();
        if(this->isValid(mBody))
            this->getReadyToReceiveHeader();
        else
            this->getReadyToReceiveBody();
    }

}

void OpenIGTLinkProtocol::setReadyToReceive(bool ready)
{
    //CX_LOG_DEBUG() << (ready ? "Is " : "NOT ") << "ready to receive";
    mReadyToReceive = ready;

}

void OpenIGTLinkProtocol::prepareHeaderPack(igtl::MessageHeader::Pointer &message)
{
    if(mPack)
        disconnect(mPack.get(), &EncodedPackage::dataArrived, this, &OpenIGTLinkProtocol::processPack);
    mPack = igtlEncodedPackage<igtl::MessageHeader>::create(message);
    connect(mPack.get(), &EncodedPackage::dataArrived, this, &OpenIGTLinkProtocol::processPack);
}

void OpenIGTLinkProtocol::prepareBodyPack(igtl::MessageBase::Pointer &message)
{
    if(mPack)
        disconnect(mPack.get(), &EncodedPackage::dataArrived, this, &OpenIGTLinkProtocol::processPack);
    mPack = igtlEncodedBodyPackage::create(message);
    connect(mPack.get(), &EncodedPackage::dataArrived, this, &OpenIGTLinkProtocol::processPack);
}

void OpenIGTLinkProtocol::prepareHeader(const igtl::MessageHeader::Pointer &header) const
{
    header->InitPack();
    //CX_LOG_DEBUG() << "(    ) prepareHeader, pointer: " << header->GetPackPointer() << " size: " << header->GetPackSize();
    //CX_LOG_DEBUG() << "(BODY) prepareHeader, pointer: " << header->GetPackBodyPointer() << " size: " << header->GetPackBodySize();
}

void OpenIGTLinkProtocol::prepareBody(const igtl::MessageHeader::Pointer &header, igtl::MessageBase::Pointer &body)
{
    QString type = QString(header->GetDeviceType()).toUpper();
    if (type=="TRANSFORM")
    {
        this->prepareBody<igtl::TransformMessage>(header, body);
    }
    else if (type=="POLYDATA")
    {
        this->prepareBody<igtl::PolyDataMessage>(header, body);
    }
    else if (type=="IMAGE")
	{
		this->prepareBody<igtl::ImageMessage>(header, body);
    }
    else if (type=="STATUS")
    {
        this->prepareBody<igtl::StatusMessage>(header, body);
    }
    else if (type=="STRING")
    {
        this->prepareBody<igtl::StringMessage>(header, body);
    }
    else if (type=="CX_US_ST")
    {
        this->prepareBody<IGTLinkUSStatusMessage>(header, body);
    }
    else
    {
        this->writeNotSupportedMessage(header);
    }
}

bool OpenIGTLinkProtocol::unpackHeader(const igtl::MessageHeader::Pointer &header) const
{
    int doCRCOnHeader = 1;
    int c = header->Unpack(doCRCOnHeader);
    if (c & igtl::MessageHeader::UNPACK_HEADER)
    {
        //CX_LOG_DEBUG() << "unpacked header of type " << header->GetDeviceType();
        return true;
    }
    else
    {
        CX_LOG_CHANNEL_ERROR(CX_OPENIGTLINK_CHANNEL_NAME) << "Could not unpack header";
        return false;
    }
}


template <typename T>
void OpenIGTLinkProtocol::prepareBody(const igtl::MessageHeader::Pointer &header, igtl::MessageBase::Pointer &body)
{
    body = T::New();
    body->SetMessageHeader(header);
    body->AllocatePack();
    //CX_LOG_DEBUG() << "(    ) prepareBody, pointer: " << body->GetPackPointer() << " size: " << body->GetPackSize();
    //CX_LOG_DEBUG() << "(BODY) prepareBody, pointer: " << body->GetPackBodyPointer() << " size: " << body->GetPackBodySize();
}

bool OpenIGTLinkProtocol::unpackBody(const igtl::MessageBase::Pointer &body)
{
    int c = body->Unpack(this->doCRC());
    if (c & igtl::MessageHeader::UNPACK_BODY)
    {
        this->translate(mHeader, body);
    }
    else
    {
        CX_LOG_CHANNEL_ERROR(CX_OPENIGTLINK_CHANNEL_NAME) << "Could not unpack the body of type: " << body->GetDeviceType();
        return false;
    }
    return true;
}
} //namespace cx

