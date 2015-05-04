#include "cxDialect.h"

#include "cxIGTLinkConversion.h"
#include "cxLogger.h"

namespace cx
{

Dialect::Dialect(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<Transform3D>("Transform3D");
    qRegisterMetaType<ImagePtr>("ImagePtr");
}

QString Dialect::getName() const
{
    return "Basic";
}

void Dialect::translate(const igtl::TransformMessage::Pointer body)
{
    QString deviceName = body->GetDeviceName();

    IGTLinkConversion converter;
    Transform3D transform3D = converter.decode(body);

    igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();
    body->GetTimeStamp(ts);
    double timestamp_ms = ts->GetTimeStamp()*1000; //since epoch

    emit transform(deviceName, transform3D, timestamp_ms);
}

void Dialect::translate(const igtl::ImageMessage::Pointer body)
{
    Q_UNUSED(body);
}

void Dialect::translate(const igtl::StatusMessage::Pointer body)
{
    IGTLinkConversion converter;
    QString status = converter.decode(body);
    CX_LOG_CHANNEL_VOLATILE(CX_OPENIGTLINK_CHANNEL_NAME) << status;
}

void Dialect::translate(const igtl::StringMessage::Pointer body)
{
    IGTLinkConversion converter;
    QString string = converter.decode(body);
    //This was spamming the console
    //CX_LOG_CHANNEL_INFO(CX_OPENIGTLINK_CHANNEL_NAME) << string;
}

} //namespace cx
