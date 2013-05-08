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


} /* namespace cx */
