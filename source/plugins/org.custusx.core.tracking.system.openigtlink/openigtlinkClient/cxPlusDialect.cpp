#include "cxPlusDialect.h"

#include "cxIGTLinkConversion.h"
#include "cxLogger.h"

namespace cx{


QString PlusDialect::getName() const
{
    return "PlusServer";
}

void PlusDialect::translate(const igtl::ImageMessage::Pointer body)
{
    IGTLinkConversion converter;
    ImagePtr theImage = converter.decode(body);
    emit image(theImage);

    //CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << "TODO need to set devicename correctly for calibration";
    QString devicename = "ProbeToTracker"; //TODO remove hardcoded devicename
    Transform3D sMt = converter.decode_image_matrix(body);
    emit calibration(devicename, sMt);
}

void PlusDialect::translate(const igtl::StringMessage::Pointer body)
{
    IGTLinkConversion converter;
    QString string = converter.decode(body);
    if(string.contains("CommandReply", Qt::CaseInsensitive))
        CX_LOG_CHANNEL_INFO(CX_OPENIGTLINK_CHANNEL_NAME) << string;
}

} //namespace cx
