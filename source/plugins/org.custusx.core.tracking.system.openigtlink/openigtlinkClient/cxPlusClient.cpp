#include "cxPlusClient.h"

#include "cxIGTLinkConversion.h"

namespace cx{

void PlusClient::process(const igtl::TransformMessage::Pointer body)
{
    QString deviceName = body->GetDeviceName();

    IGTLinkConversion converter;
    Transform3D transform3D = converter.decode(body);

    igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();
    body->GetTimeStamp(ts);
    double timestamp_ms = ts->GetTimeStamp()*1000; //since epoch

    emit transform(deviceName, transform3D, timestamp_ms);
}

void PlusClient::process(const igtl::ImageMessage::Pointer body)
{
    IGTLinkConversion converter;
    ImagePtr theImage = converter.decode(body);
    emit image(theImage);

    CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << "TODO need to set devicename correctly for calibration";
    QString devicename = "ProbeToTracker"; //TODO remove hardcoded devicename
    Transform3D sMt = converter.decode_image_matrix(body);
    emit calibration(devicename, sMt);
}

void PlusClient::process(const igtl::StatusMessage::Pointer body)
{
    IGTLinkConversion converter;
    QString status = converter.decode(body);
    CX_LOG_CHANNEL_VOLATILE(CX_OPENIGTLINK_CHANNEL_NAME) << status;
}

void PlusClient::process(const igtl::StringMessage::Pointer body)
{
    IGTLinkConversion converter;
    QString string = converter.decode(body);
    //This was spamming the console
    //CX_LOG_CHANNEL_INFO(CX_OPENIGTLINK_CHANNEL_NAME) << string;
}


} //namespace cx
