#include "cxCustusDialect.h"

#include "cxIGTLinkConversion.h"
#include "cxImage.h"
#include "cxLogger.h"

namespace cx
{

QString CustusDialect::getName() const
{
    return "Custus";
}

bool CustusDialect::doCRC() const
{
    //in the old version of the custusx openigtlink server
    //crc checking is disabled (for images)
    return false;
}

void CustusDialect::translate(const igtl::ImageMessage::Pointer body)
{
    IGTLinkConversion converter;
    ImagePtr theImage = converter.decode(body);
    emit image(theImage);
}

void CustusDialect::translate(const cx::IGTLinkUSStatusMessage::Pointer body)
{
    CX_LOG_DEBUG() << "IGTLinkUSStatusMessage received.";
    mUnsentUSStatusMessage = body;
}

}
