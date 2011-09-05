/*
 * cxIGTLinkSonixStatusMessage.cpp
 *
 *  Created on: Aug 12, 2011
 *      Author: olevs
 */

#include "cxIGTLinkSonixStatusMessage.h"
#include "igtl_header.h"
#include <string.h>

namespace cx
{
IGTLinkSonixStatusMessage::IGTLinkSonixStatusMessage():
  igtl::MessageBase(),
  mNewStatus(false),
  mWidth(0.0)
{
  for (int i = 0; i < 3; i ++)
  {
    mDataOrigin[i] = 0.0;
  }

  m_DefaultBodyType  = "SONIX_STATUS";
}


IGTLinkSonixStatusMessage::~IGTLinkSonixStatusMessage()
{}

void IGTLinkSonixStatusMessage::SetOrigin(double o[3])
{
  mDataOrigin[0] = o[0];
  mDataOrigin[1] = o[1];
  mDataOrigin[2] = o[2];
}
void IGTLinkSonixStatusMessage::SetOrigin(double oi, double oj, double ok)
{
  mDataOrigin[0] = oi;
  mDataOrigin[1] = oj;
  mDataOrigin[2] = ok;
}
void IGTLinkSonixStatusMessage::GetOrigin(double o[3])
{
  o[0] = mDataOrigin[0];
  o[1] = mDataOrigin[1];
  o[2] = mDataOrigin[2];
}
void IGTLinkSonixStatusMessage::GetOrigin(double &oi, double &oj, double &ok)
{
  oi = mDataOrigin[0];
  oj = mDataOrigin[1];
  ok = mDataOrigin[2];
}



int IGTLinkSonixStatusMessage::Pack()
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
