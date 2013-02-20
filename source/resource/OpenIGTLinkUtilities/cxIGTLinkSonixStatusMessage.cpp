/*
 * cxIGTLinkSonixStatusMessage.cpp
 *
 *  \date Aug 12, 2011
 *      \author olevs
 */

#include "cxIGTLinkSonixStatusMessage.h"
#include "igtl_header.h"
#include <string.h>
#include "igtl_sonix_status.h"

namespace cx
{
IGTLinkSonixStatusMessage::IGTLinkSonixStatusMessage():
  igtl::MessageBase(),
  mNewStatus(0)
//  mWidth(0.0)
{
	AllocatePack();
	m_StatusMessage = m_Body;

	m_StatusMessage = NULL;

  for (int i = 0; i < 3; i ++)
  {
    mDataOrigin[i] = 0.0;
    mDataSpacing[i] = 0.0;
  }

 	mROI.ulx = 0;
 	mROI.uly = 0;
 	mROI.urx = 0;
 	mROI.ury = 0;
 	mROI.brx = 0;
 	mROI.bry = 0;
 	mROI.blx = 0;
 	mROI.bly = 0;


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
void IGTLinkSonixStatusMessage::GetOrigin(float  o[3])
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
void IGTLinkSonixStatusMessage::SetSpacing(double si, double sj, double sk)
{
  mDataSpacing[0] = si;
  mDataSpacing[1] = sj;
  mDataSpacing[2] = sk;
}
void IGTLinkSonixStatusMessage::GetSpacing(double s[3])
{
  s[0] = mDataSpacing[0];
  s[1] = mDataSpacing[1];
  s[2] = mDataSpacing[2];
}
void IGTLinkSonixStatusMessage::SetROI(int ulx, int uly, int urx, int ury, int brx, int bry, int blx, int bly)
{
	mROI.ulx = ulx;
	mROI.uly = uly;
	mROI.urx = urx;
	mROI.ury = ury;
	mROI.brx = brx;
	mROI.bry = bry;
	mROI.blx = blx;
	mROI.bly = bly;
}
void IGTLinkSonixStatusMessage::GetROI(int o[8])
{
  o[0] = mROI.ulx;
  o[1] = mROI.uly;
  o[2] = mROI.urx;
  o[3] = mROI.ury;
  o[4] = mROI.brx;
  o[5] = mROI.bry;
  o[6] = mROI.blx;
  o[7] = mROI.bly;
}

int IGTLinkSonixStatusMessage::PackBody()
{
	// Allocate pack
	AllocatePack();
	m_StatusMessage = this->m_Body;

	// Set pointers
	igtl_sonix_status_message* statusMessage = (igtl_sonix_status_message*)this->m_StatusMessage;

	//Copy data
	statusMessage->spacingX = static_cast<igtl_float64>(this->mDataSpacing[0]);
	statusMessage->spacingY = static_cast<igtl_float64>(this->mDataSpacing[1]);
	statusMessage->spacingZ = static_cast<igtl_float64>(this->mDataSpacing[2]);

	statusMessage->oi = static_cast<igtl_float32>(this->mDataOrigin[0]);
	statusMessage->oj = static_cast<igtl_float32>(this->mDataOrigin[1]);
	statusMessage->ok = static_cast<igtl_float32>(this->mDataOrigin[2]);

	statusMessage->ulx = static_cast<igtl_int32>(this->mROI.ulx);
	statusMessage->uly = static_cast<igtl_int32>(this->mROI.uly);
	statusMessage->urx = static_cast<igtl_int32>(this->mROI.urx);
	statusMessage->ury = static_cast<igtl_int32>(this->mROI.ury);
	statusMessage->brx = static_cast<igtl_int32>(this->mROI.brx);
	statusMessage->bry = static_cast<igtl_int32>(this->mROI.bry);
	statusMessage->blx = static_cast<igtl_int32>(this->mROI.blx);
	statusMessage->bly = static_cast<igtl_int32>(this->mROI.bly);

	statusMessage->status = static_cast<igtl_uint16>(this->mNewStatus);

	/*int originMemSpace = sizeof(igtl_float64)*3;
	memcpy((*void)this->m_StatusMessage, (void*) mDataOrigin, originMemSpace);
	memcpy((*void)this->m_StatusMessage[originMemSpace], (void*) mNewStatus, sizeof(igtl_uint8));*/

	// Convert byte order from host to network
	igtl_sonix_status_convert_byte_order(statusMessage);

	return 1;
}

int IGTLinkSonixStatusMessage::GetBodyPackSize()
{
	return IGTL_SONIX_STATUS_HEADER_SIZE;
}

int IGTLinkSonixStatusMessage::UnpackBody()
{
	m_StatusMessage = this->m_Body;

	// Set pointers
	igtl_sonix_status_message* statusMessage = (igtl_sonix_status_message*)this->m_StatusMessage;

	// Convert byte order from network to host
	igtl_sonix_status_convert_byte_order(statusMessage);

	//Copy data
	this->mDataSpacing[0] = statusMessage->spacingX;
	this->mDataSpacing[1] = statusMessage->spacingY;
	this->mDataSpacing[2] = statusMessage->spacingZ;

	this->mDataOrigin[0] = statusMessage->oi;
	this->mDataOrigin[1] = statusMessage->oj;
	this->mDataOrigin[2] = statusMessage->ok;

	this->mROI.ulx = statusMessage->ulx;
	this->mROI.uly = statusMessage->uly;
	this->mROI.urx = statusMessage->urx;
	this->mROI.ury = statusMessage->ury;
	this->mROI.brx = statusMessage->brx;
	this->mROI.bry = statusMessage->bry;
	this->mROI.blx = statusMessage->blx;
	this->mROI.bly = statusMessage->bly;

	this->mNewStatus = static_cast<int>(statusMessage->status);

	return 1;
}

//May not need pack(), but we need PackBody(), UnPackBody() and GetBodyPackSize()
/*int IGTLinkSonixStatusMessage::Pack()
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
}*/

}//namespace cx
