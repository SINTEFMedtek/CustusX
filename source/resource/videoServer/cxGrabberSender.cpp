#include "cxGrabberSender.h"
#include "cxIGTLinkConversion.h"

namespace cx
{


void SenderImpl::send(PackagePtr package)
{
	if(package->mIgtLinkImageMessage)
		this->send(package->mIgtLinkImageMessage);

	if(package->mIgtLinkUSStatusMessage)
		this->send(package->mIgtLinkUSStatusMessage);

	if(package->mImage)
		this->send(package->mImage);

	if(package->mProbe)
		this->send(package->mProbe);
}

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

void GrabberSenderQTcpSocket::send(ssc::ProbeDataPtr msg)
{
	if (!this->isReady())
		return;

	IGTLinkConversion converter;
	this->send(converter.encode(msg));
}


} /* namespace cx */
