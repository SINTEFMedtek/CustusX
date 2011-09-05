#include "cxIGTLinkImageMessage.h"
#include "igtlMessageBase.h"
#include "igtl_header.h"
#include <string.h>

namespace cx
{
IGTLinkImageMessage::IGTLinkImageMessage():
    igtl::ImageMessage()
{}

IGTLinkImageMessage::~IGTLinkImageMessage()
{}

int IGTLinkImageMessage::Pack()
{
  PackBody();
  igtl::MessageBase::m_IsBodyUnpacked   = 0;

  // pack header
  igtl_header* h = (igtl_header*) igtl::MessageBase::m_Header;

  //igtl_uint64 crc = crc64(0, 0, 0LL); // initial crc //SKIPPING CRC

  h->version   = IGTL_HEADER_VERSION;

  igtl_uint64 ts  =  igtl::MessageBase::m_TimeStampSec & 0xFFFFFFFF;
  ts = (ts << 32) | (igtl::MessageBase::m_TimeStampSecFraction & 0xFFFFFFFF);

  h->timestamp = ts;
  h->body_size = GetBodyPackSize();
  //h->crc       = crc64((unsigned char*)igtl::MessageBase::m_Body, GetBodyPackSize(), crc); //SKIPPING CRC
  h->crc       = 0;

  strncpy(h->name, igtl::MessageBase::m_DefaultBodyType.c_str(), 12);
  // TODO: this does not allow creating pack with MessageBase class...

  strncpy(h->device_name, igtl::MessageBase::m_DeviceName.c_str(), 20);

  igtl_header_convert_byte_order(h);

  igtl::MessageBase::m_IsHeaderUnpacked = 0;

  return 1;
}
}//namespace cx
