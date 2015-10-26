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

#include "cxIGTLinkImageMessage.h"
//#include "igtlMessageBase.h"
//#include "igtl_header.h"
//#include <string.h>

//namespace cx
//{
//IGTLinkImageMessage::IGTLinkImageMessage():
//    igtl::ImageMessage()
//{}

//IGTLinkImageMessage::~IGTLinkImageMessage()
//{}

//int IGTLinkImageMessage::Pack()
//{
//  PackBody();
//  igtl::MessageBase::m_IsBodyUnpacked   = 0;

//  // pack header
//  igtl_header* h = (igtl_header*) igtl::MessageBase::m_Header;

//  //igtl_uint64 crc = crc64(0, 0, 0LL); // initial crc //SKIPPING CRC

//  h->version   = IGTL_HEADER_VERSION;

//  igtl_uint64 ts  =  igtl::MessageBase::m_TimeStampSec & 0xFFFFFFFF;
//  ts = (ts << 32) | (igtl::MessageBase::m_TimeStampSecFraction & 0xFFFFFFFF);

//  h->timestamp = ts;
//  h->body_size = GetBodyPackSize();
//  //h->crc       = crc64((unsigned char*)igtl::MessageBase::m_Body, GetBodyPackSize(), crc); //SKIPPING CRC
//  h->crc       = 0;

//  strncpy(h->name, igtl::MessageBase::m_DefaultBodyType.c_str(), 12);
//  // TODO: this does not allow creating pack with MessageBase class...

//  strncpy(h->device_name, igtl::MessageBase::m_DeviceName.c_str(), 20);

//  igtl_header_convert_byte_order(h);

//  igtl::MessageBase::m_IsHeaderUnpacked = 0;

//  return 1;
//}
//}//namespace cx
