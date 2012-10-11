/*
 * cxGrabberSender.cpp
 *
 *  Created on: Oct 11, 2012
 *      Author: christiana
 */

#include "/Users/christiana/christiana/workspace/CustusX3/CustusX3/modules/OpenIGTLinkServer/cxGrabberSender.h"

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
	mImage = msg;
	emit newImage();
}
void GrabberSenderDirectLink::send(IGTLinkUSStatusMessage::Pointer msg)
{
	if (!msg || !this->isReady())
		return;
	mUSStatus = msg;
	emit newUSStatus();
}

IGTLinkImageMessage::Pointer GrabberSenderDirectLink::popImage()
{
	return mImage;
	mImage = IGTLinkImageMessage::Pointer();
}
IGTLinkUSStatusMessage::Pointer GrabberSenderDirectLink::popUSStatus()
{
	return mUSStatus;
	mUSStatus = IGTLinkUSStatusMessage::Pointer();
}


} /* namespace cx */
