/*
 * cxGrabberSender.cpp
 *
 *  Created on: Oct 11, 2012
 *      Author: christiana
 */

#include "cxGrabberSender.h"
#include "cxIGTLinkConversion.h"

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

void GrabberSenderQTcpSocket::send(IGTLinkImageMessage::Pointer msg)
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

void GrabberSenderQTcpSocket::send(ssc::ImagePtr msg)
{
	if (!this->isReady())
		return;

	IGTLinkConversion converter;
	this->send(converter.encode(msg));
}

void GrabberSenderQTcpSocket::send(ssc::ProbeData msg)
{
	if (!this->isReady())
		return;

	IGTLinkConversion converter;
	this->send(converter.encode(msg));
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

bool GrabberSenderDirectLink::isReady() const
{
	return true;
}

void GrabberSenderDirectLink::send(IGTLinkImageMessage::Pointer msg)
{
	if (!msg || !this->isReady())
		return;
	IGTLinkConversion converter;
	this->send(converter.decode(msg));
	if (mUnsentUSStatusMessage)
	{
		this->send(converter.decode(mUnsentUSStatusMessage, msg, mUSStatus));
		mUnsentUSStatusMessage = IGTLinkUSStatusMessage::Pointer();
	}
}
void GrabberSenderDirectLink::send(IGTLinkUSStatusMessage::Pointer msg)
{
	if (!msg || !this->isReady())
		return;
	mUnsentUSStatusMessage = msg;
}

void GrabberSenderDirectLink::send(ssc::ImagePtr msg)
{
	if (!this->isReady())
		return;

	mImage = msg;
	// decode color format:
	IGTLinkConversion converter;
	mImage = converter.decode(msg);

	emit newImage();
}

void GrabberSenderDirectLink::send(ssc::ProbeData msg)
{
	if (!this->isReady())
		return;
	mUSStatus = msg;
	emit newUSStatus();
}

ssc::ImagePtr GrabberSenderDirectLink::popImage()
{
	return mImage;
	mImage.reset();
}
ssc::ProbeData GrabberSenderDirectLink::popUSStatus()
{
	return mUSStatus;
//	mUSStatus = IGTLinkUSStatusMessage::Pointer();
}


} /* namespace cx */
