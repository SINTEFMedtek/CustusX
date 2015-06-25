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

void CustusDialect::translate(const IGTLinkImageMessage::Pointer body)
{
    emit igtlimage(body);
}

void CustusDialect::translate(const cx::IGTLinkUSStatusMessage::Pointer body)
{
    emit usstatusmessage(body);
}

}
