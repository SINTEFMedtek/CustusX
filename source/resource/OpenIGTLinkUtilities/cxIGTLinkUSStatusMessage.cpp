// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxIGTLinkUSStatusMessage.h"
#include "igtl_header.h"
#include <string.h>
#include "igtl_us_status.h"

namespace cx
{

IGTLinkUSStatusMessage::IGTLinkUSStatusMessage():
		igtl::MessageBase(),
		mProbeType(0)
{
	AllocatePack();
	m_StatusMessage = m_Body;
	m_StatusMessage = NULL;

  for (int i = 0; i < 3; i ++)
  {
    mDataOrigin[i] = 0.0;
  }

  m_DefaultBodyType  = "CX_US_ST";
}

IGTLinkUSStatusMessage::~IGTLinkUSStatusMessage()
{}

void IGTLinkUSStatusMessage::SetProbeType(int probeType)
{
	mProbeType = probeType;
}
int IGTLinkUSStatusMessage::GetProbeType()
{
	return mProbeType;
}
void IGTLinkUSStatusMessage::SetOrigin(double o[3])
{
  mDataOrigin[0] = o[0];
  mDataOrigin[1] = o[1];
  mDataOrigin[2] = o[2];
}
void IGTLinkUSStatusMessage::SetOrigin(double oi, double oj, double ok)
{
  mDataOrigin[0] = oi;
  mDataOrigin[1] = oj;
  mDataOrigin[2] = ok;
}

void IGTLinkUSStatusMessage::GetOrigin(double  o[3])
{
  o[0] = mDataOrigin[0];
  o[1] = mDataOrigin[1];
  o[2] = mDataOrigin[2];
}

const double* IGTLinkUSStatusMessage::GetOrigin() const
{
	return mDataOrigin;
}

void IGTLinkUSStatusMessage::GetOrigin(double &oi, double &oj, double &ok)
{
  oi = mDataOrigin[0];
  oj = mDataOrigin[1];
  ok = mDataOrigin[2];
}

void IGTLinkUSStatusMessage::SetDepthStart(double depthStart)
{
	mDepthStart = depthStart;
}
double IGTLinkUSStatusMessage::GetDepthStart()
{
	return mDepthStart;
}
void IGTLinkUSStatusMessage::SetDepthEnd(double depthEnd)
{
	mDepthEnd = depthEnd;
}
double IGTLinkUSStatusMessage::GetDepthEnd()
{
	return mDepthEnd;
}
void IGTLinkUSStatusMessage::SetWidth(double width)
{
	mWidth = width;
}
double IGTLinkUSStatusMessage::GetWidth()
{
	return mWidth;
}
void IGTLinkUSStatusMessage::SetDataFormat(std::string dataFormat)
{
	mDataFormat = dataFormat;
}
std::string IGTLinkUSStatusMessage::GetDataFormat()
{
	return mDataFormat;
}

int IGTLinkUSStatusMessage::PackBody()
{
	// Allocate pack
	AllocatePack();
	m_StatusMessage = this->m_Body;

	// Set pointers
	igtl_us_status_message* statusMessage = (igtl_us_status_message*)this->m_StatusMessage;

	//Copy data
	statusMessage->originX    = static_cast<igtl_float64>(this->mDataOrigin[0]);
	statusMessage->originY    = static_cast<igtl_float64>(this->mDataOrigin[1]);
	statusMessage->originZ    = static_cast<igtl_float64>(this->mDataOrigin[2]);
	statusMessage->depthStart = static_cast<igtl_float64>(this->mDepthStart);
	statusMessage->depthEnd   = static_cast<igtl_float64>(this->mDepthEnd);
	statusMessage->width      = static_cast<igtl_float64>(this->mWidth);

	statusMessage->probeType  = static_cast<igtl_int32>(this->mProbeType);
	//TODO: string dataformat

	/*int originMemSpace = sizeof(igtl_float64)*3;
	memcpy((*void)this->m_StatusMessage, (void*) mDataOrigin, originMemSpace);
	memcpy((*void)this->m_StatusMessage[originMemSpace], (void*) mNewStatus, sizeof(igtl_uint8));*/

	// Convert byte order from host to network
	igtl_us_status_convert_byte_order(statusMessage);

	return 1;
}

int IGTLinkUSStatusMessage::GetBodyPackSize()
{
	return IGTL_US_STATUS_HEADER_SIZE;
}

int IGTLinkUSStatusMessage::UnpackBody()
{
	m_StatusMessage = this->m_Body;

	// Set pointers
	igtl_us_status_message* statusMessage = (igtl_us_status_message*)this->m_StatusMessage;

	// Convert byte order from network to host
	igtl_us_status_convert_byte_order(statusMessage);

	//Copy data
	this->mDataOrigin[0]  = statusMessage->originX;
	this->mDataOrigin[1]  = statusMessage->originY;
	this->mDataOrigin[2]  = statusMessage->originZ;
	this->mDepthStart     = statusMessage->depthStart;
	this->mDepthEnd       = statusMessage->depthEnd;
	this->mWidth          = statusMessage->width;
	this->mProbeType      = statusMessage->probeType;
	//TODO: dataformat

	return 1;
}
}
