/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxGrabberSenderQTcpSocket.h"
#include "cxIGTLinkConversion.h"
#include "cxIGTLinkConversionImage.h"
#include "cxIGTLinkConversionSonixCXLegacy.h"

namespace cx
{

GrabberSenderQTcpSocket::GrabberSenderQTcpSocket(QTcpSocket* socket)
{
	mMaxBufferSize = 19200000; //800(width)*600(height)*4(bytes)*10(images)
	mSocket = socket;
}

bool GrabberSenderQTcpSocket::isReady() const
{
	if (!mSocket)
		return false;
	if (mSocket->bytesToWrite() > mMaxBufferSize)
		return false;;
	return true;
}

void GrabberSenderQTcpSocket::send(igtl::ImageMessage::Pointer msg)
{
	if (!msg || !this->isReady())
		return;

	// Pack (serialize) and send
	msg->Pack();
	mSocket->write(reinterpret_cast<const char*> (msg->GetPackPointer()), msg->GetPackSize());
}

void GrabberSenderQTcpSocket::send(IGTLinkUSStatusMessage::Pointer msg)
{
	if (!msg || !this->isReady())
		return;

	// Pack (serialize) and send
	msg->Pack();
	mSocket->write(reinterpret_cast<const char*> (msg->GetPackPointer()), msg->GetPackSize());
}

void GrabberSenderQTcpSocket::send(ImagePtr msg)
{
	if (!this->isReady())
		return;

	IGTLinkConversionImage converter;
	this->send(converter.encode(msg, pcsLPS));
}

void GrabberSenderQTcpSocket::send(ProbeDefinitionPtr msg)
{
	if (!this->isReady())
		return;

	IGTLinkConversion converter;
	this->send(converter.encode(msg));
}


} /* namespace cx */
